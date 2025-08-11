///
/// \file      directories.hpp
/// \author    Joachim Danmayr
/// \date      2025-08-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <filesystem>
#include <stdexcept>
#include <string>

#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#include <mach-o/dyld.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <limits.h>
#include <unistd.h>
#endif

namespace joda::system {

// ------------------------------------------------------------------
// Get the absolute path of the running executable
// ------------------------------------------------------------------
inline std::filesystem::path getExecutablePath()
{
#if defined(__APPLE__)
  CFURLRef bundleURL = CFBundleCopyBundleURL(CFBundleGetMainBundle());
  char path[PATH_MAX];
  std::filesystem::path bundlePath;
  if(CFURLGetFileSystemRepresentation(bundleURL, true, (UInt8 *) path, PATH_MAX)) {
    bundlePath = std::filesystem::path(path);
  }
  CFRelease(bundleURL);
  return bundlePath.parent_path();
#elif defined(_WIN32)
  char buf[MAX_PATH];
  DWORD len = GetModuleFileNameA(NULL, buf, MAX_PATH);
  if(len == 0 || len == MAX_PATH)
    throw std::runtime_error("Failed to get executable path");
  return std::filesystem::canonical(buf).parent_path();

#elif defined(__linux__)
  char buf[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
  if(len == -1)
    throw std::runtime_error("Failed to read /proc/self/exe");
  buf[len] = '\0';
  return std::filesystem::canonical(buf).parent_path();

#else
  throw std::runtime_error("Unsupported platform");
#endif
}

}    // namespace joda::system
