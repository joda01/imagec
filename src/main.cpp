#include <opencv2/core/hal/interface.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "image_reader/tif/image_loader_tif.hpp"
#include "image_reader/vsi/image_loader_vsi.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>

#include "algorithms/rolling_ball/rolling_ball.hpp"

////
#include <fstream>
#include <iostream>
#include "algorithms/ai_object_detection/ai_object_detection.h"
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace dnn;

////

int main(int argc, char **argv)
{
  std::string imgName = "test/GMEV5minM1OT3_0001.btf";

  // convert("test/GMEV5minM1OT3_0001.btf", imgName);

  for(int n = 0; n < 200; n++) {
    try {
      auto tilePart = TiffLoader::loadImageTile(imgName, 14, n, 36);
      tilePart *= 10;
      cv::imwrite("out/bigtiff" + std::to_string(n) + ".jpg", tilePart);
      // ai::ObjectDetector obj("/workspaces/open-bio-image-processor/test/best.onnx", {"nuclues", "nucleus_no_focus"});
      // auto result = obj.forward(tilePart);
      // obj.paintBoundingBox(tilePart, result);
      // imwrite("pred/image_out" + std::to_string(n) + ".jpg", tilePart);
    } catch(...) {
    }
  }

  return 0;
  for(int n = 0; n < 80; n++) {
    auto tilePart = TiffLoader::loadImageTile(imgName, 14, n);
    tilePart *= 9;
    cv::imwrite("out/bigtiff" + std::to_string(n) + ".jpg", tilePart);
    ai::ObjectDetector obj("/workspaces/open-bio-image-processor/test/best.onnx", {"nuclues", "nucleus_no_focus"});
    auto result = obj.forward(tilePart);
    obj.paintBoundingBox(tilePart, result);
    imwrite("pred/image_out" + std::to_string(n) + ".jpg", tilePart);
  }

  // TiffLoader::loadEntireImage(imgName, 0);

  return 0;
}
