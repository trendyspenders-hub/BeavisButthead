#include "../BBIREvan/IREvanCircuit.h"
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <new>
#include <vector>
static bool track=false;static unsigned allocations=0;
void* operator new(std::size_t s){if(track)++allocations;if(auto p=std::malloc(s))return p;throw std::bad_alloc();}
void operator delete(void* p) noexcept {std::free(p);}
void operator delete(void* p,std::size_t) noexcept {std::free(p);}
int main(){using namespace irevan::circuit;constexpr double pi=3.14159265358979323846;
  auto started=std::chrono::steady_clock::now();int cases=0;
  for(auto m:{Model::AB763,Model::MV2203})for(double fs:{44100.,48000.,96000.,192000.})for(double knob:{0.,.5,1.}){
    Controls c{knob,knob,knob,knob,knob,true};Engine e;e.Prepare(m,fs,c);assert(e.Failures()==0);
    double peak=0;track=true;
    for(int i=0;i<int(fs*.07);++i){double input=(i<int(fs*.03)?2.:.1)*std::sin(2*pi*440*i/fs);double x=e.Tick(input);assert(std::isfinite(x));peak=std::max(peak,std::abs(x));}
    track=false;if(e.Failures()){std::cerr<<"convergence failure "<<int(m)<<" "<<fs<<" "<<knob<<" count "<<e.Failures()<<"\n";return 1;}
    assert(peak<20);if(knob>0)assert(peak>1e-5);++cases;
  }
  for(auto m:{Model::AB763,Model::MV2203}){
    Engine a,b;a.Prepare(m,48000);b.Prepare(m,48000);std::vector<double> ref(6000),output(6000);
    for(int i=0;i<6000;++i)ref[i]=a.Tick(.1*std::sin(i*.08));
    for(int start=0;start<6000;start+=37)for(int i=start;i<std::min(start+37,6000);++i)output[i]=b.Tick(.1*std::sin(i*.08));
    assert(ref==output);b.Prepare(m,48000);for(int i=0;i<6000;++i)assert(std::abs(b.Tick(.1*std::sin(i*.08))-ref[i])<1e-12);
    a.SetControls({1,0,1,0,1,false});track=true;
    for(int i=0;i<10000;++i){assert(std::isfinite(a.Tick(.1*std::sin(i*.08))));}
    track=false;assert(a.Failures()==0);
    assert(std::isfinite(a.Tick(std::numeric_limits<double>::quiet_NaN())));assert(std::isfinite(a.Tick(std::numeric_limits<double>::infinity())));
    b.Prepare(m,48000);double silence=0;for(int i=0;i<4800;++i)silence=std::max(silence,std::abs(b.Tick(0)));assert(silence<1e-7);
  }
  assert(allocations==0);
  double seconds=std::chrono::duration<double>(std::chrono::steady_clock::now()-started).count();
  std::cout<<"PASS "<<cases<<" circuit rate/control stress cases; convergence, silence, reset, block independence, moving controls and no audio allocations. Seconds="<<seconds<<"\n";
}
