///
/// \file      ai_model_bioimage.cpp
/// \author    Joachim Danmayr
/// \date      2025-02-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "ai_model_bioimage.hpp"

namespace joda::ai {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto AiModelBioImage::processPrediction(const cv::Mat &inputImage, const torch::Tensor &tensor) -> std::vector<Result>
{
  static const int CHANNEL_MASK    = 0;
  static const int CHANNEL_CONTOUR = 1;
  static float PROBABILITY         = 0.5;

  int32_t originalHeight = inputImage.rows;
  int32_t originalWith   = inputImage.cols;

  auto classProbabilities = torch::softmax(tensor, 1);    // Along the class dimension

  // ===============================
  // 0. Rescale to original image
  // ===============================
  at::Tensor tensorSmall               = tensor.to(at::kCPU).to(at::kFloat);
  at::Tensor classProbabilitiesSmall   = classProbabilities.to(at::kCPU).to(at::kFloat);
  torch::Tensor tensorCpu              = torch::upsample_bilinear2d(tensorSmall, {originalHeight, originalWith}, false);
  torch::Tensor tensorProbabilitiesCpu = torch::upsample_nearest2d(classProbabilitiesSmall, {originalHeight, originalWith});

  at::Tensor mask_tensor = tensorCpu[0][CHANNEL_MASK].clone().contiguous();    // [height, width]

  // Convert to OpenCV Mat

  // ===============================
  // 1. Convert mask to cv::mat
  // ===============================
  cv::Mat mask(originalHeight, originalWith, CV_32F, mask_tensor.data_ptr<float>());

  // ===============================
  // 2.  Apply a threshold to create a binary mask for the object
  // ===============================
  cv::Mat binary_mask;
  cv::threshold(mask, binary_mask, PROBABILITY, 1.0, cv::THRESH_BINARY);
  binary_mask.convertTo(binary_mask, CV_8U, 255);

  //
  // ===============================
  // 3. Extract each individual object by finding connected components
  // ===============================
  std::vector<cv::Mat> contours;
  std::vector<std::vector<cv::Point>> contours_poly;
  cv::findContours(binary_mask, contours_poly, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  std::vector<Result> results;
  for(size_t j = 0; j < contours_poly.size(); ++j) {
    // Create an empty mask for this object
    cv::Mat object_mask = cv::Mat::zeros(binary_mask.size(), CV_8U);

    // Draw the contour for this object on the empty mask
    cv::drawContours(object_mask, contours_poly, (int) j, cv::Scalar(255), cv::FILLED);

    // Get the class probabilities for this object (we take the maximum probability over the mask)
    cv::Mat object_probs = cv::Mat::zeros(binary_mask.size(), CV_32F);

    // Loop through the mask and find the max class probability
    float max_prob      = 0.0f;
    int predicted_class = -1;

    for(int y = 0; y < originalHeight; ++y) {
      for(int x = 0; x < originalWith; ++x) {
        if(object_mask.at<uchar>(y, x) == 255) {
          // Get the class probabilities at this pixel (for all classes)
          for(int c = 0; c < tensorProbabilitiesCpu.size(1); ++c) {
            float prob = tensorProbabilitiesCpu[0][c][y][x].item<float>();
            if(prob > max_prob) {
              max_prob        = prob;
              predicted_class = c;
            }
          }
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

}    // namespace joda::ai
