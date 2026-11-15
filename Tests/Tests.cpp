#include "../Source/PluginProcessor.h"
#include <iostream>
#include <stdexcept>
void set(RiderProcessor& p,const char* id,float v){auto* a=p.apvts.getParameter(id);a->setValueNotifyingHost(a->convertTo0to1(v));}
void check(bool b,const char* message){if(!b)throw std::runtime_error(message);std::cout<<"PASS "<<message<<std::endl;}
double run(RiderProcessor& p,double level,int seconds=8,int block=257,bool stereo=true){
    juce::AudioBuffer<float> b(stereo?2:1,block);juce::MidiBuffer m;double sum=0;int count=0;
    for(int n=0;n<48000*seconds;n+=block){int size=juce::jmin(block,48000*seconds-n);b.setSize(stereo?2:1,size,false,false,true);
        for(int j=0;j<size;++j){float x=static_cast<float>(std::pow(10,level/20)*std::sin(2*juce::MathConstants<double>::pi*1000*(n+j)/48000));b.setSample(0,j,x);if(stereo)b.setSample(1,j,x*0.5f);}
        p.processBlock(b,m);
        for(int j=0;j<size;++j){float x=b.getSample(0,j);if(!std::isfinite(x))throw std::runtime_error("nonfinite output");if(stereo&&std::abs(b.getSample(1,j)-x*0.5f)>1e-6)throw std::runtime_error("stereo link");if(n>48000*(seconds-1)){sum+=x*x;++count;}}
    }
    // Stereo mean power is 0.625 times channel 1 power.
    return 10*std::log10(sum/count*(stereo?0.625:1))+3.01029995664;
}
int main(){juce::ScopedJuceInitialiser_GUI gui;try{
    for(int mode=0;mode<3;++mode){RiderProcessor p;set(p,"kmode",static_cast<float>(mode));set(p,"up",18);p.prepareToPlay(48000,257);const double target=mode==0?-12:mode==1?-14:-20;check(std::abs(run(p,-24)-target)<0.15,"K mode converges to reference");}
    {RiderProcessor p;p.prepareToPlay(48000,257);double out=run(p,-35);check(out< -30&&p.currentGrDb.load()<=6.01f,"maximum boost respected");}
    {RiderProcessor p;set(p,"kmode",2);set(p,"down",4);p.prepareToPlay(48000,257);run(p,-3);check(std::abs(p.currentGrDb.load()+4)<0.05,"maximum cut respected");}
    {RiderProcessor p;p.prepareToPlay(48000,257);run(p,-110,3);check(std::abs(p.currentGrDb.load())<0.01,"silence does not gain boost");}
    {RiderProcessor p;set(p,"bypass",1);p.prepareToPlay(48000,257);const auto out=run(p,-25);check(std::abs(out-(-25+10*std::log10(0.625)))<0.02,"bypass unity");}
    {RiderProcessor p;p.prepareToPlay(48000,257);run(p,-25);set(p,"fix",0);const auto out=run(p,-25,2);check(std::abs(out-(-25+10*std::log10(0.625)))<0.03,"FIX off returns to unity");set(p,"fix",1);check(std::abs(run(p,-18)+14)<0.15,"FIX on returns to zero K");}
    {RiderProcessor p;set(p,"kmode",2);set(p,"up",10);juce::MemoryBlock data;p.getStateInformation(data);RiderProcessor q;q.setStateInformation(data.getData(),static_cast<int>(data.getSize()));check(q.apvts.getRawParameterValue("kmode")->load()==2&&q.apvts.getRawParameterValue("up")->load()==10,"session state recall");check(q.apvts.getParameter("target")==nullptr&&q.apvts.getParameter("trim")==nullptr,"target fixed at zero; no output trim");}
    {RiderProcessor p;juce::AudioProcessor::BusesLayout layout;layout.inputBuses.add(juce::AudioChannelSet::mono());layout.outputBuses.add(juce::AudioChannelSet::mono());check(p.setBusesLayout(layout),"mono supported");p.prepareToPlay(48000,257);check(std::abs(run(p,-18,8,257,false)+14)<0.15,"mono reference");}
    {RiderProcessor p;std::unique_ptr<juce::AudioProcessorEditor> e(p.createEditor());e->setSize(1634,252);auto img=e->createComponentSnapshot(e->getLocalBounds());juce::File f=juce::File::getCurrentWorkingDirectory().getChildFile("Preview.png");f.deleteFile();juce::FileOutputStream out(f);juce::PNGImageFormat png;png.writeImageToStream(img,out);}
    std::cout<<"ALL DSP TESTS PASSED"<<std::endl;return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<std::endl;return 1;}}
