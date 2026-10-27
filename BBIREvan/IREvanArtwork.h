#pragma once
#include <array>
#include <algorithm>
#include "IControls.h"

// UI-only observer: DAW automation and knob edits reach this control through
// the existing input parameter. No image loading or graphics on the audio thread.
class IREvanArtworkControl final : public iplug::igraphics::IControl
{
public:
  IREvanArtworkControl(const iplug::igraphics::IRECT& bounds, int driveParam,
                      const std::array<iplug::igraphics::IBitmap, 4>& frames)
  : IControl(bounds, driveParam), mFrames(frames)
  {
    SetIgnoreMouse(true);
  }

  static int ReactionFrame(double normalizedDrive)
  {
    return std::clamp(static_cast<int>(std::clamp(normalizedDrive, 0.0, 1.0) * 4.0), 0, 3);
  }

  void Draw(iplug::igraphics::IGraphics& g) override
  {
    using namespace iplug::igraphics;
    const IColor ink(255, 16, 20, 19), teal(255, 63, 112, 107);
    const IColor cream(255, 255, 236, 188), yellow(255, 255, 211, 64);
    const IColor red(255, 133, 48, 49);
    auto label = [&](const char* s, float size, IColor colour, IRECT r) {
      g.DrawText(IText(size, colour, "Roboto-Regular"), s, r);
    };
    g.FillRect(ink, mRECT);
    // Recessed chassis edge and corner fasteners.
    g.DrawRect(IColor(255,75,81,78), mRECT.GetPadded(-4), nullptr, 2.f);
    g.FillRect(yellow, IRECT(12, 12, 988, 64));
    label("BEAVIS AND BUTTHEAD", 32, ink, IRECT(20, 16, 690, 60));
    label("by I.R.Evan", 20, ink, IRECT(706, 20, 922, 56));
    g.FillRect(teal, IRECT(18, 78, 502, 552));
    const int frame = ReactionFrame(GetValue());
    g.DrawFittedBitmap(mFrames[frame], IRECT(25, 85, 495, 545));
    g.DrawRect(cream, IRECT(23, 83, 497, 547), nullptr, 3.f);
    g.FillRect(red, IRECT(520, 78, 982, 552));
    g.DrawRect(COLOR_BLACK, IRECT(520, 78, 982, 552), nullptr, 4.f);
    label("FACTORY AMP CAPTURES", 12, yellow, IRECT(530, 81, 970, 102));
    label("CABINETS", 11, cream, IRECT(530, 159, 970, 176));

    label("CIRCUIT MODES: EXPERIMENTAL PREAMPS", 11, cream, IRECT(532, 529, 970, 548));
    g.FillRect(teal, IRECT(18, 570, 982, 756));
    g.DrawRect(cream, IRECT(18, 570, 982, 756), nullptr, 2.f);
    label("AMP .NAM", 12, cream, IRECT(120, 756, 400, 776));
    label("CABINET .WAV IR", 12, cream, IRECT(610, 756, 890, 776));
    label("NAM CAPTURES + CIRCUIT PREAMPS  /  POST EQ IS SEPARATE", 11, cream, IRECT(24, 815, 976, 836));
  }

private:
  std::array<iplug::igraphics::IBitmap, 4> mFrames;
};

class IREvanPedalBackdrop final : public iplug::igraphics::IControl
{
public:
  IREvanPedalBackdrop(const iplug::igraphics::IRECT& bounds) : IControl(bounds) {}
  void Draw(iplug::igraphics::IGraphics& g) override
  {
    using namespace iplug::igraphics;
    const IColor ink(255,16,20,19), cream(255,255,236,188), yellow(255,255,211,64);
    g.FillRect(ink, mRECT);
    g.DrawRect(yellow, mRECT, nullptr, 3.f);
    g.DrawText(IText(16,cream,"Roboto-Regular"), "PEDAL DRAWER   /   GATE > DRIVE > NAM > EQ > CAB > ECHO", IRECT(25,572,825,603));
    const std::array<IColor,4> colours = {IColor(255,69,99,81),IColor(255,138,65,44),IColor(255,48,84,121),IColor(255,94,72,105)};
    const char* titles[] = {"SHUT UP", "RIFF JUICE", "HEH HEH", "THE BOX"};
    const char* subtitles[] = {"NOISE GATE", "SOFT-CLIP DRIVE", "ECHO DELAY", "CABINET IR"};
    for (int i=0; i<4; ++i)
    {
      const float x=35.f+240.f*i;
      g.FillRect(IColor(255,5,8,7), IRECT(x+5,615,x+215,827));
      g.FillRect(colours[i], IRECT(x,611,x+210,823));
      g.DrawRect(IColor(255,180,169,142), IRECT(x,611,x+210,823), nullptr,2.f);
      g.DrawLine(cream,x+6,614,x+204,614,nullptr,1.f);
      g.FillRect(ink, IRECT(x+10,779,x+200,817));
      for (int rib=0; rib<6; ++rib)
        g.DrawLine(IColor(255,48,52,48),x+14,782+rib*6,x+196,782+rib*6,nullptr,1.f);
      for (float sx : {x+10,x+200})
        for (float sy : {622.f,812.f}) {
          g.FillCircle(IColor(255,186,186,170),sx,sy,3.f);
          g.DrawLine(ink,sx-2,sy,sx+2,sy,nullptr,1.f);
        }
      g.DrawText(IText(19,yellow,"Roboto-Regular"),titles[i],IRECT(x,618,x+210,644));
      g.DrawText(IText(11,cream,"Roboto-Regular"),subtitles[i],IRECT(x,643,x+210,663));
    }
    g.DrawText(IText(12,cream,"Roboto-Regular"), "Load the WAV IR", IRECT(760,688,950,710));
    g.DrawText(IText(12,cream,"Roboto-Regular"), "on the main panel.", IRECT(760,712,950,734));
    g.DrawText(IText(10,cream,"Roboto-Regular"), "Uses the same CAB bypass.", IRECT(755,744,955,763));
  }
};

// Keep iPlug's parameter/automation handling; replace only the toggle drawing.
class IREvanStompControl final : public iplug::igraphics::IVToggleControl
{
public:
  IREvanStompControl(const iplug::igraphics::IRECT& bounds, int param,
                    const iplug::igraphics::IVStyle& style)
    : IVToggleControl(bounds, param, "", style) {}
  void Draw(iplug::igraphics::IGraphics& g) override
  {
    using namespace iplug::igraphics;
    const bool enabled = GetValue() > 0.5;
    const float x=mRECT.L+24, y=mRECT.MH();
    g.FillCircle(IColor(255,5,8,7),x+1,y+2,14);
    g.FillCircle(IColor(255,111,119,119),x,y,13);
    g.FillCircle(IColor(255,222,225,213),x,y-1,10);
    g.DrawCircle(IColor(255,248,247,227),x,y-1,8,nullptr,1.f);
    g.FillCircle(IColor(255,7,12,9),mRECT.R-17,y,6);
    g.FillCircle(enabled ? IColor(255,174,241,83) : IColor(255,58,65,53),mRECT.R-17,y,4);
    g.DrawText(IText(11,IColor(255,255,236,188),"Roboto-Regular"),
      enabled ? "ON" : "BYPASS", IRECT(mRECT.L+44,mRECT.T,mRECT.R-28,mRECT.B));
  }
};
