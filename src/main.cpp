#include <opencv2/core.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>

#include <opencv2/core/hal/interface.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "algorithms/rolling_ball/rolling_ball.hpp"

////
#include <fstream>
#include <iostream>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace dnn;

void classifier();

////

int main(int argc, char **argv)
{
  classifier();
  return 0;
  std::string inputImage = "img/test.vsi";
  std::string ouputImage = "img/test.tiff";
  std::string command    = "lib/bftools/bfconvert -overwrite " + inputImage + " " + ouputImage + "";

  // system(command.c_str());

  /// Load an image
  auto canRead = cv::haveImageReader("img/test.tiff");
  std::cout << "Can read " << canRead << std::endl;

  std::vector<Mat> channels;
  imreadmulti("img/test.tiff", channels, cv::ImreadModes::IMREAD_GRAYSCALE);

  int nr             = 0;
  auto originakImage = channels[4];
  if(originakImage.channels() == 1) {
    auto start = std::chrono::steady_clock::now();

    RollingBallBackground *bg = new RollingBallBackground();

    bg->run(originakImage);
    imwrite("output/rollingBall" + std::to_string(nr) + ".jpg", originakImage);

    //
    // Thersholding
    //
    Mat thersoldImage;
    threshold(originakImage, thersoldImage, 15, 255,
              cv::ThresholdTypes::THRESH_TRIANGLE | cv::ThresholdTypes::THRESH_BINARY);

    imwrite("output/thresholded" + std::to_string(nr) + ".jpg", thersoldImage);

    //
    // Stop timer
    //
    auto end                                      = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

    //
    // Generate markers
    //
    Mat dist_transform;
    distanceTransform(thersoldImage, dist_transform, DistanceTypes::DIST_L2, 5);
    dist_transform *= 2;
    imwrite("output/dist_transform" + std::to_string(nr) + ".jpg", dist_transform);

    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;
    minMaxLoc(dist_transform, &minVal, &maxVal, &minLoc, &maxLoc);
    Mat sure_fg;
    std::cout << "MAX " << std::to_string(maxVal) << std::endl;
    threshold(dist_transform, sure_fg, maxVal * 0.7, 255, cv::ThresholdTypes::THRESH_BINARY);
    imwrite("output/sure_fg" + std::to_string(nr) + ".jpg", sure_fg);

    Mat markers;
    sure_fg.convertTo(sure_fg, CV_8U);
    connectedComponents(sure_fg, markers);
    imwrite("output/markers" + std::to_string(nr) + ".jpg", markers);

    //
    // Watershed
    //
    Mat colorImg;
    cvtColor(thersoldImage, colorImg, COLOR_GRAY2BGR);
    watershed(colorImg, markers);
    imwrite("output/colorImg" + std::to_string(nr) + ".jpg", colorImg);
    imwrite("output/markerso" + std::to_string(nr) + ".jpg", markers);
    Mat result;
    markers.convertTo(markers, CV_8UC1);
    threshold(markers, result, 1, 255, cv::THRESH_BINARY);
    imwrite("output/result" + std::to_string(nr) + ".jpg", result);
  }

  // imwrite("sub_2.jpg", drawing);

  return 0;
}

cv::Mat format_yolov5(const cv::Mat &source);

// https://learnopencv.com/deep-learning-with-opencvs-dnn-module-a-definitive-guide/
void classifier()
{
  auto model = cv::dnn::readNetFromONNX("models/best.onnx");

  // read the image from disk
  Mat image = imread("/workspaces/evanalzer_v2/img/1b3078ec-2.jpg");
  image     = format_yolov5(image);

  std::vector<cv::Mat> predictions;
  model.setInput(image);
  model.forward(predictions, model.getUnconnectedOutLayersNames());
  const cv::Mat &output = predictions[0];

  // imshow("image", image);
  imwrite("image_result.jpg", output);
  // waitKey(0);
  // destroyAllWindows();
}
int INPUT_WIDTH  = 640;
int INPUT_HEIGHT = 640;

cv::Mat format_yolov5(const cv::Mat &source)
{
  // put the image in a square big enough
  int col         = source.cols;
  int row         = source.rows;
  int _max        = MAX(col, row);
  cv::Mat resized = cv::Mat::zeros(_max, _max, CV_8UC3);
  source.copyTo(resized(cv::Rect(0, 0, col, row)));

  // resize to 640x640, normalize to [0,1[ and swap Red and Blue channels
  cv::Mat result;
  cv::dnn::blobFromImage(source, result, 1. / 255., cv::Size(INPUT_WIDTH, INPUT_HEIGHT), cv::Scalar(), true, false);

  return result;
}
