

#pragma once

#include "backend/image_processing/roi/roi.hpp"

namespace joda::pipeline::reporting {

inline std::string validityToString(joda::func::ParticleValidity val)
{
  if(val == joda::func::ParticleValidity::UNKNOWN) {
    return "-";
  }
  if(val == joda::func::ParticleValidity::VALID) {
    return "valid";
  }
  std::string ret;
  if((joda::func::ParticleValidity)((int) val & (int) joda::func::ParticleValidity::TOO_BIG) ==
     joda::func::ParticleValidity::TOO_BIG) {
    ret += "size(big)";
  }

  if((joda::func::ParticleValidity)((int) val & (int) joda::func::ParticleValidity::TOO_SMALL) ==
     joda::func::ParticleValidity::TOO_SMALL) {
    if(!ret.empty()) {
      ret += " & ";
    }
    ret += "size(small)";
  }

  if((joda::func::ParticleValidity)((int) val & (int) joda::func::ParticleValidity::TOO_LESS_OVERLAPPING) ==
     joda::func::ParticleValidity::TOO_LESS_OVERLAPPING) {
    if(!ret.empty()) {
      ret += " & ";
    }
    ret += "intersect too small";
  }

  if((joda::func::ParticleValidity)((int) val & (int) joda::func::ParticleValidity::TOO_LESS_CIRCULARITY) ==
     joda::func::ParticleValidity::TOO_LESS_CIRCULARITY) {
    if(!ret.empty()) {
      ret += " & ";
    }
    ret += "circ.";
  }

  if((joda::func::ParticleValidity)((int) val & (int) joda::func::ParticleValidity::AT_THE_EDGE) ==
     joda::func::ParticleValidity::AT_THE_EDGE) {
    if(!ret.empty()) {
      ret += " & ";
    }
    ret += "edge";
  }

  if((joda::func::ParticleValidity)((int) val & (int) joda::func::ParticleValidity::REFERENCE_SPOT) ==
     joda::func::ParticleValidity::REFERENCE_SPOT) {
    if(!ret.empty()) {
      ret += " & ";
    }
    ret += "ref spot.";
  }
  return ret;
}
}    // namespace joda::pipeline::reporting
