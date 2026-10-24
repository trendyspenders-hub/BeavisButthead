#include "../BBIREvan/IREvanPedalDSP.h"
#include <cassert>
#include <cstdlib>
#include <new>
#include <iostream>
#include <vector>
static bool forbidAllocation = false;
void* operator new(std::size_t n) { if(forbidAllocation) std::abort(); if(auto* p=std::malloc(n)) return p; throw std::bad_alloc(); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
int main()
{
  for(double sr : {44100.0,48000.0,96000.0,192000.0})
  {
    IREvanPedalDSP dsp; dsp.Prepare(sr);
    std::vector<double> audio(static_cast<size_t>(sr*2.5),0.0);
    for(size_t i=0;i<audio.size();++i) audio[i]=0.25*std::sin(i*6.28318530718*440/sr);
    const auto dry=audio;
    forbidAllocation=true;
    dsp.Drive(audio.data(),audio.size(),false,10);dsp.Echo(audio.data(),audio.size(),false,320,20);
    forbidAllocation=false;
    assert(audio==dry);
    forbidAllocation=true;
    dsp.Drive(audio.data(),audio.size(),true,8);
    forbidAllocation=false;
    double difference=0;
    for(size_t i=0;i<audio.size();++i) {assert(std::isfinite(audio[i])&&std::abs(audio[i])<0.66);difference+=std::abs(audio[i]-dry[i]);}
    assert(difference>100.0);
    dsp.Prepare(sr);std::fill(audio.begin(),audio.end(),0.0);audio[0]=1.0;
    forbidAllocation=true;
    dsp.Echo(audio.data(),audio.size(),true,320,50);
    forbidAllocation=false;
    const size_t delay=static_cast<size_t>(sr*0.320);
    assert(audio[0]==1.0);
    double first=0,second=0;
    for(size_t i=1;i<delay-2;++i) assert(std::abs(audio[i])<1e-10);
    for(size_t i=delay-2;i<delay+30;++i) first+=std::abs(audio[i]);
    for(size_t i=delay*2-2;i<delay*2+60;++i) second+=std::abs(audio[i]);
    assert(first>0.4&&first<0.51&&second>0.10&&second<0.20);
    for(auto v:audio)assert(std::isfinite(v));
    // Sample-rate reset clears both the delay tail and its write position.
    dsp.Prepare(sr);std::fill(audio.begin(),audio.end(),0.0);
    dsp.Echo(audio.data(),audio.size(),true,900,50);
    for(auto v:audio)assert(v==0.0);
  }
  // Processing has identical results regardless of host block boundaries.
  IREvanPedalDSP a,b;a.Prepare(48000);b.Prepare(48000);
  std::vector<double>x(100000,0.0),y;x[0]=0.5;y=x;
  a.Drive(x.data(),x.size(),true,4);a.Echo(x.data(),x.size(),true,180,35);
  for(size_t i=0;i<y.size();i+=64){const auto n=std::min(size_t(64),y.size()-i);b.Drive(y.data()+i,n,true,4);b.Echo(y.data()+i,n,true,180,35);}
  assert(x==y);
  std::cout<<"PASS: exact reset bypass, drive signal change, echo timing/feedback, reset silence, block independence, no Process allocations at 44.1/48/96/192 kHz.\n";
}
