#include "../../../settings/channel/channel_settings_filter.hpp"
#include "backend/image_processing/detection/detection.hpp"
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/image_processing/reader/bioformats/bioformats_loader.hpp"
#include "backend/image_processing/reader/jpg/image_loader_jpg.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/dnn/dnn.hpp>
#include "ai_object_segmentation.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("cell:counter:ai", "[.][cell_counter_ai]")
{
  joda::image::BioformatsLoader::init();
  auto img = joda::image::BioformatsLoader::loadEntireImage("test/test_heatmap_small/A10_06.vsi", 4, 0, 0);
  joda::settings::ChannelSettingsFilter ch;
  joda::onnx::OnnxParser::Data data;
  // data.modelPath = "models/cell-brightfield-02-v1.onnx";
  data.modelPath = "models/cell-brightfield-02.onnx";
  data.classes   = {"cell", "cell_cut"};
  joda::image::segment::ai::ObjectSegmentation seg(ch, data, 0.5);
  auto result = seg.forward(img, img, joda::settings::ChannelIndex::CH4);

  CHECK(result.result->size() == 15);
}
