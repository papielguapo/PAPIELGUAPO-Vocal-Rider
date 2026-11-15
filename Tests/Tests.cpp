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
#include "SafeTests.h"
#include "SidechainTests.h"
int main(){juce::ScopedJuceInitialiser_GUI gui;try{
    for(int mode=0;mode<3;++mode){RiderProcessor p;set(p,"kmode",static_cast<float>(mode));set(p,"target",-18);set(p,"range",24);p.prepareToPlay(48000,257);const double target=-18;check(std::abs(run(p,-24)-target)<0.15,"K meter selection does not alter target or audio");}
    {RiderProcessor p;p.prepareToPlay(48000,257);double out=run(p,-35);check(out< -30&&p.currentGrDb.load()<=6.01f,"maximum boost respected");}
    {RiderProcessor p;set(p,"kmode",2);set(p,"range",4);p.prepareToPlay(48000,257);run(p,-3);check(std::abs(p.currentGrDb.load()+4)<0.05,"maximum cut respected");}
    {RiderProcessor p;p.prepareToPlay(48000,257);run(p,-110,3);check(std::abs(p.currentGrDb.load())<0.01,"silence does not gain boost");}
    {RiderProcessor p;set(p,"bypass",1);p.prepareToPlay(48000,257);const auto out=run(p,-25);check(std::abs(out-(-25+10*std::log10(0.625)))<0.02,"bypass unity");}
    {RiderProcessor p;p.prepareToPlay(48000,257);run(p,-25);set(p,"fix",0);const auto out=run(p,-25,2);check(std::abs(out-(-25+10*std::log10(0.625)))<0.03,"FIX off returns to unity");set(p,"fix",1);check(std::abs(run(p,-18)+14)<0.15,"FIX on returns to zero K");}
    {RiderProcessor p;set(p,"kmode",2);set(p,"range",10);set(p,"sensitivity",35);set(p,"target",-22.5f);juce::MemoryBlock data;p.getStateInformation(data);RiderProcessor q;q.setStateInformation(data.getData(),static_cast<int>(data.getSize()));check(q.apvts.getRawParameterValue("kmode")->load()==2&&q.apvts.getRawParameterValue("range")->load()==10&&q.apvts.getRawParameterValue("sensitivity")->load()==35&&q.apvts.getRawParameterValue("target")->load()==-22.5f,"session state recall");check(q.apvts.getParameter("target")!=nullptr&&q.apvts.getParameter("trim")==nullptr,"adjustable target present; no output trim");}
    {RiderProcessor p;juce::AudioProcessor::BusesLayout layout;layout.inputBuses.add(juce::AudioChannelSet::mono());layout.inputBuses.add(juce::AudioChannelSet::disabled());layout.outputBuses.add(juce::AudioChannelSet::mono());check(p.setBusesLayout(layout),"mono supported");p.prepareToPlay(48000,257);check(std::abs(run(p,-18,8,257,false)+14)<0.15,"mono reference");}
    {RiderProcessor p;set(p,"range",24);set(p,"sensitivity",0);p.prepareToPlay(48000,257);
        const double input=-14-10*std::log10(0.625)+0.5;
        const double out=run(p,input);check(std::abs(out+13.5)<0.08&&std::abs(p.currentGrDb.load())<0.02,"low sensitivity preserves small variation inside tolerance");
        const double farOut=run(p,-24);check(std::abs(farOut+17)<0.15,"low sensitivity converges within +/-3 dB K tolerance");
        set(p,"sensitivity",100);check(std::abs(run(p,-24)+14)<0.15,"high sensitivity restores exact zero K target");}
    {RiderProcessor p;set(p,"range",0);p.prepareToPlay(48000,257);const double out=run(p,-24,2);check(std::abs(p.currentGrDb.load())<0.001&&std::abs(out-(-24+10*std::log10(0.625)))<0.03,"zero range disables rider correction");}
    {RiderProcessor p;auto state=p.apvts.copyState();state.removeChild(state.getChildWithProperty("id","range"),nullptr);state.removeChild(state.getChildWithProperty("id","sensitivity"),nullptr);
        for(auto spec:{std::pair<const char*,float>{"up",4.0f},{"down",10.0f}}){juce::ValueTree child("PARAM");child.setProperty("id",spec.first,nullptr);child.setProperty("value",spec.second,nullptr);state.appendChild(child,nullptr);}
        juce::MemoryBlock block;auto xml=state.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,block);p.setStateInformation(block.getData(),static_cast<int>(block.getSize()));
        check(p.apvts.getRawParameterValue("range")->load()==4&&p.apvts.getRawParameterValue("sensitivity")->load()==100,"old boost/cut state migrates to conservative symmetric range");}
    for(float target:{-24.0f,-18.0f,-12.0f}){RiderProcessor p;set(p,"range",24);set(p,"target",target);p.prepareToPlay(48000,257);check(std::abs(run(p,-24)-target)<0.15,"adjustable target converges to selected dBFS RMS level");}
    {RiderProcessor p;auto state=p.apvts.copyState();state.removeChild(state.getChildWithProperty("id","target"),nullptr);state.getChildWithProperty("id","kmode").setProperty("value",2,nullptr);juce::MemoryBlock data;auto xml=state.createXml();juce::AudioProcessor::copyXmlToBinary(*xml,data);p.setStateInformation(data.getData(),static_cast<int>(data.getSize()));check(p.apvts.getRawParameterValue("target")->load()==-20,"old K-20 state migrates to -20 dBFS target");}
    sidechainTests();
    safeTests();
    {RiderProcessor p;std::unique_ptr<juce::AudioProcessorEditor> e(p.createEditor());auto* face=e->getChildComponent(0);std::vector<juce::Label*> fields;
        for(int i=0;i<face->getNumChildComponents();++i)if(auto* label=dynamic_cast<juce::Label*>(face->getChildComponent(i)))fields.push_back(label);
        check(fields.size()==5,"five editable numeric readouts");
        auto type=[&](size_t i,const juce::String& value,bool cancel=false){fields[i]->showEditor();auto* editor=fields[i]->getCurrentTextEditor();if(!editor)throw std::runtime_error("numeric editor unavailable");editor->setText(value,false);fields[i]->hideEditor(cancel);};
        type(0,"9,5");check(p.apvts.getRawParameterValue("range")->load()==9.5f,"numeric entry accepts decimal comma");
        type(0,"99");check(p.apvts.getRawParameterValue("range")->load()==24,"numeric entry clamps to range");
        type(0,"invalid");check(p.apvts.getRawParameterValue("range")->load()==24,"invalid numeric input preserves value");
        type(0,"nan");check(p.apvts.getRawParameterValue("range")->load()==24,"nonfinite numeric input rejected");
        type(0,"5",true);check(p.apvts.getRawParameterValue("range")->load()==24,"cancel preserves original numeric value");
        type(1,"50%");type(2,"450ms");type(3,"-62dBFS");type(4,"-20dBFS");
        check(p.apvts.getRawParameterValue("sensitivity")->load()==50&&p.apvts.getRawParameterValue("speed")->load()==450&&p.apvts.getRawParameterValue("gate")->load()==-62&&p.apvts.getRawParameterValue("target")->load()==-20,"typed values update all remaining parameters");
        e->setSize(1634,252);e.reset();e.reset(p.createEditor());check(e->getWidth()==817&&e->getHeight()==126,"editor reopens at minimum after enlargement");}
    {RiderProcessor p;std::unique_ptr<juce::AudioProcessorEditor> e(p.createEditor());std::cout<<"Editor opening size: "<<e->getWidth()<<" x "<<e->getHeight()<<std::endl;{auto minimum=e->createComponentSnapshot(e->getLocalBounds());auto f=juce::File::getCurrentWorkingDirectory().getChildFile("Preview-Min.png");f.deleteFile();juce::FileOutputStream stream(f);juce::PNGImageFormat png;png.writeImageToStream(minimum,stream);}e->setSize(1634,252);auto img=e->createComponentSnapshot(e->getLocalBounds());juce::File f=juce::File::getCurrentWorkingDirectory().getChildFile("Preview.png");f.deleteFile();juce::FileOutputStream out(f);juce::PNGImageFormat png;png.writeImageToStream(img,out);}
    std::cout<<"ALL DSP TESTS PASSED"<<std::endl;return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<std::endl;return 1;}}
