#include "PluginProcessor.h"
#include "PluginEditor.h"
RiderProcessor::RiderProcessor():AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true).withInput("Sidechain",juce::AudioChannelSet::stereo(),false).withOutput("Output",juce::AudioChannelSet::stereo(),true)),apvts(*this,nullptr,"VocalRiderK",makeParameters()) {}
juce::AudioProcessorValueTreeState::ParameterLayout RiderProcessor::makeParameters(){
    juce::AudioProcessorValueTreeState::ParameterLayout l;
    auto f=[&](const char* id,const char* name,float lo,float hi,float def,float step=0.1f){l.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id,1},name,juce::NormalisableRange<float>(lo,hi,step),def));};
    f("range","Range +/- (dB)",0,24,6);
    f("sensitivity","Sensitivity (%)",0,100,100,1);
    f("speed","Ride time (ms)",50,1500,300,1);
    f("gate","Silence threshold (dBFS)",-80,-20,-50);
    l.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"kmode",1},"K meter scale",juce::StringArray{"K-12","K-14","K-20"},1));
    for(auto spec: {std::pair<const char*,const char*>{"power","Power"},{"meterMode","Meter output / ride"},{"fix","FIX: automatic target leveling"},{"safe","SAFE: true-peak protection"},{"bypass","Bypass"}})
        l.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{spec.first,1},spec.second,(juce::String(spec.first)=="power"||juce::String(spec.first)=="fix"||juce::String(spec.first)=="safe")));
    f("target","Target RMS (dBFS)",-36,-6,-14);
    return l;
}
void RiderProcessor::prepareToPlay(double sr,int){rate=sr;safeLimiter.prepare(sr);reset();setLatencySamples(safeLimiter.latency());}
void RiderProcessor::reset(){energy=outputEnergy=0;ride=gate=0;musicEnergy=musicReference=musicReferenceSum=0;musicWarmup=musicReferenceCount=0;musicOffset=0;blend=apvts.getRawParameterValue("power")->load()>0.5f?1.0f:0.0f;currentGrDb=0;currentOutDb=-100;safeReductionDb=0;safeLimiter.reset(apvts.getRawParameterValue("safe")->load()>0.5f);}
bool RiderProcessor::isBusesLayoutSupported(const BusesLayout& b) const {
    const auto main=b.getMainOutputChannelSet();
    if(b.getMainInputChannelSet()!=main||(main!=juce::AudioChannelSet::mono()&&main!=juce::AudioChannelSet::stereo()))return false;
    if(b.inputBuses.size()>2||b.outputBuses.size()!=1)return false;
    if(b.inputBuses.size()>1){const auto sc=b.inputBuses[1];if(!sc.isDisabled()&&sc!=juce::AudioChannelSet::mono()&&sc!=juce::AudioChannelSet::stereo())return false;}
    return true;
}
void RiderProcessor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer& m){processAudio(b,m,false);}
void RiderProcessor::processBlockBypassed(juce::AudioBuffer<float>& b,juce::MidiBuffer& m){processAudio(b,m,true);}
juce::AudioProcessorParameter* RiderProcessor::getBypassParameter() const{return apvts.getParameter("bypass");}
void RiderProcessor::processAudio(juce::AudioBuffer<float>& b,juce::MidiBuffer& m,bool hostBypass){
    juce::ScopedNoDenormals nd;m.clear();
    auto v=[&](const char* id){return apvts.getRawParameterValue(id)->load(std::memory_order_relaxed);};
    const float target=v("target"), range=v("range"), tolerance=3.0f*(1.0f-v("sensitivity")*0.01f),floor=v("gate");
    const bool active=!hostBypass&&v("bypass")<0.5f&&v("power")>0.5f&&v("fix")>0.5f;
    const bool safeActive=!hostBypass&&v("bypass")<0.5f&&v("power")>0.5f&&v("safe")>0.5f;
    float safeMeter=safeReductionDb.load(std::memory_order_relaxed);
    const float safeMeterRelease=static_cast<float>(std::exp(-1.0/(rate*0.150)));
    const double rmsA=std::exp(-1.0/(rate*0.300));
    const float cutA=static_cast<float>(std::exp(-1.0/(rate*v("speed")*0.0005)));
    const float boostA=static_cast<float>(std::exp(-1.0/(rate*v("speed")*0.0015)));
    const float silenceA=static_cast<float>(std::exp(-1.0/(rate*0.8)));
    const float blendStep=static_cast<float>(1.0/(rate*0.01));
    const int channels=juce::jmin(b.getNumChannels(),getMainBusNumInputChannels());
    if(channels==0)return;
    const auto* scBus=getBus(true,1);
    const int scChannels=scBus&&scBus->isEnabled()?scBus->getNumberOfChannels():0;
    const bool hasSidechain=scChannels>0&&b.getNumChannels()>=channels+scChannels;
    const float musicPole=static_cast<float>(std::exp(-1.0/(rate*0.400)));
    if(!hasSidechain){musicEnergy=musicReference=musicReferenceSum=0;musicWarmup=musicReferenceCount=0;}
    for(int n=0;n<b.getNumSamples();++n){
        float requestedOffset=0;
        if(hasSidechain){
            double musicPower=0;
            for(int c=0;c<scChannels;++c){const double x=b.getSample(channels+c,n);musicPower+=x*x;}
            musicPower/=scChannels;musicEnergy=rmsA*musicEnergy+(1-rmsA)*musicPower;
            if(musicEnergy>5.0e-7){ // Ignore silence below approximately -60 dBFS, sine calibrated.
                if(musicReference==0){
                    if(++musicWarmup>static_cast<int>(rate*0.5)){musicReferenceSum+=musicPower;++musicReferenceCount;}
                    if(musicReferenceCount>=static_cast<int>(rate))musicReference=musicReferenceSum/musicReferenceCount;
                }else requestedOffset=juce::jlimit(-6.0f,6.0f,static_cast<float>(10*std::log10(musicEnergy/musicReference)));
            }
        }
        musicOffset=musicPole*musicOffset+(1-musicPole)*requestedOffset;
        double power=0;
        for(int c=0;c<channels;++c){const auto x=b.getSample(c,n);power+=static_cast<double>(x)*x;}
        power/=channels;energy=rmsA*energy+(1-rmsA)*power;
        const float level=static_cast<float>(10*std::log10(juce::jmax(energy,1.0e-20))+3.01029995664);
        // Hysteresis prevents rapid switching on noise around the silence threshold.
        if(level>floor+3)gate=1;else if(level<floor)gate=0;
        {const float error=target+musicOffset-level;
            // Soft deadband: preserve variations inside the selected tolerance.
            const float correction=std::copysign(juce::jmax(0.0f,std::abs(error)-tolerance),error);
            const float wanted=gate>0?juce::jlimit(-range,range,correction):0;
            const float a=gate>0?(wanted<ride?cutA:boostA):silenceA;ride=a*ride+(1-a)*wanted;}
        blend=juce::jlimit(0.0f,1.0f,blend+(active?blendStep:-blendStep));
        const float gain=juce::Decibels::decibelsToGain(ride*blend);
        double out=0;std::array<float,2> frame{};
        for(int c=0;c<channels;++c){float x=b.getSample(c,n);frame[static_cast<size_t>(c)]=blend==0?x:x*gain;}
        const float reduction=safeLimiter.process(frame,channels,safeActive);
        safeMeter=juce::jmax(reduction,safeMeter*safeMeterRelease);
        for(int c=0;c<channels;++c){const float y=frame[static_cast<size_t>(c)];b.setSample(c,n,y);out+=static_cast<double>(y)*y;}
        outputEnergy=rmsA*outputEnergy+(1-rmsA)*out/channels;
    }
    currentGrDb.store(ride*blend,std::memory_order_relaxed);
    safeReductionDb.store(safeMeter,std::memory_order_relaxed);
    currentOutDb.store(static_cast<float>(10*std::log10(juce::jmax(outputEnergy,1.0e-20))+3.01029995664),std::memory_order_relaxed);
}
juce::AudioProcessorEditor* RiderProcessor::createEditor(){return new RiderEditor(*this);}
void RiderProcessor::getStateInformation(juce::MemoryBlock& dest){auto xml=apvts.copyState().createXml();copyXmlToBinary(*xml,dest);}
void RiderProcessor::setStateInformation(const void* d,int size){
    auto xml=getXmlFromBinary(d,size);if(!xml||!xml->hasTagName(apvts.state.getType()))return;
    auto state=juce::ValueTree::fromXml(*xml);
    if(!state.getChildWithProperty("id","range").isValid()){
        auto up=state.getChildWithProperty("id","up"),down=state.getChildWithProperty("id","down");
        if(up.isValid()||down.isValid()){
            const float limit=juce::jlimit(0.0f,24.0f,juce::jmin(static_cast<float>(up.getProperty("value",6.0f)),static_cast<float>(down.getProperty("value",12.0f))));
            juce::ValueTree rangeState("PARAM");rangeState.setProperty("id","range",nullptr);rangeState.setProperty("value",limit,nullptr);state.appendChild(rangeState,nullptr);
            juce::ValueTree sensitivityState("PARAM");sensitivityState.setProperty("id","sensitivity",nullptr);sensitivityState.setProperty("value",100.0f,nullptr);state.appendChild(sensitivityState,nullptr);
            if(up.isValid())state.removeChild(up,nullptr);if(down.isValid())state.removeChild(down,nullptr);
        }
    }
    if(!state.getChildWithProperty("id","target").isValid()){
        const auto kState=state.getChildWithProperty("id","kmode");
        const int mode=juce::jlimit(0,2,static_cast<int>(kState.getProperty("value",1)));
        const float targets[]{-12,-14,-20};juce::ValueTree targetState("PARAM");
        targetState.setProperty("id","target",nullptr);targetState.setProperty("value",targets[mode],nullptr);state.appendChild(targetState,nullptr);
    }
    apvts.replaceState(state);
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new RiderProcessor();}
