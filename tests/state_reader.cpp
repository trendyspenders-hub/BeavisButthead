#include "../BBIREvan/IREvanStateReader.h"
#include <cassert>
#include <iostream>
#include <limits>
int main()
{
  const std::vector<std::string> all={"Drive","Threshold","Bass","Middle","Treble","Output","NoiseGateActive","ToneStack","IRToggle","CalibrateInput","InputCalibrationLevel","OutputMode","Slim","PedalDriveActive","PedalDriveAmount","EchoActive","EchoTime","EchoMix"};
  for(int count:{13,18})
  {
    std::vector<std::string> names(all.begin(),all.begin()+count);
    iplug::IByteChunk full;full.PutStr("amp.nam");full.PutStr("cab.wav");
    for(int i=0;i<count;++i){double value=i+0.25;full.Put(&value);}
    nlohmann::json read;
    assert(IREvanReadPathsAndParams(full,0,read,names)==full.Size());
    assert(read["Drive"]==0.25&&read["NAMPath"]=="amp.nam"&&read["IRPath"]=="cab.wav");
    for(int cut=0;cut<full.Size();++cut)
    {
      iplug::IByteChunk truncated;if(cut)truncated.PutBytes(full.GetData(),cut);
      nlohmann::json sentinel={{"untouched",true}};
      assert(IREvanReadPathsAndParams(truncated,0,sentinel,names)==-1);
      assert(sentinel==nlohmann::json({{"untouched",true}}));
    }
    assert(IREvanReadPathsAndParams(full,-1,read,names)==-1);
    assert(IREvanReadPathsAndParams(full,full.Size()+1,read,names)==-1);
  }
  for(double invalid:{std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN()})
  {
    iplug::IByteChunk chunk;chunk.PutStr("amp");chunk.PutStr("cab");chunk.Put(&invalid);
    nlohmann::json out;assert(IREvanReadPathsAndParams(chunk,0,out,{"Drive"})==-1);
  }
  for(int length:{-1,std::numeric_limits<int>::min(),std::numeric_limits<int>::max(),1000000000})
  {
    iplug::IByteChunk bad;bad.Put(&length);WDL_String text;
    assert(IREvanReadString(bad,text,0)==-1);
    nlohmann::json out;assert(IREvanReadPathsAndParams(bad,0,out,{"Drive"})==-1);
  }
  std::cout<<"PASS: 13/18-param byte-chunk round trips, every truncated prefix, invalid offsets, NaN/Inf and malformed string-length rejection, atomic config update.\n";
}
