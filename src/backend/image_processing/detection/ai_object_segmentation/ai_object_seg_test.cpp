#include "../../../image_reader/jpg/image_loader_jpg.hpp"
#include "../../../settings/channel/channel_settings_filter.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/dnn/dnn.hpp>

#include "ai_object_segmentation.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("cell:counter:ai", "[cell_counter_ai]")
{
  cv::Mat img = JpgLoader::loadEntireImage("test/cells.jpg");
  joda::settings::ChannelSettingsFilter ch;
  joda::onnx::OnnxParser::Data data;
  data.modelPath = "models/cell_segmentation_brightfield_in_vitro_v1.onnx";
  data.classes   = {"cell"};
  joda::func::ai::ObjectSegmentation seg(ch, data, 0.5);
  auto result = seg.forward(img, img, joda::settings::ChannelIndex::CH0);
  CHECK(result.result.size() == 15);
}
