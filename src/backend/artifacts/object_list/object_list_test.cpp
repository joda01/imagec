#include "backend/artifacts/roi/roi.hpp"
#include "backend/enums/enums_classes.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <opencv2/core/mat.hpp>
#include "object_list.hpp"

namespace joda::test {

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
SCENARIO("object_list", "[object_list]")
{
  /*
  RoiObjectId index, Confidence confidence, const Boxes &boundingBox, const cv::Mat &mask, const std::vector<cv::Point> &contour,
        const cv::Size &imageSize, const cv::Size &originalImageSize, const enums::tile_t &tile, const cv::Size &tileSize
  */

  joda::atom::ObjectList list;

  atom::ROI::RoiObjectId index{.classId = enums::ClassId::C10, .imagePlane = {.tStack = 1, .zStack = 2, .cStack = 3}};
  atom::Confidence confidence = 123.456;
  atom::Boxes boundingBox(4, 5, 100, 200);
  cv::Mat mask = cv::Mat::zeros({50, 60}, CV_8UC1);
  for(int x = 0; x < 50; x++) {
    for(int y = 0; y < 60; y++) {
      mask.at<uint8_t>(y, x) = 1;
    }
  }

  std::vector<cv::Point> contour;
  cv::Size imageSize(500, 600);
  cv::Size originalImageSize(700, 800);
  enums::tile_t tile{7, 8};
  cv::Size tileSize{500, 600};

  joda::atom::ROI myRoi(index, confidence, boundingBox, mask, contour, imageSize, originalImageSize, tile, tileSize);
  list.push_back(myRoi);
  list.serialize("tmp/serialized.bin");

  CHECK(1 == 1);
}
}    // namespace joda::test
