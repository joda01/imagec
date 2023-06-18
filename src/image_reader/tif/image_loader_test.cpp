#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>

#include "image_loader_tif.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("image:loader", "[image_loader]")
{
  TiffLoader::readImageMeta("test/GMEV5minM1OT3_0001.btf");
}
