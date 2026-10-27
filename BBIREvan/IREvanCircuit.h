#pragma once
// Original circuit solver. Source boundaries and approximations: docs/CIRCUIT-ENGINE.md.
#include <array>
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace irevan::circuit {
constexpr int Nodes=24, Parts=64;
using Vec=std::array<double,Nodes>;
using Matrix=std::array<Vec,Nodes>;
struct Controls { double gain=.5,bass=.5,middle=.5,treble=.5,master=.5; bool bright=true; };
enum class Model { AB763, MV2203 };
struct Part { int a,b; double value; };
struct Tube { int plate,grid,cathode; };
inline bool Solve(Matrix a,Vec b,Vec& x,int n) {
  for(int k=0;k<n;++k) {
    int pivot=k;
    for(int i=k+1;i<n;++i)if(std::abs(a[i][k])>std::abs(a[pivot][k]))pivot=i;
    if(!std::isfinite(a[pivot][k])||std::abs(a[pivot][k])<1e-18)return false;
    if(pivot!=k){std::swap(a[pivot],a[k]);std::swap(b[pivot],b[k]);}
    for(int i=k+1;i<n;++i){double q=a[i][k]/a[k][k];for(int j=k+1;j<n;++j)a[i][j]-=q*a[k][j];b[i]-=q*b[k];}
  }
  for(int i=n-1;i>=0;--i){double v=b[i];for(int j=i+1;j<n;++j)v-=a[i][j]*x[j];x[i]=v/a[i][i];if(!std::isfinite(x[i]))return false;}
  return true;
}
inline double Softplus(double x){return x>35?x:(x< -35?std::exp(x):std::log1p(std::exp(x)));}
inline double Logistic(double x){return x>=0?1/(1+std::exp(-x)):std::exp(x)/(1+std::exp(x));}
struct Current{double plate,grid,dp,dg,gg;};
inline Current Triode(double vp,double vg){
  // Koren's 12AX7 plate-current equation; grid conduction is a smooth resistive approximation.
  double p=std::max(0.,vp),d=std::sqrt(300+p*p),q=600*(.01+vg/d),l=Softplus(q),s=Logistic(q),e=p*l/600;
  double scale=e>0?2*1.4/1060*std::pow(e,.4):0;
  return {2/1060.0*std::pow(e,1.4),.1/2000*Softplus(vg/.1),vp>0?scale*(l/600-p*p*vg*s/(d*d*d)):0,scale*p*s/d,Logistic(vg/.1)/2000};
}
class Network {
public:
  Model model=Model::AB763; Controls controls{};
  int n=0,nr=0,nc=0,nt=0,output=0;
  std::array<Part,Parts> resistors{},capacitors{};
  std::array<Tube,4> tubes{};
  Vec volts{};uint64_t failures=0;double sampleRate=192000;
  void Configure(Model m,double fs,Controls c={}) {
    model=m;sampleRate=fs;controls=c;volts={};y={};history={};failures=0;
    Build();Compile(false);Step(0,false,240);
    for(int i=0;i<nc;++i)history[i]=-sampleRate*capacitors[i].value*Voltage(capacitors[i],0);
    Compile(true);
  }
  void SetControls(Controls c){
    const auto old=capacitors;controls=c;Build();
    for(int i=0;i<nc;++i)if(old[i].value!=capacitors[i].value)
      history[i]=-sampleRate*capacitors[i].value*Voltage(capacitors[i],0);
    Compile(true);
  }
  double Tick(double input){Step(input,true,64);return volts[output];}
  double Rail()const{return model==Model::AB763?410.:320.;}
private:
  Matrix inverse{},impedance{},projection{};Vec base{},inputVector{},y{};
  std::array<double,Parts> history{};
  static constexpr int Gnd=-1,Bplus=-2,Input=-3;
  void R(int a,int b,double v){resistors[nr++]={a,b,std::max(1.,v)};}
  void C(int a,int b,double v){capacitors[nc++]={a,b,v};}
  void T(int p,int g,int k){tubes[nt++]={p,g,k};}
  static double Audio(double v){return (std::pow(10.,2*std::clamp(v,0.,1.))-1)/99.;}
  void Build(){
    nr=nc=nt=0;
    double g=Audio(controls.gain),b=Audio(controls.bass),t=std::clamp(controls.treble,0.,1.),mid=std::clamp(controls.middle,0.,1.),master=Audio(controls.master);
    if(model==Model::AB763){
      n=13;output=11;
      R(Input,Gnd,1e6);R(Input,0,34e3);R(1,Bplus,100e3);R(2,Gnd,1500);C(2,Gnd,25e-6);T(1,0,2);
      R(1,3,100e3);C(1,4,250e-12);C(3,5,.1e-6);C(3,6,.047e-6);
      R(4,7,250e3*(1-t));R(7,5,250e3*t);R(5,6,250e3*b);R(6,Gnd,10e3*mid);
      R(7,8,1e6*(1-g));R(8,Gnd,1e6*g);C(7,8,controls.bright?120e-12:0);T(9,8,10);
      R(9,Bplus,100e3);R(10,Gnd,820);C(10,Gnd,25e-6);
      // Idle Vibrato recovery shares the Normal recovery's cathode bias network.
      R(12,Bplus,100e3);T(12,Gnd,10);
      // Preamp output boundary: coupling capacitor loaded by approximated mixer/PI input.
      C(9,11,.047e-6);R(11,Gnd,400e3);
    }else{
      n=20;output=18;
      R(Input,Gnd,1e6);R(Input,0,68e3);R(1,Bplus,100e3);R(2,Gnd,2700);C(2,Gnd,.68e-6);T(1,0,2);
      C(1,3,22e-9);R(3,4,470e3);C(3,4,470e-12);
      R(4,5,1e6*(1-g));R(5,Gnd,1e6*g);C(4,5,1e-9);
      R(6,Bplus,100e3);R(7,Gnd,10e3);T(6,5,7);
      C(6,8,22e-9);R(8,9,470e3);C(8,9,470e-12);R(9,Gnd,470e3);
      R(10,Bplus,100e3);R(11,Gnd,820);T(10,9,11);T(Bplus,10,12);R(12,Gnd,100e3);
      R(12,13,33e3);C(12,14,500e-12);C(13,15,22e-9);C(13,16,22e-9);
      R(14,17,250e3*(1-t));R(17,15,250e3*t);R(15,19,1e6*b);
      // Marshall Mid is a potentiometer with capacitor at wiper, not a rheostat.
      R(19,16,25e3*(1-mid));R(16,Gnd,25e3*mid);
      R(17,18,1e6*(1-master));R(18,Gnd,1e6*master);R(18,Gnd,1e6);
    }
    for(int i=0;i<nt;++i){auto t=tubes[i];C(t.grid,t.cathode,2.3e-12);C(t.plate,t.grid,2.4e-12);C(t.plate,t.cathode,.9e-12);}
  }
  int A(int p)const{return p%2?tubes[p/2].grid:tubes[p/2].plate;}
  int B(int p)const{return tubes[p/2].cathode;}
  double Known(int node,double in)const{return node==Bplus?Rail():(node==Input?in:0);}
  double V(int node,double in)const{return node>=0?volts[node]:Known(node,in);}
  double Voltage(const Part& p,double in)const{return V(p.a,in)-V(p.b,in);}
  void Compile(bool transient){
    Matrix a{};base={};inputVector={};for(int i=0;i<n;++i)a[i][i]=1e-12;
    auto stamp=[&](int x,int z,double g){
      if(x>=0){a[x][x]+=g;if(z>=0)a[x][z]-=g;else if(z==Bplus)base[x]+=g*Rail();else if(z==Input)inputVector[x]+=g;}
      if(z>=0){a[z][z]+=g;if(x>=0)a[z][x]-=g;else if(x==Bplus)base[z]+=g*Rail();else if(x==Input)inputVector[z]+=g;}
    };
    for(int i=0;i<nr;++i)stamp(resistors[i].a,resistors[i].b,1/resistors[i].value);
    if(transient)for(int i=0;i<nc;++i)stamp(capacitors[i].a,capacitors[i].b,sampleRate*capacitors[i].value);
    for(int j=0;j<n;++j){Vec rhs{},col{};rhs[j]=1;if(!Solve(a,rhs,col,n)){++failures;return;}for(int i=0;i<n;++i)inverse[i][j]=col[i];}
    for(int p=0;p<2*nt;++p)for(int i=0;i<n;++i)projection[i][p]=(A(p)>=0?inverse[i][A(p)]:0)-(B(p)>=0?inverse[i][B(p)]:0);
    for(int p=0;p<2*nt;++p)for(int q=0;q<2*nt;++q)impedance[p][q]=(A(p)>=0?projection[A(p)][q]:0)-(B(p)>=0?projection[B(p)][q]:0);
  }
  void Step(double input,bool transient,int limit){
    Vec rhs=base,linear{},open{},curr{};
    for(int i=0;i<n;++i)rhs[i]+=input*inputVector[i];
    if(transient)for(int i=0;i<nc;++i){auto c=capacitors[i];if(c.a>=0)rhs[c.a]-=history[i];if(c.b>=0)rhs[c.b]+=history[i];}
    for(int i=0;i<n;++i)for(int j=0;j<n;++j)linear[i]+=inverse[i][j]*rhs[j];
    for(int p=0;p<2*nt;++p)open[p]=(A(p)>=0?linear[A(p)]:Known(A(p),input))-(B(p)>=0?linear[B(p)]:Known(B(p),input));
    Vec candidate=y;bool converged=false;
    for(int it=0;it<limit;++it){
      Matrix jac{};Vec residual{};
      for(int k=0;k<nt;++k){auto c=Triode(candidate[2*k],candidate[2*k+1]);curr[2*k]=c.plate;curr[2*k+1]=c.grid;
        for(int p=0;p<2*nt;++p){jac[p][2*k]=impedance[p][2*k]*c.dp;jac[p][2*k+1]=impedance[p][2*k]*c.dg+impedance[p][2*k+1]*c.gg;}}
      double error=0;
      for(int p=0;p<2*nt;++p){jac[p][p]+=1;residual[p]=candidate[p]-open[p];for(int q=0;q<2*nt;++q)residual[p]+=impedance[p][q]*curr[q];error=std::max(error,std::abs(residual[p]));}
      if(error<1e-5){converged=true;break;}
      Vec step{};if(!Solve(jac,residual,step,2*nt))break;
      double damping=1;for(int p=0;p<2*nt;++p)damping=std::min(damping,(p%2?10.:60.)/(std::abs(step[p])+1e-30));
      for(int p=0;p<2*nt;++p)candidate[p]-=damping*step[p];
    }
    if(!converged){++failures;return;}
    y=candidate;for(int i=0;i<n;++i){volts[i]=linear[i];for(int p=0;p<2*nt;++p)volts[i]-=projection[i][p]*curr[p];}
    if(transient)for(int i=0;i<nc;++i)history[i]=-sampleRate*capacitors[i].value*Voltage(capacitors[i],input);
  }
};
class FIR {
  std::array<double,65> taps{},delay{};int pos=0;
public:
  void Reset(){delay={};pos=0;double sum=0;constexpr double pi=3.14159265358979323846;
    for(int i=0;i<65;++i){double x=i-32.;taps[i]=(x==0?.225:std::sin(pi*.225*x)/(pi*x))*(.42-.5*std::cos(2*pi*i/64)+.08*std::cos(4*pi*i/64));sum+=taps[i];}for(auto& x:taps)x/=sum;}
  double Tick(double x){delay[pos]=x;double out=0;int j=pos;for(int i=0;i<65;++i){out+=taps[i]*delay[j];if(--j<0)j=64;}if(++pos==65)pos=0;return out;}
};
class Engine {
  Network network;FIR up,down;Controls current{},target{};double fs=48000,fade=0;int count=0;
public:
  // Two 65-tap FIRs at 4x: 64 high-rate samples = 16 host samples.
  static constexpr int Latency=16;
  void Prepare(Model model,double rate,Controls c={}){fs=std::isfinite(rate)?std::max(8000.,rate):48000;current=target=c;network.Configure(model,fs*4,c);up.Reset();down.Reset();fade=0;count=0;}
  void SetControls(Controls c){target=c;}
  uint64_t Failures()const{return network.failures;}
  double Tick(double input){
    if(++count==32){count=0;const double a=1-std::exp(-32/(fs*.02));auto smooth=[a](double& x,double y){x+=a*(y-x);};
      smooth(current.gain,target.gain);smooth(current.bass,target.bass);smooth(current.middle,target.middle);smooth(current.treble,target.treble);smooth(current.master,target.master);current.bright=target.bright;
      auto diff=[](double a,double b){return std::abs(a-b);};auto old=network.controls;
      if(diff(current.gain,old.gain)+diff(current.bass,old.bass)+diff(current.middle,old.middle)+diff(current.treble,old.treble)+diff(current.master,old.master)>1e-5||current.bright!=old.bright)network.SetControls(current);
    }
    input=std::isfinite(input)?std::clamp(input,-20.,20.):0;double out=0;
    for(int i=0;i<4;++i){double filtered=down.Tick(network.Tick(up.Tick(i==0?input*4:0)));if(i==0)out=filtered;}
    fade=std::min(1.,fade+1/(fs*.01));return out*(network.model==Model::AB763?.02:.03)*fade;
  }
};
}
