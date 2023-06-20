#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>

#include "image_loader_tif.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("image:loader", "[image_loader]")
{
  auto prop = TiffLoader::readOmeInformation("test/GMEV5minM1OT3_0001.btf");

  CHECK(3 == prop.getDirectoryForChannel(0, 0).size());
}
