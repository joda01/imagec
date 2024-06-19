#include <opencv2/core/hal/interface.h>
#include "backend/helper/duration_count/duration_count.h"
#include "backend/image_processing/functions/blur/blur.hpp"
#include "backend/image_processing/functions/threshold/threshold.hpp"
#include "backend/image_processing/functions/threshold/threshold_manual.hpp"
#include "backend/image_processing/reader/bioformats/bioformats_loader.hpp"
#include "backend/image_processing/reader/jpg/image_loader_jpg.hpp"
#include "backend/image_processing/reader/tif/image_loader_tif.hpp"
#include "backend/settings/channel/channel_settings.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include "rank_filter.hpp"
///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("func::rank", "[.][rank]")
{
  joda::image::BioformatsLoader::init();

  auto img         = joda::image::BioformatsLoader::loadEntireImage("test/test_in_cell_counting/A10_01.vsi", 0, 0);
  auto imgOriginal = img.clone();

  auto imgClone = img.clone();
  imgClone      = imgClone * ((float) 255.0F / (float) 65536.0F) * 255;
  cv::imwrite("zz_start.jpg", imgClone);

  auto id = DurationCount::start("Start");
  joda::image::func::RankFilter rank;
  rank.rank(img, 3.0, joda::image::func::RankFilter::MEDIAN);
  DurationCount::stop(id);

  auto imgCloneRes = img.clone();
  imgCloneRes      = imgCloneRes * ((float) 255.0F / (float) 65536.0F) * 255;

  cv::imwrite("zz_result_01.jpg", imgCloneRes);

  img = imgOriginal - img;
  img = img = img * ((float) 255.0F / (float) 65536.0F) * 255;
  cv::imwrite("zz_result.jpg", img);
}
