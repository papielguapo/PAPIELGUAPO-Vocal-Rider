#pragma once
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdint>

// 8x windowed-sinc true-peak estimation. Audio remains at the native rate.
// A sliding minimum plus a moving average anticipates gain changes. Fixed
// delay includes the detector delay and a guard on both sides of each peak.
class SafeLimiter
{
public:
    void prepare(double sr)
    {
        attack=std::max(1,static_cast<int>(std::ceil(sr*0.003)));
        hold=attack+128; delay=attack+96;
        audio.assign(static_cast<size_t>(delay),{});
        average.assign(static_cast<size_t>(attack),1.0f);
        queue.assign(static_cast<size_t>(hold+2),{});
        release=static_cast<float>(std::exp(-1.0/(sr*0.080)));
        blendStep=static_cast<float>(1.0/(sr*0.010));
        constexpr double pi=3.14159265358979323846;
        for(int phase=0;phase<8;++phase){
            double coefficientSum=0;
            for(int k=0;k<65;++k){const double d=k-32+phase/8.0;
                const double sinc=std::abs(d)<1.0e-12?1.0:std::sin(pi*d)/(pi*d);
                const double window=std::abs(d)>32?0:0.42+0.5*std::cos(pi*d/32)+0.08*std::cos(2*pi*d/32);
                coeff[static_cast<size_t>(phase)][static_cast<size_t>(k)]=static_cast<float>(sinc*window);coefficientSum+=sinc*window;}
            for(auto& c:coeff[static_cast<size_t>(phase)])c=static_cast<float>(c/coefficientSum);
        }
        reset(true);
    }
    void reset(bool enabled)
    {
        for(auto& channel:history)channel.fill(0);
        for(auto& frame:audio)frame.fill(0);
        std::fill(average.begin(),average.end(),1.0f);
        historyPosition=audioPosition=averagePosition=head=tail=0;clock=0;
        sum=static_cast<double>(attack);envelope=1;blend=enabled?1.0f:0.0f;
    }
    int latency() const noexcept{return delay;}
    float process(std::array<float,2>& frame,int channels,bool enabled) noexcept
    {
        if(audio.empty())return 0;
        float peak=0;
        for(int c=0;c<channels;++c){auto& h=history[static_cast<size_t>(c)];h[static_cast<size_t>(historyPosition)]=frame[static_cast<size_t>(c)];
            for(int phase=0;phase<8;++phase){float value=0;int pos=historyPosition;
                for(int k=0;k<65;++k){value+=h[static_cast<size_t>(pos)]*coeff[static_cast<size_t>(phase)][static_cast<size_t>(k)];if(--pos<0)pos=64;}
                peak=std::max(peak,std::abs(value));}
        }
        historyPosition=(historyPosition+1)%65;
        // Conservative 1.2 dB reserve for near-Nyquist transients and gain modulation.
        constexpr float ceiling=0.776247117f; // -2.2 dBFS internal ceiling
        const float wanted=peak>ceiling?ceiling/peak:1.0f;
        const int capacity=static_cast<int>(queue.size());
        while(head!=tail&&queue[static_cast<size_t>(head)].time+static_cast<uint64_t>(hold)<clock)head=(head+1)%capacity;
        while(head!=tail){const int previous=(tail+capacity-1)%capacity;if(queue[static_cast<size_t>(previous)].gain<wanted)break;tail=previous;}
        queue[static_cast<size_t>(tail)]={clock,wanted};tail=(tail+1)%capacity;
        const float minimum=queue[static_cast<size_t>(head)].gain;
        sum+=minimum-average[static_cast<size_t>(averagePosition)];average[static_cast<size_t>(averagePosition)]=minimum;
        averagePosition=(averagePosition+1)%attack;
        const float smooth=std::clamp(static_cast<float>(sum/attack),0.0f,1.0f);
        envelope=smooth<envelope?smooth:release*envelope+(1-release)*smooth;
        blend=std::clamp(blend+(enabled?blendStep:-blendStep),0.0f,1.0f);
        const float gain=1+blend*(envelope-1);
        auto delayed=audio[static_cast<size_t>(audioPosition)];audio[static_cast<size_t>(audioPosition)]=frame;
        for(int c=0;c<channels;++c)frame[static_cast<size_t>(c)]=delayed[static_cast<size_t>(c)]*gain;
        audioPosition=(audioPosition+1)%delay;++clock;
        return gain>0?-20*std::log10(gain):120.0f;
    }
private:
    struct Entry{uint64_t time=0;float gain=1;};
    std::array<std::array<float,65>,8> coeff{};
    std::array<std::array<float,65>,2> history{};
    std::vector<std::array<float,2>> audio;
    std::vector<float> average;
    std::vector<Entry> queue;
    int attack=1,hold=129,delay=97,historyPosition=0,audioPosition=0,averagePosition=0,head=0,tail=0;
    uint64_t clock=0;
    double sum=1;
    float envelope=1,blend=1,release=0,blendStep=1;
};
