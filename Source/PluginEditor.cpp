#include "PluginEditor.h"
#include <cmath>
#include <cstdlib>

namespace
{
juce::Colour colour(juce::uint32 hex) { return juce::Colour(hex); }
juce::Font font(const juce::String& name,float size,int style)
{
#if JUCE_MAJOR_VERSION >= 8
    return juce::Font(juce::FontOptions(name,size,style));
#else
    return juce::Font(name,size,style);
#endif
}
void text(juce::Graphics& g, const juce::String& s, float x, float y, float w, float h, float size = 10, bool bold = true)
{
    g.setFont(font("Arial", size, bold ? juce::Font::bold : juce::Font::plain));
    g.drawText(s, juce::Rectangle<float>(x,y,w,h), juce::Justification::centred, false);
}
void screw(juce::Graphics& g, float x, float y)
{
    g.setColour(juce::Colours::white.withAlpha(0.42f)); g.fillEllipse(x-15,y-14,31,31);
    g.setGradientFill(juce::ColourGradient(colour(0xff080b0c),x-8,y-12,colour(0xff353b3c),x+9,y+12,false));
    g.fillEllipse(x-15,y-15,30,30);
    g.setGradientFill(juce::ColourGradient(colour(0xff191f20),x-3,y-6,colour(0xff020404),x+4,y+5,true));
    g.fillEllipse(x-6,y-6,12,12);
}
void lamp(juce::Graphics& g, juce::Rectangle<float> r, bool on)
{
    const auto c=r.getCentre();
    if(on) { g.setGradientFill(juce::ColourGradient(colour(0xff9b26ff).withAlpha(0.35f),c.x,c.y,juce::Colours::transparentBlack,c.x+r.getWidth()*0.85f,c.y,true));g.fillEllipse(r.expanded(r.getWidth()*0.4f)); }
    g.setColour(juce::Colours::black.withAlpha(0.3f));g.fillEllipse(r.translated(2,3));
    g.setGradientFill(juce::ColourGradient(colour(0xff73349d),r.getX(),r.getY(),colour(0xff260438),r.getRight(),r.getBottom(),false));g.fillEllipse(r);
    auto inner=r.reduced(5);
    if(on) { auto jewel=juce::ColourGradient(colour(0xffdf9aff),c.x-10,c.y-13,colour(0xff490078),c.x+24,c.y+20,true);jewel.addColour(0.36,colour(0xffb839ff));jewel.addColour(0.74,colour(0xff7900ce));g.setGradientFill(jewel);g.fillEllipse(inner);g.setColour(colour(0xffdd8fff).withAlpha(0.23f));g.drawEllipse(inner.reduced(1),2); }
    else { g.setColour(colour(0xff240046));g.fillEllipse(inner); }
    g.setColour(juce::Colours::white.withAlpha(on?0.83f:0.12f));g.fillEllipse(r.getX()+13,r.getY()+12,r.getWidth()*0.30f,r.getHeight()*0.19f);
    if(on){g.setColour(colour(0xffdc8fff).withAlpha(0.36f));g.fillEllipse(c.x+4,c.y+11,13,7);}
}
}

void RiderLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float pos, float start, float end, juce::Slider& slider)
{
    const float r = static_cast<float>(juce::jmin(width, height)) * 0.46f;
    const float cx = static_cast<float>(x) + static_cast<float>(width) * 0.5f;
    const float cy = static_cast<float>(y) + static_cast<float>(height) * 0.5f;
    g.setColour(juce::Colours::black.withAlpha(0.38f));
    g.fillEllipse(cx-r+2,cy-r+5,r*2+2,r*2+2);
    g.setColour(colour(0xff24202c));g.fillEllipse(cx-r-1,cy-r-1,r*2+2,r*2+2);
    auto resin=juce::ColourGradient(colour(0xffaf40ff),cx-r*0.55f,cy-r*0.75f,
                                   colour(0xff200034),cx+r*0.8f,cy+r*0.9f,false);
    resin.addColour(0.34,colour(0xff7e12d5));resin.addColour(0.65,colour(0xff51008e));
    g.setGradientFill(resin);g.fillEllipse(cx-r,cy-r,r*2,r*2);
    g.setColour(colour(0xffdd9cff).withAlpha(0.8f));g.drawEllipse(cx-r+1.4f,cy-r+1.4f,r*2-2.8f,r*2-2.8f,1.1f);
    g.setColour(colour(0xff26003f).withAlpha(0.75f));g.drawEllipse(cx-r+4,cy-r+4,r*2-8,r*2-8,1.4f);
    juce::Graphics::ScopedSaveState save(g);
    g.addTransform(juce::AffineTransform::rotation(start+pos*(end-start)).translated(cx,cy));
    // Narrow concave channels follow the fin; the stationary resin skirt catches the light.
    for(float side : {-1.0f,1.0f})
    {
        juce::Path flute;
        flute.startNewSubPath(side*r*0.25f,-r*0.88f);
        flute.cubicTo(side*r*0.73f,-r*0.63f,side*r*0.77f,r*0.46f,side*r*0.30f,r*0.85f);
        flute.cubicTo(side*r*0.47f,r*0.25f,side*r*0.47f,-r*0.32f,side*r*0.25f,-r*0.88f);
        flute.closeSubPath();g.setColour(colour(0xff25003e));g.fillPath(flute);
        juce::Path rib;rib.startNewSubPath(side*r*0.55f,-r*0.67f);
        rib.cubicTo(side*r*0.77f,-r*0.27f,side*r*0.73f,r*0.32f,side*r*0.48f,r*0.67f);
        g.setColour(colour(0xffc45cff).withAlpha(side<0?0.56f:0.22f));g.strokePath(rib,juce::PathStrokeType(1.0f));
    }
    juce::Path fin;
    fin.startNewSubPath(-r*0.07f,-r*0.94f);
    fin.cubicTo(r*0.12f,-r*0.97f,r*0.18f,-r*0.70f,r*0.19f,-r*0.25f);
    fin.lineTo(r*0.21f,r*0.58f);
    fin.quadraticTo(r*0.24f,r*0.86f,r*0.04f,r*0.91f);
    fin.quadraticTo(-r*0.18f,r*0.94f,-r*0.20f,r*0.68f);
    fin.lineTo(-r*0.19f,-r*0.60f);
    fin.quadraticTo(-r*0.19f,-r*0.89f,-r*0.07f,-r*0.94f);fin.closeSubPath();
    auto ridge=juce::ColourGradient(colour(0xffe3a2ff),-r*0.20f,0,colour(0xff3c0065),r*0.23f,0,false);
    ridge.addColour(0.23,colour(0xffb14cff));ridge.addColour(0.40,colour(0xff7b13c0));
    ridge.addColour(0.63,colour(0xff6500a8));g.setGradientFill(ridge);g.fillPath(fin);
    g.setColour(colour(0xffcc82ff).withAlpha(0.72f));g.strokePath(fin,juce::PathStrokeType(0.8f));
    g.setColour(colour(0xfff8efff));g.fillRoundedRectangle(-r*0.09f,-r*0.87f,r*0.075f,r*0.49f,0.7f);
    if(slider.hasKeyboardFocus(true)) {g.setColour(colour(0xffefd4ff));g.drawEllipse(-r-3,-r-3,r*2+6,r*2+6,1);}
}

void RiderLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& b, bool hover, bool down)
{
    const auto c=b.getLocalBounds().toFloat().getCentre();
    const bool right=b.getToggleState()!=static_cast<bool>(b.getProperties().getWithDefault("reverse",false));
    const float bx=c.x+3, tip=bx+(right?12.0f:-12.0f);
    g.setColour(juce::Colours::black.withAlpha(0.28f));g.fillEllipse(bx-12,c.y-11,27,28);
    g.setGradientFill(juce::ColourGradient(colour(0xff555b5e),bx-8,c.y-12,colour(0xff020506),bx+9,c.y+13,false));
    g.fillEllipse(bx-13,c.y-13,26,26);
    g.setColour(colour(0xff090c0d));g.fillEllipse(bx-10,c.y-10,20,20);
    g.setGradientFill(juce::ColourGradient(colour(0xff4c555b),tip,c.y-7,colour(0xff080b0d),tip,c.y+7,false));
    g.fillRoundedRectangle(juce::jmin(tip,bx)-5,c.y-7,std::abs(tip-bx)+10,14,6);
    g.setColour(colour(0xff030607));g.fillEllipse(tip-8,c.y-8,16,16);
    g.setGradientFill(juce::ColourGradient(colour(0xff515a60),tip-3,c.y-5,colour(0xff101416),tip+4,c.y+5,false));
    g.fillEllipse(tip-5,c.y-5.5f,10,11);
    if(b.getName()=="safe"&&b.getToggleState()){
        g.setColour(colour(0xffb84dff).withAlpha(0.25f));g.drawEllipse(bx-14,c.y-14,28,28,3);
        g.setColour(colour(0xffc77aff).withAlpha(0.8f));g.drawEllipse(bx-12,c.y-12,24,24,1);
    }
    if(hover||down||b.hasKeyboardFocus(true)){g.setColour(colour(0xffb84dff).withAlpha(0.65f));g.drawEllipse(bx-15,c.y-15,30,30,1);}
}
FaceplateComponent::FaceplateComponent(RiderProcessor& p):processor(p)
{
    setLookAndFeel(&look);
    const char* ids[]{"range","sensitivity","speed","gate","kmode"};
    const char* names[]{"RANGE +/- dB","SENSITIVITY percent","RIDE ms","SILENCE dBFS","K reference"};
    for(size_t i=0;i<knobs.size();++i){auto& s=knobs[i];addAndMakeVisible(s);s.setName(names[i]);s.setTitle(names[i]);
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);s.setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);
        s.setColour(juce::Slider::textBoxTextColourId,juce::Colour(0xff22202b));s.setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::transparentBlack);
        s.setRotaryParameters(juce::MathConstants<float>::pi*7/6,juce::MathConstants<float>::pi*17/6,true);
        s.setPopupDisplayEnabled(true,true,this);s.setMouseDragSensitivity(180);
        const double defaults[]{6,100,300,-50,1};s.setDoubleClickReturnValue(true,defaults[i]);
        if(i==0)s.setTooltip("RANGE: maximum boost and cut, symmetrically. 6 dB means -6 to +6 dB.");
        if(i==1)s.setTooltip("SENSITIVITY: 100% seeks exact 0 dB K; 0% allows +/-3 dB around zero K. RIDE controls speed.");
        if(i==4){s.textFromValueFunction=[](double v){const char* labels[]{"K-12","K-14","K-20"};return juce::String(labels[juce::jlimit(0,2,juce::roundToInt(v))]);};s.setTooltip("Automatic target: 0 dB K. Select K-12, K-14 or K-20.");}
        knobAttachments[i]=std::make_unique<SA>(processor.apvts,ids[i],s);
        auto& value=values[i];addAndMakeVisible(value);value.setName(juce::String(names[i])+" value");
        value.setJustificationType(juce::Justification::centred);value.setFont(font("Arial",12,juce::Font::bold));
        value.setBorderSize(juce::BorderSize<int>(0));value.setEditable(false,true,false);
        value.setColour(juce::Label::textColourId,colour(0xff171c1f));
        value.setColour(juce::Label::backgroundColourId,juce::Colours::transparentBlack);
        value.setColour(juce::Label::textWhenEditingColourId,colour(0xff171c1f));
        value.setColour(juce::Label::backgroundWhenEditingColourId,colour(0xfff4eaff));
        value.setColour(juce::Label::outlineWhenEditingColourId,colour(0xff9c43d9));
        value.setTooltip(i==4?"Double-click and enter K-12, K-14 or K-20. Enter applies; Escape cancels.":"Double-click to type a value. Enter applies; Escape cancels. Values are limited to the knob range.");
        value.setText(s.getTextFromValue(s.getValue()),juce::dontSendNotification);
        value.onEditorShow=[this,i]{if(auto* editor=values[i].getCurrentTextEditor())editor->selectAll();};
        value.onTextChange=[this,i,parameter=processor.apvts.getParameter(ids[i])]{
            const auto original=knobs[i].getValue();double parsed=original;bool valid=false;
            auto input=values[i].getText().trim().replace(",",".");
            if(i==4){input=input.toUpperCase().removeCharacters("K- ");if(input=="12"){parsed=0;valid=true;}else if(input=="14"){parsed=1;valid=true;}else if(input=="20"){parsed=2;valid=true;}}
            else{input=input.toLowerCase();if(i==0)input=input.replace("db","");if(i==1)input=input.replace("%","");if(i==2)input=input.replace("ms","");if(i==3)input=input.replace("dbfs","");input=input.trim();
                char* end=nullptr;parsed=std::strtod(input.toRawUTF8(),&end);valid=!input.isEmpty()&&end&&*end=='\0'&&std::isfinite(parsed);}
            if(valid){knobs[i].setValue(parsed,juce::dontSendNotification);parameter->beginChangeGesture();parameter->setValueNotifyingHost(parameter->convertTo0to1(static_cast<float>(knobs[i].getValue())));parameter->endChangeGesture();}
            values[i].setText(knobs[i].getTextFromValue(knobs[i].getValue()),juce::dontSendNotification);
        };
    }
    const char* switchIds[]{"power","meterMode","fix","safe"};
    for(size_t i=0;i<switches.size();++i){auto& b=switches[i];addAndMakeVisible(b);b.setName(switchIds[i]);if(i==2)b.setTooltip("FIX: automatically ride the voice to 0 dB K. Off: no rider gain.");if(i==3)b.setTooltip("SAFE: 8x true-peak estimation, -1 dBTP protection with safety margin. Purple LED shows SAFE enabled and becomes brighter during attenuation.");b.setClickingTogglesState(true);if(i!=1)b.getProperties().set("reverse",true);switchAttachments[i]=std::make_unique<BA>(processor.apvts,switchIds[i],b);}
    startTimerHz(60);
}
FaceplateComponent::~FaceplateComponent() {stopTimer();setLookAndFeel(nullptr);}
void FaceplateComponent::resized()
{
    constexpr int centres[]{374,511,649,787,924};
    for(size_t i=0;i<5;++i){knobs[i].setBounds(centres[i]-40,80,80,80);values[i].setBounds(centres[i]-45,179,90,20);}
    switches[0].setBounds(204,137,50,36);switches[1].setBounds(1169,135,50,36);switches[2].setBounds(1299,135,50,36);
    switches[3].setBounds(1039,135,50,36);
}

void FaceplateComponent::timerCallback()
{
    shownGr=processor.currentGrDb.load(std::memory_order_relaxed);
    shownOut=processor.currentOutDb.load(std::memory_order_relaxed);
    for(size_t i=0;i<values.size();++i)if(!values[i].isBeingEdited())values[i].setText(knobs[i].getTextFromValue(knobs[i].getValue()),juce::dontSendNotification);
    repaint();
}
void FaceplateComponent::paintMeter(juce::Graphics& g){
    g.setColour(colour(0xff08090a));g.fillRoundedRectangle(1003,38,380,78,3);
    const bool out=processor.apvts.getRawParameterValue("meterMode")->load()>0.5f;
    const float refs[]{12,14,20};const float k=refs[juce::jlimit(0,2,juce::roundToInt(processor.apvts.getRawParameterValue("kmode")->load()))];
    const float value=out?shownOut+k:shownGr;
    for(int i=0;i<33;++i){const float db=-24.0f+i;const float x=1013+i*10.9f;
        const bool lit=out?value>=db:(value>=0?db>=0&&db<=value:db<=0&&db>=value);
        const auto c=colour(db>4?0xffff4545:db>=0?0xffffb94b:0xffbb5aff);
        g.setColour(c.withAlpha(lit?1.0f:0.15f));g.fillRect(x,66.0f,8.0f,15.0f);
        if(i%4==0){g.setColour(juce::Colours::white);text(g,juce::String(juce::roundToInt(db)),x-8,87,25,13,9);}}
    g.setColour(juce::Colours::white);text(g,(out?"OUTPUT ":"RIDE ")+juce::String(value,1)+(out?" dB K":" dB"),1020,43,345,17,12);
}

void FaceplateComponent::paint(juce::Graphics& g)
{
    g.fillAll(colour(0xff08090b));
    const juce::Rectangle<float> plate(5,4,1622,242);
    juce::Path outline;outline.addRoundedRectangle(plate,6);
    {
        juce::Graphics::ScopedSaveState save(g);g.reduceClipRegion(outline);
        auto aluminium=juce::ColourGradient(colour(0xffe4e6e9),0,4,colour(0xff8a9095),0,246,false);
        aluminium.addColour(0.08,colour(0xffe4e6e9));aluminium.addColour(0.35,colour(0xffdfe3e6));
        aluminium.addColour(0.64,colour(0xffd9dee1));aluminium.addColour(0.92,colour(0xffd1d6da));
        aluminium.addColour(0.96,colour(0xffa0a5aa));
        g.setGradientFill(aluminium);g.fillRect(plate);
        for(int y=5;y<246;++y)
        {
            const float alpha=y%4==0?0.28f:y%4==1?0.10f:y%4==2?0.025f:0.045f;
            g.setColour((y%4<2?juce::Colours::white:juce::Colours::black).withAlpha(alpha));
            g.drawHorizontalLine(y,5,1627);
        }
        // Subtle broad machining highlights retain the horizontal grain at any scale.
        for(int i=0;i<12;++i){g.setColour(juce::Colours::white.withAlpha(0.012f));g.fillRect(5.0f,9.0f+static_cast<float>(i)*19,1622.0f,7.0f);}
    }
    g.setColour(juce::Colours::white.withAlpha(0.6f));g.drawRoundedRectangle(plate.reduced(0.5f),5,1);
    g.setColour(juce::Colours::black.withAlpha(0.25f));g.drawRoundedRectangle(plate.reduced(2),4,1);
    for(float x:{59.0f,1575.0f}){g.setColour(juce::Colours::black.withAlpha(0.25f));g.drawVerticalLine(static_cast<int>(x),6,244);g.setColour(juce::Colours::white.withAlpha(0.24f));g.drawVerticalLine(static_cast<int>(x+1),6,244);}
    for(float x:{16.0f,1585.0f})for(float y:{28.0f,199.0f})
    {
        g.setColour(juce::Colours::white.withAlpha(0.8f));g.fillRoundedRectangle(x-1,y-1,33,24,12);
        g.setColour(colour(0xff474d50));g.fillRoundedRectangle(x,y,31,22,11);
        g.setColour(colour(0xff020506));g.fillRoundedRectangle(x+1,y+1,29,20,10);
    }
    for(auto point:{juce::Point<float>(165,53),{165,198},{1417,53},{1417,198}})screw(g,point.x,point.y);
    g.setColour(colour(0xff15191b));g.setFont(font("Arial Black",18,juce::Font::bold));
    g.drawText("VOCAL",68,121,104,23,juce::Justification::centredLeft);
    g.setColour(colour(0xff15191b));text(g,"RIDER K",68,146,104,15,10);
    lamp(g,{206,32,60,60},processor.apvts.getRawParameterValue("power")->load()>0.5f);
    g.setColour(colour(0xff171c1f));text(g,"POWER",197,102,78,18,14);
    text(g,"ON",176,145,35,17,11);text(g,"OFF",258,145,35,17,11);
    constexpr float centres[]{374,511,649,787,924};
    const char* titles[]{"RANGE / +/- dB","SENSITIVITY / %","RIDE / ms","SILENCE / dBFS","K-12 / K-14 / K-20"};
    const std::array<std::vector<double>,5> scaleValues{{
        {0,4,8,12,16,20,24}, {0,25,50,75,100},
        {50,100,300,600,1000,1500}, {-80,-70,-60,-50,-40,-30,-20}, {0,1,2}
    }};
    for(int i=0;i<5;++i){
        const float cx=centres[i], cy=120;
        auto point=[&](double proportion,float radius){
            const float a=juce::MathConstants<float>::pi*(7.0f/6.0f+5.0f/3.0f*static_cast<float>(proportion));
            return juce::Point<float>(cx+std::sin(a)*radius,cy-std::cos(a)*radius);
        };
        g.setColour(colour(0xff282e30));
        const int ticks=i==4?2:20;
        for(int t=0;t<=ticks;++t){auto q=point(static_cast<double>(t)/ticks,47);g.fillEllipse(q.x-1.2f,q.y-1.2f,2.4f,2.4f);}
        for(double value:scaleValues[static_cast<size_t>(i)]){
            const double proportion=knobs[static_cast<size_t>(i)].valueToProportionOfLength(value);
            auto q=point(proportion,47);g.fillEllipse(q.x-1.7f,q.y-1.7f,3.4f,3.4f);
            q=point(proportion,57);
            juce::String label=i==4?juce::String(value==0?"K-12":value==1?"K-14":"K-20"):juce::String(value,0);
            if(i==2&&value==100)continue; // 50 and 100 ms are too close for two printed labels.
            text(g,label,q.x-21,q.y-7,42,14,i==4?10.5f:11.0f);
        }
        text(g,titles[i],cx-68,207,136,21,12);
    }
    paintMeter(g);g.setColour(colour(0xff171c1f));
    text(g,"RIDE",1140,145,42,15,10);text(g,"OUT",1219,145,35,15,10);text(g,"METER",1158,177,82,18,11);
    text(g,"ON",1020,145,30,15,10);text(g,"OFF",1089,145,35,15,10);text(g,"SAFE",1028,177,82,18,11);
    text(g,"ON",1280,145,30,15,10);text(g,"OFF",1349,145,35,15,10);text(g,"FIX",1288,177,82,18,11);
    const float reduction=processor.safeReductionDb.load(std::memory_order_relaxed);
    const bool safeOn=processor.apvts.getRawParameterValue("safe")->load()>0.5f&&processor.apvts.getRawParameterValue("power")->load()>0.5f&&processor.apvts.getRawParameterValue("bypass")->load()<0.5f;
    const float ledAlpha=safeOn?(reduction>0.1f?0.7f:0.3f):0.0f;
    g.setGradientFill(juce::ColourGradient(colour(0xffbc65ff).withAlpha(ledAlpha),1012,153,juce::Colours::transparentBlack,1021,153,true));g.fillEllipse(1003,144,18,18);
    g.setColour(colour(0xff784a96));g.drawEllipse(1009,150,6,6,0.8f);
    g.setColour(colour(safeOn?(reduction>0.1f?0xffedd0ff:0xffc77aff):0xff665470));g.fillEllipse(1010,151,4,4);
    g.setColour(colour(0xff171c1f));
    text(g,"-1 dBTP",1024,215,90,16,10);text(g,"0 dB K AUTO",1264,215,130,16,10);
    // Hand-drawn vector script, independent of installed fonts.
    juce::Path signature;
    signature.startNewSubPath(0,22);signature.lineTo(12,2);signature.cubicTo(25,-3,25,7,13,12);signature.lineTo(7,12);
    signature.startNewSubPath(14,18);signature.cubicTo(21,6,29,9,22,18);signature.cubicTo(18,24,13,22,18,16);signature.lineTo(28,12);signature.lineTo(24,20);signature.lineTo(34,14);
    signature.startNewSubPath(29,29);signature.lineTo(39,10);signature.cubicTo(49,5,48,15,40,19);signature.lineTo(36,18);signature.lineTo(49,13);
    signature.startNewSubPath(51,10);signature.lineTo(46,20);signature.quadraticTo(49,23,57,14);
    signature.startNewSubPath(54,6);signature.lineTo(55,4);
    signature.startNewSubPath(54,17);signature.cubicTo(66,16,67,9,61,11);signature.cubicTo(50,17,56,26,68,17);
    signature.cubicTo(82,1,80,-3,73,8);signature.lineTo(68,19);signature.quadraticTo(68,23,78,16);
    signature.cubicTo(86,6,93,10,86,18);signature.cubicTo(79,26,77,16,84,13);signature.lineTo(91,11);signature.lineTo(81,29);signature.cubicTo(75,37,70,29,88,22);
    signature.lineTo(95,13);signature.lineTo(91,20);signature.quadraticTo(92,24,101,14);signature.lineTo(98,20);signature.quadraticTo(101,22,108,14);
    signature.cubicTo(116,7,122,10,115,18);signature.cubicTo(108,26,106,17,112,13);signature.lineTo(120,11);signature.lineTo(116,20);signature.lineTo(127,13);
    signature.startNewSubPath(121,29);signature.lineTo(132,10);signature.cubicTo(142,5,142,15,134,19);signature.lineTo(129,18);signature.lineTo(143,13);
    signature.cubicTo(153,6,156,12,148,19);signature.cubicTo(140,25,137,17,146,12);signature.quadraticTo(149,17,155,13);
    const auto transform=juce::AffineTransform::scale(0.80f,0.80f).translated(1436,107);
    g.setColour(juce::Colours::white.withAlpha(0.28f));g.strokePath(signature,juce::PathStrokeType(3.0f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded),transform.followedBy(juce::AffineTransform::translation(1,1)));
    g.setColour(colour(0xff111719));g.strokePath(signature,juce::PathStrokeType(3.0f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded),transform);
    juce::Path flourish;flourish.startNewSubPath(1447,138);flourish.lineTo(1572,130);flourish.lineTo(1522,141);flourish.lineTo(1536,133);
    g.strokePath(flourish,juce::PathStrokeType(1.6f));
}
RiderEditor::RiderEditor(RiderProcessor& p):AudioProcessorEditor(p),processor(p),faceplate(p)
{
    constexpr int initialWidth=817; // Always open at the FET 67 minimum, including restored sessions.
    addAndMakeVisible(faceplate);setResizable(true,true);setResizeLimits(817,126,4085,630);
    getConstrainer()->setFixedAspectRatio(static_cast<double>(kBaseWidth)/kBaseHeight);
    setSize(initialWidth,juce::roundToInt(static_cast<double>(initialWidth)*kBaseHeight/kBaseWidth));resized();
}
void RiderEditor::resized()
{
    const float sx=static_cast<float>(getWidth())/kBaseWidth,sy=static_cast<float>(getHeight())/kBaseHeight;
    faceplate.setBounds(0,0,kBaseWidth,kBaseHeight);faceplate.setTransform(juce::AffineTransform::scale(juce::jmin(sx,sy)));
    processor.editorWidth.store(getWidth());processor.editorHeight.store(getHeight());
}
