#include <cstdlib>
#include <string>
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/imgcodecs.hpp>

#include "image_loader_vsi.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("image:loader:vsi:ome", "[image_loader_vsi_ome")
{
  auto prop = TiffLoader::getOmeInformation("test/GMEV5minM1OT3_0001.btf");

  CHECK(3 == prop.getDirectoryForChannel(0, 0).size());
}

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("image:loader:vsi:single", "[image_loader_vsi_single]")
{
  std::string inputImage = "test_areosold_Evs/hoeachst.vsi";

  // std ::string cmd = "./lib/bftools/bfconvert -overwrite -tilex 512 -tiley 512 " + inputImage + " tmp.tiff";
  std::string cmd = "./lib/bftools/bfconvert -overwrite " + inputImage + " test_areosold_Evs/hoechst.tiff";
  std::system(cmd.c_str());

  // auto img = TiffLoader::loadEntireImage("tmp.tiff", 0);
  // img *= 0.003906250;
  // cv::imwrite("test_spot/test_00.jpg", img);    // A JPG FILE IS BEING SAVED
  //
  // img = TiffLoader::loadEntireImage("tmp.tiff", 1);
  // img *= 0.003906250;
  // cv::imwrite("test_spot/test_01.jpg", img);    // A JPG FILE IS BEING SAVED
  //
  // img = TiffLoader::loadEntireImage("tmp.tiff", 2);
  // img *= 0.003906250;
  // cv::imwrite("test_spot/test_02.jpg", img);    // A JPG FILE IS BEING SAVED
  //
  // img = TiffLoader::loadEntireImage("tmp.tiff", 3);
  // img *= 0.003906250;
  // cv::imwrite("test_spot/test_03.jpg", img);    // A JPG FILE IS BEING SAVED
}
