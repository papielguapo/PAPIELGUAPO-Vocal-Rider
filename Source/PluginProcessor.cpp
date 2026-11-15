#include "PluginProcessor.h"
#include "PluginEditor.h"
RiderProcessor::RiderProcessor():AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true).withOutput("Output",juce::AudioChannelSet::stereo(),true)),apvts(*this,nullptr,"VocalRiderK",makeParameters()) {}
juce::AudioProcessorValueTreeState::ParameterLayout RiderProcessor::makeParameters(){
    juce::AudioProcessorValueTreeState::ParameterLayout l;
    auto f=[&](const char* id,const char* name,float lo,float hi,float def,float step=0.1f){l.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id,1},name,juce::NormalisableRange<float>(lo,hi,step),def));};
    f("up","Maximum boost (dB)",0,18,6);
    f("down","Maximum cut (dB)",0,24,12);
    f("speed","Ride time (ms)",50,1500,300,1);
    f("gate","Silence threshold (dBFS)",-80,-20,-50);
    l.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"kmode",1},"K reference",juce::StringArray{"K-12","K-14","K-20"},1));
    for(auto spec: {std::pair<const char*,const char*>{"power","Power"},{"meterMode","Meter output / ride"},{"fix","FIX: automatic 0 dB K"},{"safe","SAFE: true-peak protection"},{"bypass","Bypass"}})
        l.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{spec.first,1},spec.second,(juce::String(spec.first)=="power"||juce::String(spec.first)=="fix"||juce::String(spec.first)=="safe")));
    return l;
}
void RiderProcessor::prepareToPlay(double sr,int){rate=sr;safeLimiter.prepare(sr);reset();setLatencySamples(safeLimiter.latency());}
void RiderProcessor::reset(){energy=outputEnergy=0;ride=gate=0;blend=apvts.getRawParameterValue("power")->load()>0.5f?1.0f:0.0f;currentGrDb=0;currentOutDb=-100;safeReductionDb=0;safeLimiter.reset(apvts.getRawParameterValue("safe")->load()>0.5f);}
bool RiderProcessor::isBusesLayoutSupported(const BusesLayout& b) const {return b.getMainInputChannelSet()==b.getMainOutputChannelSet()&&(b.getMainOutputChannelSet()==juce::AudioChannelSet::mono()||b.getMainOutputChannelSet()==juce::AudioChannelSet::stereo());}
void RiderProcessor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer& m){processAudio(b,m,false);}
void RiderProcessor::processBlockBypassed(juce::AudioBuffer<float>& b,juce::MidiBuffer& m){processAudio(b,m,true);}
juce::AudioProcessorParameter* RiderProcessor::getBypassParameter() const{return apvts.getParameter("bypass");}
void RiderProcessor::processAudio(juce::AudioBuffer<float>& b,juce::MidiBuffer& m,bool hostBypass){
    juce::ScopedNoDenormals nd;m.clear();
    auto v=[&](const char* id){return apvts.getRawParameterValue(id)->load(std::memory_order_relaxed);};
    const float refs[]{12,14,20};const float ref=refs[juce::jlimit(0,2,juce::roundToInt(v("kmode")))];
    const float target=-ref, up=v("up"),down=v("down"),floor=v("gate");
    const bool active=!hostBypass&&v("bypass")<0.5f&&v("power")>0.5f&&v("fix")>0.5f;
    const bool safeActive=!hostBypass&&v("bypass")<0.5f&&v("power")>0.5f&&v("safe")>0.5f;
    float safeMeter=safeReductionDb.load(std::memory_order_relaxed);
    const float safeMeterRelease=static_cast<float>(std::exp(-1.0/(rate*0.150)));
    const double rmsA=std::exp(-1.0/(rate*0.300));
    const float gainA=static_cast<float>(std::exp(-1.0/(rate*v("speed")*0.001)));
    const float silenceA=static_cast<float>(std::exp(-1.0/(rate*0.8)));
    const float blendStep=static_cast<float>(1.0/(rate*0.01));
    const int channels=juce::jmin(b.getNumChannels(),getTotalNumInputChannels());
    if(channels==0)return;
    for(int n=0;n<b.getNumSamples();++n){
        double power=0;
        for(int c=0;c<channels;++c){const auto x=b.getSample(c,n);power+=static_cast<double>(x)*x;}
        power/=channels;energy=rmsA*energy+(1-rmsA)*power;
        const float level=static_cast<float>(10*std::log10(juce::jmax(energy,1.0e-20))+3.01029995664);
        // Hysteresis prevents rapid switching on noise around the silence threshold.
        if(level>floor+3)gate=1;else if(level<floor)gate=0;
        {const float wanted=gate>0?juce::jlimit(-down,up,target-level):0;
            const float a=gate>0?gainA:silenceA;ride=a*ride+(1-a)*wanted;}
        blend=juce::jlimit(0.0f,1.0f,blend+(active?blendStep:-blendStep));
        const float gain=juce::Decibels::decibelsToGain(ride*blend);
        double out=0;std::array<float,2> frame{};
        for(int c=0;c<channels;++c){float x=b.getSample(c,n);frame[static_cast<size_t>(c)]=blend==0?x:x*gain;}
        const float reduction=safeLimiter.process(frame,channels,safeActive);
        safeMeter=juce::jmax(reduction,safeMeter*safeMeterRelease);
        for(int c=0;c<channels;++c){const float y=frame[static_cast<size_t>(c)];b.setSample(c,n,y);out+=static_cast<double>(y)*y;}
        outputEnergy=rmsA*outputEnergy+(1-rmsA)*out/channels;
    }
    for(int c=channels;c<b.getNumChannels();++c)b.clear(c,0,b.getNumSamples());
    currentGrDb.store(ride*blend,std::memory_order_relaxed);
    safeReductionDb.store(safeMeter,std::memory_order_relaxed);
    currentOutDb.store(static_cast<float>(10*std::log10(juce::jmax(outputEnergy,1.0e-20))+3.01029995664),std::memory_order_relaxed);
}
juce::AudioProcessorEditor* RiderProcessor::createEditor(){return new RiderEditor(*this);}
void RiderProcessor::getStateInformation(juce::MemoryBlock& dest){auto xml=apvts.copyState().createXml();copyXmlToBinary(*xml,dest);}
void RiderProcessor::setStateInformation(const void* d,int size){auto xml=getXmlFromBinary(d,size);if(xml&&xml->hasTagName(apvts.state.getType()))apvts.replaceState(juce::ValueTree::fromXml(*xml));}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new RiderProcessor();}
