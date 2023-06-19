#include "image_reader/jpg/image_loader_jpg.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/dnn/dnn.hpp>

#include "ai_object_seg.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("cell:counter:ai", "[cell_counter_ai]")
{
  std::vector<cv::Scalar> color;
  srand(time(0));
  for(int i = 0; i < 80; i++) {
    int b = rand() % 256;
    int g = rand() % 256;
    int r = rand() % 256;
    color.push_back(cv::Scalar(b, g, r));
  }

  cv::dnn::Net net;
  std::vector<OutputSeg> output;
  cv::Mat img = JpgLoader::loadEntireImage("test/cells.jpg", 0);
  YoloSeg seg;
  seg.ReadModel(net, "imagec_models/cell_segmentation_brightfield_in_vitro_v1.onnx", false);
  seg.Detect(img, net, output);
  seg.DrawPred(img, output, color);
}
