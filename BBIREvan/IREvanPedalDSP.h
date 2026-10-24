#pragma once
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

// Original lightweight prototype effects; no third-party pedal code copied.
// Buffers are allocated only by Prepare() during the host reset, never Process().
class IREvanPedalDSP
{
public:
  void Prepare(double sampleRate)
  {
    mRate = std::isfinite(sampleRate) && sampleRate >= 8000.0 ? sampleRate : 48000.0;
    mSlew = 1.0 - std::exp(-1.0 / (0.010 * mRate));
    mDelay.assign(static_cast<size_t>(std::ceil(mRate * 1.1)) + 2, 0.0);
    mWrite = 0;
    mDriveMix = mEchoMix = 0.0;
    mDriveGain = 4.8;
    mDelaySamples = 0.320 * mRate;
    mDamp = 0.0;
    mDampCoeff = 1.0 - std::exp(-6.283185307179586 * 4500.0 / mRate);
  }

  template <class Sample> void Drive(Sample* buffer, size_t count, bool enabled, double amount)
  {
    const double target = enabled ? 1.0 : 0.0;
    const double gain = 1.0 + 1.9 * std::clamp(amount, 0.0, 10.0);
    for (size_t i = 0; i < count; ++i)
    {
      mDriveMix += mSlew * (target - mDriveMix);
      mDriveGain += mSlew * (gain - mDriveGain);
      if (!std::isfinite(static_cast<double>(buffer[i]))) { buffer[i] = 0; continue; }
      if (mDriveMix == 0.0) continue; // exact dry bypass on a fresh/reset instance
      const double dry = static_cast<double>(buffer[i]);
      const double clipped = 0.65 * std::tanh(dry * mDriveGain) / std::tanh(mDriveGain);
      buffer[i] = static_cast<Sample>(dry + mDriveMix * (clipped - dry));
    }
  }

  template <class Sample> void Echo(Sample* buffer, size_t count, bool enabled, double milliseconds, double mixPercent)
  {
    if (mDelay.empty()) return;
    const double targetMix = enabled ? std::clamp(mixPercent, 0.0, 50.0) * 0.01 : 0.0;
    const double delayTarget = std::clamp(milliseconds, 80.0, 900.0) * 0.001 * mRate;
    const size_t length = mDelay.size();
    for (size_t i = 0; i < count; ++i)
    {
      mEchoMix += mSlew * (targetMix - mEchoMix);
      mDelaySamples += mSlew * (delayTarget - mDelaySamples);
      double pos = static_cast<double>(mWrite) - mDelaySamples;
      if (pos < 0.0) pos += static_cast<double>(length);
      const size_t a = static_cast<size_t>(pos);
      const size_t b = (a + 1) % length;
      const double frac = pos - static_cast<double>(a);
      const double delayed = mDelay[a] + frac * (mDelay[b] - mDelay[a]);
      mDamp += mDampCoeff * (delayed - mDamp);
      if (std::abs(mDamp) < 1.0e-20) mDamp = 0.0;
      const double dry = std::isfinite(static_cast<double>(buffer[i])) ? static_cast<double>(buffer[i]) : 0.0;
      // Fixed feedback is deliberately below unity; bypass ramps wet mix down.
      mDelay[mWrite] = (enabled ? dry : 0.0) + 0.35 * mDamp;
      mWrite = (mWrite + 1) % length;
      buffer[i] = static_cast<Sample>(dry + mEchoMix * mDamp);
    }
  }

private:
  std::vector<double> mDelay;
  size_t mWrite = 0;
  double mRate = 48000.0, mSlew = 0.0020811647;
  double mDriveMix = 0.0, mDriveGain = 4.8;
  double mEchoMix = 0.0, mDelaySamples = 15360.0;
  double mDamp = 0.0, mDampCoeff = 0.4;
};
