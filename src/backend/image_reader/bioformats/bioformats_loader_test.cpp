#include <string>
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/imgcodecs.hpp>

#include "bioformats_loader.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("image:loader:bioformats", "[image_loader_bioformats]")
{
  auto prop = BioformatsLoader::loadEntireImage("test/test_spot/CD63mNeon_001.vsi", 0);
}
