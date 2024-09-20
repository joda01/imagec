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

#elif defined(__APPLE__)

#include <mach/mach.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

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

#elif defined(__APPLE__)

inline uint64_t getTotalSystemMemory()
{
  uint64_t totalMemory = 0;
  size_t length        = sizeof(totalMemory);
  int mib[2]           = {CTL_HW, HW_MEMSIZE};

  // Rufe sysctl auf, um die Gesamtmenge des physischen Speichers abzufragen
  if(sysctl(mib, 2, &totalMemory, &length, NULL, 0) < 0) {
    perror("sysctl");
    return 0;    // Fehlerbehandlung
  }

  return totalMemory;
}

inline uint64_t getAvailableSystemMemory()
{
  return (uint64_t) ((float) getTotalSystemMemory() * (float) 0.8);
  /*
  uint64_t totalMemory = 0;
  size_t length        = sizeof(totalMemory);
  int mib[2]           = {CTL_HW, HW_MEMSIZE};

  // Get total physical memory
  if(sysctl(mib, 2, &totalMemory, &length, NULL, 0) < 0) {
    perror("sysctl");
    return 0;
  }

  // Get available memory using vm_statistics
  vm_statistics_data_t vmStats;
  mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
  if(host_statistics(mach_host_self(), HOST_VM_INFO, &vmStats, &count) != KERN_SUCCESS) {
    perror("host_statistics");
    return 0;
  }

  // Calculate available memory
  uint64_t availableMemory = vmStats.free_count * vmStats.page_size;

  return availableMemory;
  */
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
