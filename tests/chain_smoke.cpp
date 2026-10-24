#include "../BBIREvan/IREvanModelWrapper.h"
#include "../BBIREvan/IREvanPedalDSP.h"
#include "../BBIREvan/ToneStack.h"
#include "../NeuralAmpModelerCore/NAM/get_dsp.h"
#include "../AudioDSPTools/dsp/ImpulseResponse.h"
#include "../AudioDSPTools/dsp/NoiseGate.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <chrono>
int main(int argc, char** argv)
{
  if(argc<3 || argc>4) return 2;
  const std::filesystem::path dir(argv[1]);
  size_t cases=0;
  // Exact regression: encapsulated model reset must use its 48 kHz rate.
  class RateProbe final : public nam::DSP {
  public:
    double lastRate=0;
    RateProbe():nam::DSP(1,1,48000){}
    void Reset(double rate,int maxFrames) override {lastRate=rate;nam::DSP::Reset(rate,maxFrames);}
    void prewarm() override {}
    void process(NAM_SAMPLE** in,NAM_SAMPLE** out,int n) override {std::copy(in[0],in[0]+n,out[0]);}
  };
  auto probe=std::make_unique<RateProbe>();auto* observer=probe.get();
  ResamplingNAM checked(std::move(probe),96000);checked.Reset(44100,1024);
  assert(observer->lastRate==48000);

  for(const auto& file:std::filesystem::recursive_directory_iterator(dir))
  {
    if(file.path().extension()!=".nam")continue;
    if(argc==4 && file.path().filename()!=argv[3])continue;
    for(const auto& cabinet:std::filesystem::directory_iterator(argv[2]))
    {
    if(cabinet.path().extension()!=".wav")continue;
    for(double rate:{44100.0,48000.0,96000.0,192000.0})for(int block:{32,64,256,1024})
    {
      auto original=nam::get_dsp(file.path());
      ResamplingNAM model(std::move(original),rate);model.Reset(rate,block);
      IREvanPedalDSP pedals;pedals.Prepare(rate);
      dsp::tone_stack::BasicNamToneStack eq;eq.Reset(rate,block);
      eq.SetParam("bass",4);eq.SetParam("middle",6);eq.SetParam("treble",4);
      dsp::noise_gate::Gain gain;dsp::noise_gate::Trigger gate;gate.AddListener(&gain);gate.SetSampleRate(rate);
      gate.SetParams(dsp::noise_gate::TriggerParams(0.01,-80,0.1,0.005,0.01,0.05));
      dsp::ImpulseResponse ir(cabinet.path().string().c_str(),rate);
      assert(ir.GetWavState()==dsp::wav::LoadReturnCode::SUCCESS);
      recursive_linear_filter::HighPass hpf;hpf.SetParams(recursive_linear_filter::HighPassParams(rate,5));
      std::vector<double> in(block),out(block);double* ip[]={in.data()};double* op[]={out.data()};
      double peak=0,energy=0;size_t count=0;
      const size_t total=static_cast<size_t>(rate*0.60);
      const auto start=std::chrono::steady_clock::now();
      for(size_t offset=0;offset<total;offset+=block)
      {
        const auto n=std::min(static_cast<size_t>(block),total-offset);
        const double inputGain=offset<total/2?1.0:10.0; // 0 then +20 dB stress
        for(size_t i=0;i<n;++i)in[i]=inputGain*0.1*(std::sin((offset+i)*6.28318530718*110/rate)+0.3*std::sin((offset+i)*6.28318530718*880/rate));
        auto** triggered=gate.Process(ip,1,n);
        pedals.Drive(triggered[0],n,true,6);
        model.process(triggered,op,static_cast<int>(n));
        auto** g=gain.Process(op,1,n);auto** toned=eq.Process(g,1,static_cast<int>(n));
        auto** cab=ir.Process(toned,1,n);pedals.Echo(cab[0],n,true,320,25);
        auto** final=hpf.Process(cab,1,n);
        for(size_t i=0;i<n;++i){assert(std::isfinite(final[0][i]));peak=std::max(peak,std::abs(final[0][i]));energy+=final[0][i]*final[0][i];++count;}
      }
      assert(peak>1e-7&&peak<100&&energy>0&&count==total);
      const auto sec=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
      std::cout<<file.path().filename().string()<<" | "<<cabinet.path().filename().string()<<" | "<<rate<<" Hz | "<<block<<" | peak="<<peak<<" | cpu/offline="<<sec/0.60<<"\n";
      ++cases;
    }
  }
    }
  assert(cases==(argc==4?48:288));std::cout<<"PASS: "<<cases<<" integrated chain cases (gate, drive, model/resampler, EQ, real cabinet IR, echo, DC blocker).\n";
}
