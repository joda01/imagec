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

#include <string>
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

cv::Mat followFlowField(const cv::Mat &flowX, const cv::Mat &flowY, const cv::Mat &masl);

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

  {
    followFlowField(flowXImage, flowYImage, maskImage);
    // cv::Mat normMat;
    // cv::normalize(maskImage, normMat, 0.0, 1.0, cv::NORM_MINMAX);
    auto success = cv::imwrite("tmp/mask.png", maskImage * 128);

    success = cv::imwrite("tmp/flowXImage.png", flowXImage * 128);
    success = cv::imwrite("tmp/flowYImage.png", flowYImage * 128);
  }

  // ===============================
  // 2.  Apply a threshold to create a binary mask for the object
  // ===============================
  cv::Mat binaryMask;
  cv::threshold(maskImage, binaryMask, mSettings.maskThreshold, 1.0, cv::THRESH_BINARY);
  binaryMask.convertTo(binaryMask, CV_8U, 255);

  //
  // ===============================
  // 3. Extract each individual object by finding connected components
  // ===============================
  std::vector<cv::Mat> contours;
  std::vector<std::vector<cv::Point>> contours_poly;
  cv::findContours(binaryMask, contours_poly, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  std::vector<Result> results;
  for(size_t j = 0; j < contours_poly.size(); ++j) {
    // Create an empty mask for this object
    cv::Mat object_mask = cv::Mat::zeros(binaryMask.size(), CV_8U);

    // Draw the contour for this object on the empty mask
    cv::drawContours(object_mask, contours_poly, (int) j, cv::Scalar(255), cv::FILLED);

    // Get the class probabilities for this object (we take the maximum probability over the mask)
    cv::Mat object_probs = cv::Mat::zeros(binaryMask.size(), CV_32F);

    // Loop through the mask and find the max class probability
    float max_prob      = 0.0f;
    int predicted_class = 0;

    for(int y = 0; y < originalHeight; ++y) {
      for(int x = 0; x < originalWith; ++x) {
        if(object_mask.at<uchar>(y, x) == 255) {
          // Get the class probabilities at this pixel (for all classes)
          // for(int c = 0; c < tensorProbabilitiesCpu.size(1); ++c) {
          //   float prob = tensorProbabilitiesCpu[0][c][y][x].item<float>();
          //   if(prob > max_prob) {
          //     max_prob        = prob;
          //     predicted_class = c;
          //   }
          // }
        }
      }
    }

    cv::Rect fittedBoundingBox = cv::boundingRect(contours_poly[j]);
    // Fit the bounding box and mask to the new size
    cv::Mat shiftedMask = cv::Mat::zeros(fittedBoundingBox.size(), CV_8UC1);
    shiftedMask         = object_mask(fittedBoundingBox).clone();

    // Move the contour points
    int32_t xOffset = fittedBoundingBox.x;
    int32_t yOffset = fittedBoundingBox.y;
    auto contour    = contours_poly[j];
    for(auto &point : contour) {
      point.x = point.x - xOffset;
      if(point.x < 0) {
        point.x = 0;
      }
      point.y = point.y - yOffset;
      if(point.y < 0) {
        point.y = 0;
      }
    }

    // Add the individual object mask to the vector
    results.push_back(Result{.boundingBox = fittedBoundingBox,
                             .mask        = std::move(shiftedMask),
                             .contour     = std::move(contour),
                             .classId     = predicted_class,
                             .probability = max_prob});
  }

  return results;    // Return the vector of individual object masks
}

cv::Vec2f bilinearInterpolate(const cv::Mat &flowX, const cv::Mat &flowY, float x, float y);
cv::Point2f followFlow(const cv::Mat &flowX, const cv::Mat &flowY, float startX, float startY, int numSteps = 10, float stepSize = 1.0f);

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

void drawFlowArrows(const cv::Mat &flowX, const cv::Mat &flowY, cv::Mat &outImage, int stride = 10, float scale = 1.0,
                    cv::Scalar color = cv::Scalar(0, 255, 0))
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

cv::Mat followFlowField(const cv::Mat &flowX, const cv::Mat &flowY, const cv::Mat &mask)
{
  cv::Mat colorImage(mask.size(), CV_8UC3, cv::Scalar(0, 0, 0));    // Black image

  for(int x = 0; x < mask.cols; x++) {
    for(int y = 0; y < mask.rows; y++) {
      float pixelMask = mask.at<float>(y, x);
      if(pixelMask > 0.5) {
        float pixelValFlowX = flowX.at<float>(y, x);
        float pixelValFlowY = flowY.at<float>(y, x);

        colorImage.at<cv::Vec3b>(y, x) = flowToColor(pixelValFlowX, pixelValFlowY);
      } else {
        colorImage.at<cv::Vec3b>(y, x) = {0, 0, 0};
      }
    }
  }

  cv::imwrite("tmp/flowField.jpg", colorImage);

  cv::transpose(flowX, flowX);
  cv::transpose(flowY, flowY);

  cv::Mat arrowImage;
  drawFlowArrows(flowX, flowY, arrowImage, 10, 5.0);    // stride=10, scale flow x5 for visibility

  // Follow the flow starting at pixel (x0, y0)
  float x0                = 820.0f;
  float y0                = 1543.0f;
  cv::Point2f landing_pos = followFlow(flowX, flowY, x0, y0, 250, 2.0f);
  std::cout << "Pixel lands at: " << std::to_string(landing_pos.x) << " " << std::to_string(landing_pos.y) << std::endl;

  cv::Point center(cvRound(landing_pos.x), cvRound(landing_pos.y));    // Convert to integer pixel position
  cv::circle(arrowImage, center, 2, cv::Scalar(0, 0, 255), -1);        // BGR = Red

  cv::imwrite("tmp/arrows.jpg", arrowImage);

  return {};
}

// Follow flow field from (startX, startY) for numSteps with stepSize
cv::Point2f followFlow(const cv::Mat &flowX, const cv::Mat &flowY, float startX, float startY, int numSteps, float stepSize)
{
  float x = startX;
  float y = startY;

  int width  = flowX.cols;
  int height = flowX.rows;

  for(int i = 0; i < numSteps; ++i) {
    cv::Vec2f flow = bilinearInterpolate(flowX, flowY, x, y);
    float ε        = 0.0001;
    if(std::abs(flow[0]) < ε && std::abs(flow[1]) < ε) {
      break;    // converged
    }
    x += stepSize * flow[0];
    y += stepSize * flow[1];

    // Clamp position inside image bounds
    x = std::clamp(x, 0.0f, static_cast<float>(width - 1));
    y = std::clamp(y, 0.0f, static_cast<float>(height - 1));
  }

  return cv::Point2f(x, y);
}

///
/// Bilinear interpolation of flow vectors at floating point (x, y)
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

  return cv::Vec2f(flow_x, flow_y);
}

}    // namespace joda::ai
