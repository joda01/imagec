///
/// \file      system_memory.hpp
/// \author    Joachim Danmayr
/// \date      2023-10-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Get avialble system memory
///

#pragma once

#include <thread>

#ifdef _WIN32

#include <windows.h>
#include <iostream>

#else

#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "sys/sysinfo.h"
#include "sys/types.h"

#endif

namespace joda::system {

#ifdef _WIN32

inline uint64_t getTotalSystemMemory()
{
  MEMORYSTATUSEX status;
  status.dwLength = sizeof(status);
  GlobalMemoryStatusEx(&status);
  return status.ullTotalPhys;
}

inline uint64_t getAvailableSystemMemory()
{
  MEMORYSTATUSEX status;
  status.dwLength = sizeof(status);
  GlobalMemoryStatusEx(&status);
  return status.ullAvailPhys;
}

#else

inline uint64_t getTotalSystemMemory()
{
  struct sysinfo memInfo;
  sysinfo(&memInfo);
  uint64_t totalPhysMem = memInfo.totalram;
  totalPhysMem *= memInfo.mem_unit;

  return totalPhysMem;
}

// inline uint64_t getAvailableSystemMemory()
//{
//   struct sysinfo memInfo;
//   sysinfo(&memInfo);
//   uint64_t result = memInfo.freeram * memInfo.mem_unit;
//   result += memInfo.bufferram * memInfo.mem_unit;
//   return result;
// }

inline uint64_t getAvailableSystemMemory()
{
  std::ifstream meminfo("/proc/meminfo");
  if(!meminfo.is_open()) {
    return -1;    // Error opening the file
  }

  std::string line;
  while(std::getline(meminfo, line)) {
    if(line.find("MemAvailable:") != std::string::npos) {
      remove_if(line.begin(), line.end(), isspace);

      auto removeSubStr = [&line](const std::string &toRemove) {
        size_t found;
        while((found = line.find(toRemove)) != std::string::npos) {
          line.replace(found, std::string(toRemove).length(), "");
        }
      };
      removeSubStr("MemAvailable:");
      removeSubStr(" kB");
      return std::stol(line) * 1000;
    }
  }

  return -1;    // MemAvailable not found
}

#endif

inline unsigned int getNrOfCPUs()
{
  return std::thread::hardware_concurrency();
}

struct SystemResources
{
  uint64_t ramTotal;    // RAM in bytes
  uint64_t ramAvailable;
  uint32_t cpus;    // Nr. of CPUs
};

inline SystemResources acquire()
{
  SystemResources ret;
  ret.ramTotal     = getTotalSystemMemory();
  ret.ramAvailable = getAvailableSystemMemory();
  ret.cpus         = getNrOfCPUs();
  return ret;
}

}    // namespace joda::system
