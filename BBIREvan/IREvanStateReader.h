#pragma once
#include <cmath>
#include <string>
#include <vector>
#include "IPlugStructs.h"
#include "../NeuralAmpModelerCore/Dependencies/nlohmann/json.hpp"

// iPlug's general GetStr accepts signed lengths. Bound the string explicitly
// before creating a path or version string from untrusted host state bytes.
inline int IREvanReadString(const iplug::IByteChunk& chunk, WDL_String& text, int pos)
{
  if(pos<0 || pos>chunk.Size() || chunk.Size()-pos<static_cast<int>(sizeof(int))) return -1;
  int length=0;pos=chunk.Get(&length,pos);
  if(pos<0 || length<0 || length>chunk.Size()-pos) return -1;
  if(length==0)text.Set("");
  else text.Set(reinterpret_cast<const char*>(chunk.GetData()+pos),length);
  return pos+length;
}

// Shared by the native state loader and standalone regression test. Reject a
// partial/corrupt chunk before changing the caller's configuration object.
inline int IREvanReadPathsAndParams(const iplug::IByteChunk& chunk, int startPos,
                                    nlohmann::json& config, const std::vector<std::string>& names)
{
  if(startPos<0 || startPos>chunk.Size())return -1;
  nlohmann::json staged;
  WDL_String path;
  int pos=IREvanReadString(chunk,path,startPos);
  if(pos<0)return -1;
  staged["NAMPath"]=std::string(path.Get());
  pos=IREvanReadString(chunk,path,pos);
  if(pos<0)return -1;
  staged["IRPath"]=std::string(path.Get());
  for(const auto& name:names)
  {
    double value=0.0;pos=chunk.Get(&value,pos);
    if(pos<0 || !std::isfinite(value))return -1;
    staged[name]=value;
  }
  config=std::move(staged);
  return pos;
}
