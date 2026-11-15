#pragma once
void sidechainTests()
{
    RiderProcessor p;auto layout=p.getBusesLayout();layout.inputBuses.set(1,juce::AudioChannelSet::stereo());
    check(p.setBusesLayout(layout),"stereo music sidechain layout supported");
    set(p,"range",24);p.prepareToPlay(48000,257);
    juce::AudioBuffer<float> buffer(4,257);juce::MidiBuffer midi;int time=0;
    auto runMusic=[&](double musicLevel,int seconds,bool silent=false){double energy=0;int measured=0;
        for(int n=0;n<48000*seconds;n+=257){const int count=std::min(257,48000*seconds-n);buffer.setSize(4,count,false,false,true);
            for(int j=0;j<count;++j){const float voice=static_cast<float>(std::pow(10,-24.0/20)*std::sin(2*juce::MathConstants<double>::pi*1000*(time+j)/48000));
                const float music=silent?0:static_cast<float>(std::pow(10,musicLevel/20)*std::sin(2*juce::MathConstants<double>::pi*170*(time+j)/48000));
                buffer.setSample(0,j,voice);buffer.setSample(1,j,voice);buffer.setSample(2,j,music);buffer.setSample(3,j,music);}
            const float scBefore=buffer.getSample(2,0);p.processBlock(buffer,midi);if(buffer.getSample(2,0)!=scBefore)throw std::runtime_error("sidechain input was overwritten");
            if(n>=48000*(seconds-1))for(int j=0;j<count;++j){const double x=buffer.getSample(0,j);energy+=x*x;++measured;}time+=count;}
        return 10*std::log10(energy/measured)+3.01029995664;};
    const double base=runMusic(-24,7), louder=runMusic(-20,7), softer=runMusic(-28,7);
    check(std::abs(base+14)<0.15,"music reference anchors around selected zero K");
    check(louder-base>3.6&&louder-base<4.4,"voice follows a 4 dB rise in instrumental");
    check(softer-base< -3.6&&softer-base> -4.4,"voice follows a 4 dB fall in instrumental");
    const double silent=runMusic(-24,8,true);check(std::abs(silent+14)<0.15,"silent sidechain returns smoothly to fixed zero K");
    check(p.getLatencySamples()==240,"sidechain adds no extra latency");
    layout=p.getBusesLayout();layout.inputBuses.set(1,juce::AudioChannelSet::disabled());check(p.setBusesLayout(layout),"music sidechain can be disconnected");
    p.prepareToPlay(48000,257);check(std::abs(run(p,-24)+14)<0.15,"disconnected sidechain restores zero K");
    RiderProcessor mono;auto monoLayout=mono.getBusesLayout();monoLayout.inputBuses.set(0,juce::AudioChannelSet::mono());monoLayout.outputBuses.set(0,juce::AudioChannelSet::mono());monoLayout.inputBuses.set(1,juce::AudioChannelSet::mono());
    check(mono.setBusesLayout(monoLayout),"mono voice and mono sidechain layout supported");
}
