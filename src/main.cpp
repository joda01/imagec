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
  std::string imgName = "test/test_images_nuclei/falte.tiff";

  // convert("test/test_images_nuclei/falte.vsi", imgName);

  TiffLoader::openTiff(imgName);

  return 0;
  //  convert("test/test_images_nuclei/standard.vsi", imgName);

  std::vector<Mat> channels;
  imreadmulti(imgName, channels, CV_32FC3);
  int i = 0;
  for(const auto &img : channels) {
    imwrite("image_out" + std::to_string(i) + ".jpg", img);
    i++;
  }

  // cv::max(channels[4], channels[9], channels[4]);
  // cv::max(channels[4], channels[14], channels[4]);
  channels[0] *= 6;
  auto image = channels[0];
  imwrite("image_in.jpg", image);

  auto t_start = std::chrono::high_resolution_clock::now();
  ai::ObjectDetector obj("/workspaces/open-bio-image-processor/test/best.onnx", {"nuclues", "nucleus_no_focus"});
  auto result            = obj.forward(image);
  auto t_end             = std::chrono::high_resolution_clock::now();
  double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();
  std::cout << "Duration " << std::to_string(elapsed_time_ms) << std::endl;

  obj.paintBoundingBox(image, result);

  imwrite("image_out.jpg", image);

  return 0;
}
