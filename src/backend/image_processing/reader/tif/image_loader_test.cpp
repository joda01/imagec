#include <string>
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/imgcodecs.hpp>

#include "image_loader_tif.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("image:loader:ome", "[image_loader_ome")
{
  auto prop = joda::image::TiffLoader::getOmeInformation("test/GMEV5minM1OT3_0001.btf");

  CHECK(3 == prop.getDirectoryForChannel(0, 0).size());
}

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("image:loader:bigtiff", "[image_loader_bigtiff]")
{
  auto prop = joda::image::TiffLoader::getImageProperties("test/GMEV5minM1OT3_0001.btf", 0);
  std::cout << std::to_string(prop.nrOfTiles) << std::endl;

  for(int n = 0; n < prop.nrOfTiles / 36; n++) {
    auto img = joda::image::TiffLoader::loadImageTile("test/GMEV5minM1OT3_0001.btf", 3, n, 36);
    img *= 0.003906250;
    cv::imwrite("test/img/test_" + std::to_string(n) + ".jpg", img);    // A JPG FILE IS BEING SAVED
  }
}

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("image:loader:single", "[image_loader_single]")
{
  auto img = joda::image::TiffLoader::loadEntireImage(
      "test_cell/B2_0001.vsi - 001 CY7, CY5, Cy5-Cy7-FRET, DAPI_MB, BF - Z=5.tif", 4);
  img *= 0.003906250;
  cv::imwrite("test_cell/test.jpg", img);    // A JPG FILE IS BEING SAVED
}
