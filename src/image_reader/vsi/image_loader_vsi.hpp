
#pragma once

#include <string>
inline void convert(const std::string &in, const std::string &out)
{
  std::string command = "lib/bftools/bfconvert -overwrite " + in + " " + out + "";
  system(command.c_str());
}
