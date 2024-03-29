#include "../../../image_reader/jpg/image_loader_jpg.hpp"
#include "../../../settings/channel_settings.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/dnn/dnn.hpp>

#include "object_segmentation.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("thresholdt", "[tjreshold]")
{
  cv::Mat img = JpgLoader::loadEntireImage("test_cell/test.jpg");
  ::joda::settings::json::ChannelSettings ch;
  joda::func::threshold::ObjectSegmentation seg(ch.getFilter(), 1000,
                                                joda::settings::json::ThresholdSettings::Threshold::MANUAL, false);
  cv::Mat res;
  auto result = seg.forward(img, img, 0);
  // CHECK(result.result.size() == 15);
}
