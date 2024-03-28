#include "../../../image_reader/jpg/image_loader_jpg.hpp"
#include "../../../image_reader/tif/image_loader_tif.hpp"
#include "../../../settings/channel_settings.hpp"
#include "backend/image_processing/functions/blur/blur.hpp"
#include "backend/image_processing/functions/threshold/threshold.hpp"
#include "backend/image_processing/functions/threshold/threshold_manual.hpp"
#include "backend/image_reader/bioformats/bioformats_loader.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/imgcodecs.hpp>
#include "watershed.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("func::watershed", "[watershed]")
{
  BioformatsLoader::init();

  auto img = BioformatsLoader::loadEntireImage("test/test_in_cell_counting/A10_01.vsi", 1, 0);
  joda::func::img::RollingBallBackground bg(joda::func::img::RollingBallBackground::Configuration::BALL, 3);
  joda::func::img::Blur blur(1);
  joda::func::img::Watershed ws;
  joda::func::img::ThresholdManual th(100);
  bg.execute(img);
  blur.execute(img);
  cv::Mat binaryImage;
  th.execute(img, binaryImage);
  ws.execute(binaryImage);

  cv::Mat grayImageFloat;
  binaryImage = binaryImage;
  binaryImage.convertTo(grayImageFloat, CV_32F);
  cv::Mat inputImage;
  cv::cvtColor(grayImageFloat, inputImage, cv::COLOR_GRAY2BGR);
  cv::imwrite("rollingball.jpg", inputImage);

  // img2 = img2 * ((float) 255.0F / (float) 65536.0F) * 250;
}
