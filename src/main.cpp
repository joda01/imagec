#include "starter/starter.hpp"

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

#endif

  joda::start::Starter cmdLine;
  cmdLine.exec(argc, argv);
  return 0;
}
