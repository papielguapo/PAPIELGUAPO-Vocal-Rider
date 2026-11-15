#pragma once
#include <random>
#include <fstream>

// Independent, longer 32x/129-tap reconstruction for verification, separate
// from the limiter's 8x/33-tap estimator.
double referenceTruePeak(const std::vector<float>& signal)
{
    constexpr double pi=3.14159265358979323846;
    double peak=0;
    for(float x:signal)peak=std::max(peak,std::abs(static_cast<double>(x)));
    for(int phase=1;phase<32;++phase){std::array<double,129> weights{};double sum=0;
        for(int k=-64;k<=64;++k){const double d=k-phase/32.0;
            const double w=std::abs(d)>64?0:0.42+0.5*std::cos(pi*d/64)+0.08*std::cos(2*pi*d/64);
            const double c=std::abs(d)<1e-14?1:std::sin(pi*d)/(pi*d);
            weights[static_cast<size_t>(k+64)]=w*c;sum+=w*c;}
        for(size_t n=64;n+65<signal.size();++n){double y=0;for(int k=-64;k<=64;++k)y+=signal[n+static_cast<size_t>(k)]*weights[static_cast<size_t>(k+64)];peak=std::max(peak,std::abs(y/sum));}}
    return 20*std::log10(std::max(peak,1e-20));
}
void safeTests()
{
    std::ofstream results("Validation/true-peak.txt");
    std::mt19937 rng(6720);std::uniform_real_distribution<float> noise(-1,1);
    for(double sr:{44100.0,48000.0,96000.0}){
        for(int kind=0;kind<8;++kind){
            RiderProcessor p;set(p,"fix",0);set(p,"safe",1);p.prepareToPlay(sr,257);
            std::vector<float> output;juce::MidiBuffer m;juce::AudioBuffer<float> buffer(2,257);
            constexpr int total=8192;
            for(int n=0;n<total;n+=257){const int count=std::min(257,total-n);buffer.setSize(2,count,false,false,true);
                for(int j=0;j<count;++j){const int t=n+j;float x=0;
                    if(t>=1024&&t<6144){const double phase=2*juce::MathConstants<double>::pi*(t-1024);
                        if(kind==0)x=static_cast<float>(2*std::sin(phase*0.25+juce::MathConstants<double>::pi/4));
                        if(kind==1)x=static_cast<float>(2*std::sin(phase*0.47));
                        if(kind==2)x=t%97==0?4.0f:0.0f;
                        if(kind==3)x=(t%32<16?2.0f:-2.0f);
                        if(kind==4)x=static_cast<float>(1.3*std::sin(phase*0.021)+1.7*std::sin(phase*0.197)+0.7*std::sin(phase*0.403));
                        if(kind==5)x=noise(rng)*3;
                        if(kind==6)x=static_cast<float>(2*std::sin(phase*0.49));
                        if(kind==7)x=static_cast<float>(2*std::sin(phase*0.499));}
                    buffer.setSample(0,j,x);buffer.setSample(1,j,x*0.5f);}
                p.processBlock(buffer,m);
                for(int j=0;j<count;++j){const float x=buffer.getSample(0,j);if(!std::isfinite(x))throw std::runtime_error("SAFE finite output");if(std::abs(buffer.getSample(1,j)-x*0.5f)>1e-6)throw std::runtime_error("SAFE stereo image");output.push_back(x);}}
            const double tp=referenceTruePeak(output);
            results<<"rate="<<sr<<" stimulus="<<kind<<" true_peak_dBTP="<<tp<<std::endl;
            if(tp> -1.0+0.02)throw std::runtime_error("SAFE exceeds -1 dBTP: "+std::to_string(tp));
        }
    }
    check(true,"SAFE -1 dBTP: 24 independent 32x stress measurements");
    {RiderProcessor p;set(p,"fix",0);set(p,"safe",0);p.prepareToPlay(48000,64);const int latency=p.getLatencySamples();
        check(latency>0&&latency<480,"fixed lookahead latency reported to host");
        juce::AudioBuffer<float> b(2,64);juce::MidiBuffer m;int found=-1;
        for(int n=0;n<1024;n+=64){b.clear();if(n==0){b.setSample(0,0,0.25f);b.setSample(1,0,0.125f);}p.processBlock(b,m);for(int j=0;j<64;++j)if(b.getSample(0,j)!=0){found=n+j;check(b.getSample(0,j)==0.25f,"SAFE off preserves original sample");}}
        check(found==latency,"SAFE off timing matches host compensation");}
    {RiderProcessor p;set(p,"fix",0);set(p,"safe",0);p.prepareToPlay(48000,257);check(run(p,0,2)>-3,"SAFE off does not limit peaks");}
}
