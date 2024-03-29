#include <opencv2/core/hal/interface.h>
#include "../../../image_reader/jpg/image_loader_jpg.hpp"
#include "../../../image_reader/tif/image_loader_tif.hpp"
#include "../../../settings/channel_settings.hpp"
#include "backend/image_processing/functions/blur/blur.hpp"
#include "backend/image_processing/functions/threshold/threshold.hpp"
#include "backend/image_processing/functions/threshold/threshold_manual.hpp"
#include "backend/image_reader/bioformats/bioformats_loader.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include "edm.hpp"
#include "maximum_finder.hpp"
#include "watershed.hpp"
///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("func::watershed", "[watershed]")
{
  double MAXFINDER_TOLERANCE = 0.5;    // reasonable v
  BioformatsLoader::init();

  auto img = BioformatsLoader::loadEntireImage("test/test_in_cell_counting/A10_01.vsi", 1, 0);
  joda::func::img::RollingBallBackground bg(joda::func::img::RollingBallBackground::Configuration::BALL, 3);
  joda::func::img::Blur blur(1);
  joda::func::img::ThresholdManual th(100);
  bg.execute(img);
  blur.execute(img);
  cv::Mat binaryImage;
  th.execute(img, binaryImage);

  cv::imwrite("zz_inout.jpg", binaryImage);

  joda::func::img::Watershed watershed;
  watershed.execute(binaryImage);
  cv::imwrite("zz_result.jpg", binaryImage);
}
