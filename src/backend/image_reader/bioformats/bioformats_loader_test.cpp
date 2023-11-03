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
  auto img = BioformatsLoader::loadEntireImage("test/test_histo/cond1_MEVnoP_178.vsi", 14);

  // img = img * (256.0F / 65536);
  cv::Mat grayImageFloat;
  img.convertTo(grayImageFloat, CV_32F, (float) UCHAR_MAX / (float) UINT16_MAX);
  grayImageFloat *= 12;
  cv::imwrite("test/img/test_bioformats.png", grayImageFloat);    // A JPG FILE IS BEING SAVED
}
