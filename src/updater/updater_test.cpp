#include <iostream>
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include "updater.hpp"

SCENARIO("Updater test", "[updater]")
{
  int argc     = 1;
  char *argv[] = {"string1"};

  joda::upd::Updater upd(argc, argv);
  std::cout << "A: " << upd.getRemoteHash() << std::endl;
  std::cout << "B: " << upd.getRemoteVersion() << std::endl;
}
