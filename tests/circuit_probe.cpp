#include "../BBIREvan/IREvanCircuit.h"
#include <iostream>
#include <iomanip>
#include <string>
int main(int argc,char** argv){
  if(argc<2)return 2;
  using namespace irevan::circuit;
  Model m=std::stoi(argv[1])==0?Model::AB763:Model::MV2203;
  double rate=argc>2?std::stod(argv[2]):192000,level=argc>3?std::stod(argv[3]):.1;
  Network n;n.Configure(m,rate);std::cout<<std::setprecision(16);
  if(argc>4&&std::string(argv[4])=="netlist"){
    auto node=[](int v){return v==-1?std::string("0"):(v==-2?std::string("supply"):(v==-3?std::string("input"):"n"+std::to_string(v)));};
    std::cout<<"* Circuit preamp test export; same device law, independent SPICE solver\nVs supply 0 "<<n.Rail()<<"\nVi input 0 SIN(0 "<<level<<" 440)\n";
    for(int i=0;i<n.nr;++i){auto p=n.resistors[i];std::cout<<"R"<<i<<" "<<node(p.a)<<" "<<node(p.b)<<" "<<p.value<<"\n";}
    for(int i=0;i<n.nc;++i){auto p=n.capacitors[i];if(p.value>0)std::cout<<"C"<<i<<" "<<node(p.a)<<" "<<node(p.b)<<" "<<p.value<<"\n";}
    for(int i=0;i<n.n;++i)std::cout<<"Rgmin"<<i<<" n"<<i<<" 0 1e12\n";
    for(int i=0;i<n.nt;++i){auto t=n.tubes[i];std::cout<<"X"<<i<<" "<<node(t.plate)<<" "<<node(t.grid)<<" "<<node(t.cathode)<<" triode\n";}
    std::cout<<".subckt triode p g k\n.func soft(x) {max(x,0)+ln(1+exp(-abs(x)))}\nBplate p k I={2/1060*pow(max(v(p,k),0)/600*soft(600*(.01+v(g,k)/sqrt(300+pow(max(v(p,k),0),2)))),1.4)}\nBgrid g k I={.1/2000*soft(v(g,k)/.1)}\n.ends\n.control\nset wr_singlescale\nset wr_vecnames\nset numdgt=16\nset filetype=ascii\nop\nwrdata bias.txt";
    for(int i=0;i<n.n;++i)std::cout<<" v(n"<<i<<")";
    std::cout<<"\ntran "<<1/rate<<" .08 0 "<<1/rate/4<<"\nwrdata transient.txt v(n"<<n.output<<")\nquit\n.endc\n.end\n";
  }else{
    std::cerr<<"DC";for(int i=0;i<n.n;++i)std::cerr<<" "<<std::setprecision(16)<<n.volts[i];std::cerr<<"\n";
    for(int i=1;i<=int(rate*.08);++i){double time=i/rate;std::cout<<time<<" "<<n.Tick(level*std::sin(time*6.283185307179586*440))<<"\n";}
    std::cerr<<"failures="<<n.failures<<"\n";if(n.failures)return 1;
  }
}
