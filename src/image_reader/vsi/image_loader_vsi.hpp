
#pragma once

#include <string>
inline void convert(const std::string &in, const std::string &out)
{
  std::string command = "lib/bftools/bfconvert -bigtiff -channel 1 -tilex 512 -tiley 512 " + in + " " + out + "";
  system(command.c_str());
}
