#include "image_reader/jpg/image_loader_jpg.hpp"
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
  cv::Mat img = JpgLoader::loadEntireImage("test/cells.jpg", 0);
  joda::func::ai::ObjectSegmentation seg("imagec_models/cell_segmentation_brightfield_in_vitro_v1.onnx", {"cell"});
  auto result = seg.forward(img);
  seg.paintBoundingBox(img, result);
  imwrite("test/out.jpg", img);
  CHECK(result.size() == 15);
}
