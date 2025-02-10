#include "starter/starter.hpp"

int main(int argc, char *argv[])
{
#ifdef _WIN32
#elif defined(__APPLE__)
#else
  //  setenv("LD_LIBRARY_PATH", "./lib", 1);
  setenv("FONTCONFIG_PATH", "/etc/fonts/", 1);
#endif

  joda::start::Starter cmdLine;
  cmdLine.exec(argc, argv);
  return 0;
}
