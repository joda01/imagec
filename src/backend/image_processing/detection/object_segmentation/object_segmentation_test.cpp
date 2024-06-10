#include "../../../settings/channel/channel_settings.hpp"
#include "backend/image_processing/reader/jpg/image_loader_jpg.hpp"
#include "backend/settings/detection/detection_settings_threshold.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/dnn/dnn.hpp>

#include "object_segmentation.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("thresholdt", "[.][tjreshold]")
{
  cv::Mat img = joda::image::JpgLoader::loadEntireImage("test_cell/test.jpg");
  ::joda::settings::ChannelSettings ch;
  joda::image::segment::ObjectSegmentation seg(ch.objectFilter, 1000, joda::settings::ThresholdSettings::Mode::LI,
                                               false);
  cv::Mat res;
  auto result = seg.forward(img, img, joda::settings::ChannelIndex::CH0);
  // CHECK(result.result.size() == 15);
}
