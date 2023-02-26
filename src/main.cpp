#include <opencv2/core/hal/interface.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/core.hpp>
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
  ai::ObjectDetector obj("/workspaces/open-bio-image-processor/test/best.onnx", {"nuclues"});
  Mat image   = imread("/workspaces/open-bio-image-processor/test/nucleus_03.tiff", CV_32FC3);
  auto result = obj.forward(image);
  obj.paintBoundingBox(image, result);
  imwrite("image_out.jpg", image);

  return 0;
}
