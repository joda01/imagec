///
/// \file      ai_model_bioimage.cpp
/// \author    Joachim Danmayr
/// \date      2025-02-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include <random>
#include <string>
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core/types.hpp>

#undef slots
#include <ATen/core/TensorBody.h>
#include <stdexcept>
#include <opencv2/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/persistence.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "ai_model_cyto3.hpp"
#define slots Q_SLOTS

namespace joda::ai {

std::pair<cv::Mat, std::set<int>> followFlowField(const cv::Mat &flowX, const cv::Mat &flowY, const cv::Mat &masl, float maskThreshold);
cv::Vec2f bilinearInterpolate(const cv::Mat &flowX, const cv::Mat &flowY, float x, float y);
cv::Point2f followFlow(const cv::Mat &flowX, const cv::Mat &flowY, float startX, float startY, int numSteps = 10, float stepSize = 1.0f);
cv::Vec3b flowToColor(float flow_x, float flow_y);
void drawFlowArrows(const cv::Mat &flowX, const cv::Mat &flowY, cv::Mat &outImage, int stride = 10, float scale = 1.0,
                    cv::Scalar color = cv::Scalar(0, 255, 0));
void clusterLandingPoints(std::map<std::pair<int, int>, int> &landingLabelMap, cv::Mat &outLabelImageDebug, cv::Mat &outBinaryMaskDebug);
cv::Mat paintLabels(const cv::Mat &labels);

std::vector<AiModel::Result> extractObjectMasksAndBoundingBoxes(const cv::Mat &labelImage, const std::set<int> &labels);

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
AiModelCyto3::AiModelCyto3(const ProbabilitySettings &settings) : mSettings(settings)
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto AiModelCyto3::processPrediction(const cv::Mat &inputImage, const at::IValue &tensorIn) -> std::vector<Result>
{
  static const int CHANNEL_GRADIENTS_X = 0;
  static const int CHANNEL_GRADIENTS_Y = 1;
  static const int CHANNEL_MASKS       = 2;

  const int32_t originalHeight = inputImage.rows;
  const int32_t originalWith   = inputImage.cols;

  if(!tensorIn.isTuple()) {
    joda::log::logError("Expected model output to be a tuple (detections, seg_masks).");
    return {};
  }

  auto outputTuple = tensorIn.toTuple();
  std::cout << std::to_string(outputTuple->size()) << std::endl;

  auto maskTensor = outputTuple->elements()[0].toTensor();    // Shape: [N, 6]

  // Move tensor to CPU and convert to float32
  maskTensor = maskTensor.detach().to(torch::kCPU).to(torch::kFloat32);

  // ===============================
  // 0. Rescale to original image
  // ===============================
  at::Tensor tensorSmall     = maskTensor.to(at::kCPU).to(at::kFloat);
  torch::Tensor tensorCpu    = torch::upsample_bilinear2d(tensorSmall, {originalHeight, originalWith}, false);
  at::Tensor maskTensorImage = tensorCpu[0][CHANNEL_MASKS].clone().contiguous();          // [height, width]
  at::Tensor flowX           = tensorCpu[0][CHANNEL_GRADIENTS_X].clone().contiguous();    // [height, width]
  at::Tensor flowY           = tensorCpu[0][CHANNEL_GRADIENTS_Y].clone().contiguous();    // [height, width]

  // ===============================
  // 1. Convert mask to cv::mat
  // ===============================
  cv::Mat maskImage(originalHeight, originalWith, CV_32F, maskTensorImage.data_ptr<float>());
  cv::Mat flowXImage(originalHeight, originalWith, CV_32F, flowX.data_ptr<float>());
  cv::Mat flowYImage(originalHeight, originalWith, CV_32F, flowY.data_ptr<float>());

  // ===============================
  // 2. Follow the flow field, returns a object segmented mask
  // ===============================
  auto idx                        = DurationCount::start("Follow field");
  auto [segmentationMask, labels] = followFlowField(flowXImage, flowYImage, maskImage, mSettings.maskThreshold);
  DurationCount::stop(idx);

  return extractObjectMasksAndBoundingBoxes(segmentationMask, labels);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
std::vector<AiModel::Result> extractObjectMasksAndBoundingBoxes(const cv::Mat &labelImage, const std::set<int> &labels)
{
  CV_Assert(labelImage.type() == CV_32S || labelImage.type() == CV_8U);

  const int rows = labelImage.rows;
  const int cols = labelImage.cols;

  std::vector<AiModel::Result> result;
  for(int label : labels) {
    cv::Mat fullMask = cv::Mat::zeros(rows, cols, CV_8U);
    // Generate full-size binary mask
    for(int y = 0; y < rows; ++y) {
      for(int x = 0; x < cols; ++x) {
        int val = (labelImage.type() == CV_32S) ? labelImage.at<int>(y, x) : labelImage.at<uchar>(y, x);
        if(val == label) {
          fullMask.at<uchar>(y, x) = 255;
        }
      }
    }

    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(fullMask.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    if(contours.empty()) {
      continue;
    }

    // Use contour to get bounding box
    cv::Rect bbox = cv::boundingRect(contours[0]);
    for(size_t i = 1; i < contours.size(); ++i) {
      bbox |= cv::boundingRect(contours[i]);
    }

    // Crop mask and adjust contours
    cv::Mat croppedMask = fullMask(bbox).clone();

    // Shift contours relative to top-left of bbox
    std::vector<std::vector<cv::Point>> adjustedContours;
    int32_t maxContourIdx = 0;
    int32_t conoutrSize   = 0;
    int32_t idx           = 0;
    for(const auto &contour : contours) {
      std::vector<cv::Point> shifted;
      if(contour.size() > conoutrSize) {
        maxContourIdx = idx;
        conoutrSize   = contour.size();
      }
      for(const auto &pt : contour) {
        shifted.emplace_back(pt.x - bbox.x, pt.y - bbox.y);
      }
      adjustedContours.push_back(shifted);
      idx++;
    }

    result.push_back(AiModel::Result{.boundingBox = bbox, .mask = croppedMask, .contour = adjustedContours.at(maxContourIdx), .classId = 0});
  }

  return result;
}

///
/// \brief      Follow the flow field and returns a cv::Mat with each segmented object having a pixel value
/// \author     Joachim Danmayr
/// \return     Segmented mask, set of labels
///
std::pair<cv::Mat, std::set<int>> followFlowField(const cv::Mat &flowX, const cv::Mat &flowY, const cv::Mat &mask, float maskThreshold)
{
  // We have to transform to get a correct flow field
  cv::transpose(flowX, flowX);
  cv::transpose(flowY, flowY);
  cv::transpose(mask, mask);

  // Follow the flow starting at pixel (x0, y0)
  cv::Mat labels = cv::Mat::zeros(flowX.size(), CV_32S);    // int labels per pixel
  std::map<std::pair<int, int>, int> landingLabelMap;
  std::set<int> labelsSet;
  int currentLabel = 1;
  for(int y = 0; y < flowX.rows; ++y) {
    for(int x = 0; x < flowX.cols; ++x) {
      float pixelMask = mask.at<float>(y, x);
      if(pixelMask > maskThreshold) {
        float px = static_cast<float>(x);
        float py = static_cast<float>(y);

        cv::Point2f landing_pos = followFlow(flowX, flowY, px, py, 500, 2);    // your method modifies px, py to landing pos

        int lx = cvRound(landing_pos.x);
        int ly = cvRound(landing_pos.y);

        // Key for map
        auto key = std::make_pair(lx, ly);

        int label;
        auto it = landingLabelMap.find(key);
        if(it == landingLabelMap.end()) {
          label                = currentLabel++;
          landingLabelMap[key] = label;
        } else {
          label = it->second;
        }
        labelsSet.emplace(label);
        labels.at<int>(y, x) = label;
      }
    }
  }

  // transpose back
  cv::transpose(labels, labels);

  //
  // Cell pose flow field
  //
  {
      //   cv::Mat colorImage(mask.size(), CV_8UC3, cv::Scalar(0, 0, 0));    // Black image
      //   for(int x = 0; x < mask.cols; x++) {
      //     for(int y = 0; y < mask.rows; y++) {
      //       float pixelMask = mask.at<float>(y, x);
      //       if(pixelMask > 0.5) {
      //         float pixelValFlowX = flowX.at<float>(y, x);
      //         float pixelValFlowY = flowY.at<float>(y, x);
      //
      //         colorImage.at<cv::Vec3b>(y, x) = flowToColor(pixelValFlowX, pixelValFlowY);
      //       } else {
      //         colorImage.at<cv::Vec3b>(y, x) = {0, 0, 0};
      //       }
      //     }
      //   }
      //   cv::imwrite("tmp/flowField.jpg", colorImage);
  }

  //
  // Debugging
  //
  {
    // cv::Mat labelDebugImage;
    // cv::Mat binaryDebugImage;
    // clusterLandingPoints(landingLabelMap, labelDebugImage, binaryDebugImage);
    // cv::imwrite("tmp/labelDebugImage.jpg", labelDebugImage);
    // cv::imwrite("tmp/binaryDebugImage.jpg", binaryDebugImage);
    // cv::transpose(labels, labels);
    // cv::imwrite("tmp/labels.jpg", paintLabels(labels));

    //
    // FLow field arrows
    //
    // cv::Mat arrowImage;
    // drawFlowArrows(flowX, flowY, arrowImage, 10, 5.0);    // stride=10, scale flow x5 for visibility
    // for(const auto &[coor, a] : landingLabelMap) {
    //  cv::Point center(cvRound(coor.first), cvRound(coor.second));     // Convert to integer pixel position
    //  cv::circle(arrowImage, center, 2, cv::Scalar(0, 0, 255), -1);    // BGR = Red
    //}
    // cv::imwrite("tmp/arrows.jpg", arrowImage);
  }

  return {labels, labelsSet};
}

///
/// \brief      Follow flow field from (startX, startY) for numSteps with stepSize
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Point2f followFlow(const cv::Mat &flowX, const cv::Mat &flowY, float startX, float startY, int numSteps, float stepSize)
{
  float x = startX;
  float y = startY;

  int width  = flowX.cols;
  int height = flowX.rows;

  for(int i = 0; i < numSteps; ++i) {
    cv::Vec2f flow = bilinearInterpolate(flowX, flowY, x, y);
    float epsilon  = 0.0001;
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

  float dx = x - x0;
  float dy = y - y0;

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

      cv::Point2f start(x, y);
      cv::Point2f end(x + fx * scale, y + fy * scale);

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
  float hue = (angle + CV_PI) * 90.0f / CV_PI;

  // Normalize magnitude (optional: scale as needed)
  float mag_normalized = std::min(1.0f, magnitude / 10.0f);    // assuming max mag = 10

  // Create HSV color: H [0,180], S [0,255], V [0,255]
  cv::Mat hsv(1, 1, CV_8UC3, cv::Scalar(hue, 255, mag_normalized * 255));
  cv::Mat bgr;
  cv::cvtColor(hsv, bgr, cv::COLOR_HSV2BGR);

  return bgr.at<cv::Vec3b>(0, 0);
}

}    // namespace joda::ai
