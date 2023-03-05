
#pragma once

#include <string>
inline void convert(const std::string &in, const std::string &out)
{
  std::string command = "lib/bftools/bfconvert -overwrite -tilex 1024 -tiley 1024 " + in + " " + out + "";
  system(command.c_str());
}
