#include "starter/starter.hpp"

#ifdef _WIN32
#elif defined(__APPLE__)
#else
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
void crash_handler(int sig)
{
  void *array[50];
  size_t size = backtrace(array, 50);

  std::cerr << "\n=== CRASH: signal " << sig << " ===\n";
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  std::cerr << "===============================\n";

  exit(1);
}
#endif

int main(int argc, char *argv[])
{
#ifdef _WIN32
#elif defined(__APPLE__)
#else
  //  setenv("LD_LIBRARY_PATH", "./lib", 1);
  setenv("FONTCONFIG_PATH", "/etc/fonts/", 1);
  setenv("QT_SCALE_FACTOR_ROUNDING_POLICY", "PassThrough", 1);
  setenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0", 1);
  setenv("QT_SCALE_FACTOR", "1", 1);
  setenv("QT_SCREEN_SCALE_FACTORS", "1", 1);

  //
  // signal(SIGSEGV, crash_handler);
  // signal(SIGABRT, crash_handler);
  // signal(SIGFPE, crash_handler);
  // signal(SIGILL, crash_handler);
  // signal(SIGBUS, crash_handler);

#endif

  joda::start::Starter cmdLine;
  cmdLine.exec(argc, argv);
  return 0;
}
