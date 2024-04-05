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
#include "rank_filter.hpp"
///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("func::rank", "[rank]")
{
  BioformatsLoader::init();

  auto img         = BioformatsLoader::loadEntireImage("test/test_in_cell_counting/A10_01.vsi", 0, 0);
  auto imgOriginal = img.clone();

  auto imgClone = img.clone();
  imgClone      = imgClone * ((float) 255.0F / (float) 65536.0F) * 255;
  cv::imwrite("zz_start.jpg", imgClone);

  joda::func::img::RankFilter rank;
  rank.rank(img, 3.0, joda::func::img::RankFilter::MEDIAN);

  auto imgCloneRes = img.clone();
  imgCloneRes      = imgCloneRes * ((float) 255.0F / (float) 65536.0F) * 255;

  cv::imwrite("zz_result_01.jpg", imgCloneRes);

  img = imgOriginal - img;
  img = img = img * ((float) 255.0F / (float) 65536.0F) * 255;
  cv::imwrite("zz_result.jpg", img);
}
