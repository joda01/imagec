///
/// \file      cpu_flow_field.cpp
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include <iostream>
#include <random>
#include <stdexcept>
#include <opencv2/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/persistence.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

cv::Vec2f bilinearInterpolate(const cv::Mat &flowX, const cv::Mat &flowY, float x, float y);

///
/// \brief      Follow flow field from (startX, startY) for numSteps with stepSize
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Point2f cpuFlowIterationKernel(const cv::Mat &flowX, const cv::Mat &flowY, float startX, float startY, int numSteps, float stepSize)
{
  float x = startX;
  float y = startY;

  int width  = flowX.cols;
  int height = flowX.rows;

  for(int i = 0; i < numSteps; ++i) {
    cv::Vec2f flow = bilinearInterpolate(flowX, flowY, x, y);
    float epsilon  = 0.0001F;
    if(std::abs(flow[0]) < epsilon && std::abs(flow[1]) < epsilon) {
      break;    // converged
    }
    x += stepSize * flow[0];
    y += stepSize * flow[1];
    // Clamp position inside image bounds
    x = std::clamp(x, 0.0f, static_cast<float>(width - 1));
    y = std::clamp(y, 0.0f, static_cast<float>(height - 1));
  }

  return {x, y};
}

///
/// \brief      Bilinear interpolation of flow vectors at floating point (x, y)
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Vec2f bilinearInterpolate(const cv::Mat &flowX, const cv::Mat &flowY, float x, float y)
{
  int width  = flowX.cols;
  int height = flowX.rows;

  int x0 = static_cast<int>(std::floor(x));
  int y0 = static_cast<int>(std::floor(y));
  int x1 = x0 + 1;
  int y1 = y0 + 1;

  float dx = x - static_cast<float>(x0);
  float dy = y - static_cast<float>(y0);

  // Clamp to valid image coordinates
  x0 = std::clamp(x0, 0, width - 1);
  x1 = std::clamp(x1, 0, width - 1);
  y0 = std::clamp(y0, 0, height - 1);
  y1 = std::clamp(y1, 0, height - 1);

  // Sample flowX at 4 neighbors
  float Q11_x = flowX.at<float>(y0, x0);
  float Q12_x = flowX.at<float>(y0, x1);
  float Q21_x = flowX.at<float>(y1, x0);
  float Q22_x = flowX.at<float>(y1, x1);

  // Sample flowY at 4 neighbors
  float Q11_y = flowY.at<float>(y0, x0);
  float Q12_y = flowY.at<float>(y0, x1);
  float Q21_y = flowY.at<float>(y1, x0);
  float Q22_y = flowY.at<float>(y1, x1);

  // Bilinear interpolation
  float flow_x = (1 - dx) * (1 - dy) * Q11_x + dx * (1 - dy) * Q12_x + (1 - dx) * dy * Q21_x + dx * dy * Q22_x;
  float flow_y = (1 - dx) * (1 - dy) * Q11_y + dx * (1 - dy) * Q12_y + (1 - dx) * dy * Q21_y + dx * dy * Q22_y;

  return {flow_x, flow_y};
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Mat paintLabels(const cv::Mat &labels)
{
  CV_Assert(labels.type() == CV_32S);

  int rows = labels.rows;
  int cols = labels.cols;

  cv::Mat colorMap(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0));

  std::unordered_map<int, cv::Vec3b> labelColors;

  // Random generator for colors
  std::mt19937 rng(12345);    // fixed seed for reproducibility
  std::uniform_int_distribution<int> dist(0, 255);

  for(int y = 0; y < rows; ++y) {
    const int *labelRow = labels.ptr<int>(y);
    cv::Vec3b *colorRow = colorMap.ptr<cv::Vec3b>(y);

    for(int x = 0; x < cols; ++x) {
      int label = labelRow[x];
      if(label == 0) {
        // background or unlabeled pixels -> black
        colorRow[x] = cv::Vec3b(0, 0, 0);
        continue;
      }

      // Assign color if not yet assigned
      if(labelColors.find(label) == labelColors.end()) {
        labelColors[label] = cv::Vec3b(dist(rng), dist(rng), dist(rng));
      }

      colorRow[x] = labelColors[label];
    }
  }

  return colorMap;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void drawFlowArrows(const cv::Mat &flowX, const cv::Mat &flowY, cv::Mat &outImage, int stride, float scale, cv::Scalar color)
{
  outImage = cv::Mat::zeros(flowX.size(), CV_8UC3);    // black canvas

  for(int y = 0; y < flowX.rows; y += stride) {
    for(int x = 0; x < flowX.cols; x += stride) {
      float fx = flowX.at<float>(y, x);
      float fy = flowY.at<float>(y, x);

      cv::Point2f start(static_cast<float>(x), static_cast<float>(y));
      cv::Point2f end(static_cast<float>(x) + fx * scale, static_cast<float>(y) + fy * scale);

      cv::arrowedLine(outImage, start, end, color, 1, cv::LINE_AA, 0, 0.3);
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void clusterLandingPoints(std::map<std::pair<int, int>, int> &landingLabelMap,
                          cv::Mat &outLabelImageDebug,    // Output: debug color labels
                          cv::Mat &outBinaryMaskDebug     // Output: debug binary mask
)
{
  // 1. Find bounding box
  int minX = std::numeric_limits<int>::max();
  int maxX = std::numeric_limits<int>::min();
  int minY = std::numeric_limits<int>::max();
  int maxY = std::numeric_limits<int>::min();

  for(const auto &[pt, _] : landingLabelMap) {
    int x = pt.first;
    int y = pt.second;
    minX  = std::min(minX, x);
    maxX  = std::max(maxX, x);
    minY  = std::min(minY, y);
    maxY  = std::max(maxY, y);
  }

  int width  = maxX - minX + 1;
  int height = maxY - minY + 1;

  if(width <= 0 || height <= 0) {
    std::cerr << "Invalid landing point bounds." << std::endl;
    return;
  }

  // 2. Create binary mask
  cv::Mat binaryMask = cv::Mat::zeros(height, width, CV_8UC1);
  for(const auto &[pt, label] : landingLabelMap) {
    int x                      = pt.first - minX;
    int y                      = pt.second - minY;
    binaryMask.at<uchar>(y, x) = 255;
  }

  // Optional debug: visualize binary landing point mask
  outBinaryMaskDebug = binaryMask.clone();

  // 3. Connected components
  cv::Mat ccLabels;
  int numComponents = cv::connectedComponents(binaryMask, ccLabels, 8, CV_32S);

  // Optional debug: visualize components as color image
  cv::Mat ccLabelsColor;
  ccLabels.convertTo(ccLabels, CV_16U);    // For color map
  ccLabels *= 1000;                        // Scale for better color separation
  cv::Mat ccLabels8U;
  ccLabels.convertTo(ccLabels8U, CV_8U, 1.0 / 256);    // bring back to 8-bit range
  cv::applyColorMap(ccLabels8U, outLabelImageDebug, cv::COLORMAP_JET);

  // 4. Update map with clustered label IDs
  for(auto &[pt, label] : landingLabelMap) {
    int x            = pt.first - minX;
    int y            = pt.second - minY;
    int clusterLabel = ccLabels.at<int>(y, x);
    label            = clusterLabel;
  }

  std::cout << "Clustering complete. Found " << numComponents << " components." << std::endl;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Vec3b flowToColor(float flow_x, float flow_y)
{
  float magnitude = std::sqrt(flow_x * flow_x + flow_y * flow_y);
  float angle     = std::atan2(flow_y, flow_x);    // Radians

  // Normalize angle to [0, 180) for hue (OpenCV uses 0–180 for H)
  float hue = static_cast<float>((static_cast<double>(angle) + CV_PI) * 90.0 / CV_PI);

  // Normalize magnitude (optional: scale as needed)
  float mag_normalized = std::min(1.0F, magnitude / 10.0F);    // assuming max mag = 10

  // Create HSV color: H [0,180], S [0,255], V [0,255]
  cv::Mat hsv(1, 1, CV_8UC3, cv::Scalar(static_cast<int32_t>(hue), 255, static_cast<int32_t>(mag_normalized * 255.0F)));
  cv::Mat bgr;
  cv::cvtColor(hsv, bgr, cv::COLOR_HSV2BGR);

  return bgr.at<cv::Vec3b>(0, 0);
}
