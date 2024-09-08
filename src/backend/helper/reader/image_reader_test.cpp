#include <string>
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/imgcodecs.hpp>
#include "image_reader.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("image:loader:bioformats", "[.][image_loader_bioformats]")
{
  joda::image::reader::ImageReader::init();

  auto img = joda::image::reader::ImageReader::loadEntireImage("test/test_histo/cond1_MEVnoP_178.vsi", {0, 0, 0}, 0, 0);

  // img = img * (256.0F / 65536);
  cv::Mat grayImageFloat;
  img.convertTo(grayImageFloat, CV_8UC3, (float) UCHAR_MAX / (float) UINT16_MAX);
  grayImageFloat *= 1;
  cv::imwrite("test/img/test_bioformats.png", grayImageFloat);    // A JPG FILE IS BEING SAVED

  joda::image::reader::ImageReader::destroy();
}

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("image:loader:bioformats:pyramid", "[image_loader_pyramid]")
{
  joda::image::reader::ImageReader::init();

  joda::ome::OmeInfo ome;
  auto omeXML = joda::image::reader::ImageReader::getOmeInformation(
      "/workspaces/imagec/test/test_data/images_pyramid/pyramid.ome.btf");

  auto img = joda::image::reader::ImageReader::loadEntireImage(
      "/workspaces/imagec/test/test_data/images_pyramid/pyramid.ome.btf", {0, 0, 0}, 0, 1);
  img *= 256;
  cv::imwrite("tmp/test.jpg", img);    // A JPG FILE IS BEING SAVED

  joda::image::reader::ImageReader::destroy();
}
