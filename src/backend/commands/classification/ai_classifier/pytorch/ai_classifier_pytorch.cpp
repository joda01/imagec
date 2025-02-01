///
/// \file      ai_classifier_pytorch.cpp
/// \author    Joachim Danmayr
/// \date      2025-01-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include <ATen/ops/upsample_nearest2d.h>
#if defined(WITH_TENSORFLOW)

#include <c10/core/ScalarType.h>
#include <c10/core/TensorOptions.h>
#include <torch/csrc/jit/serialization/import.h>
#include <torch/script.h>    // One-stop header.
#include <torch/types.h>

#include <stdexcept>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "ai_classifier_pytorch.hpp"

namespace joda::ai {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
AiClassifierPyTorch::AiClassifierPyTorch(const settings::AiClassifierSettings &settings) : mSettings(settings)
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto AiClassifierPyTorch::getClasses(const std::filesystem::path &modelPath) -> std::vector<std::string>
{
  // Load the TorchScript model
  try {
    int32_t INPUT_SIZE = 256;
    auto model         = torch::jit::load(modelPath.string());
    // Create a dummy input with the expected input shape
    torch::Tensor dummy_input = torch::randn({1, 1, INPUT_SIZE, INPUT_SIZE});    // Adjust shape as needed
    auto output               = model.forward({dummy_input}).toTensor();
    int num_classes           = output.size(1);    // Assuming the output is of shape [batch_size, num_classes]

    std::cout << "Number of output classes: " << num_classes << std::endl;
  } catch(const c10::Error &e) {
    throw std::runtime_error("Could not load torch model!");
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto AiClassifierPyTorch::execute(const cv::Mat &originalImage) -> std::vector<Result>
{
  int32_t INPUT_SIZE   = 256;
  int32_t CHANNEL_SIZE = 1;

  // Load the TorchScript model
  torch::jit::script::Module model;
  try {
    model = torch::jit::load(mSettings.modelPath);
  } catch(const c10::Error &e) {
    throw std::runtime_error("Could not load torch model!");
  }

  const cv::Mat &inputImageOriginal = originalImage;
  // Normalize the pixel values to [0, 1] float for detection
  cv::Mat inputImage;
  inputImageOriginal.convertTo(inputImage, CV_32F, 1.0 / 65535.0);

  // Resize
  cv::Mat resized_image;
  cv::resize(inputImage, resized_image, cv::Size(mSettings.netInputWidth, mSettings.netInputHeight));    // Adjust size to your model's input

  // Create a tensor from the image data
  auto input_tensor = torch::from_blob(resized_image.data, {1, 1, resized_image.rows, resized_image.cols}, torch::kFloat32);

  // Clone to ensure the tensor is contiguous
  input_tensor = input_tensor.to(torch::kFloat).clone();

  // Step 3: Perform inference
  auto output = model.forward({input_tensor}).toTensor();

  // Step 4: Apply softmax to get probabilities
  torch::Tensor probabilities = torch::softmax(output, 1);    // Along the class dimension

  // int num_objects = output.size(1);    // Number of objects (channels)

  return tensorToObjectMasks(output, probabilities, inputImage.cols, inputImage.rows);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto AiClassifierPyTorch::tensorToObjectMasks(const at::Tensor &tensor, const at::Tensor &class_probabilities, int originalWith, int originalHeight)
    -> std::vector<Result>
{
  // Assuming tensor shape is [batch_size, channels, height, width]
  int channels  = tensor.size(1);
  int heightNet = tensor.size(2);
  int widthNet  = tensor.size(3);

  // Ensure the tensor is in CPU and float32 for processing
  at::Tensor tensor_cpuSmall      = tensor.to(at::kCPU).to(at::kFloat);
  at::Tensor class_probs_cpuSmall = class_probabilities.to(at::kCPU).to(at::kFloat);

  torch::Tensor tensor_cpu      = torch::upsample_bilinear2d(tensor_cpuSmall, {originalHeight, originalWith}, false);
  torch::Tensor class_probs_cpu = torch::upsample_nearest2d(class_probs_cpuSmall, {originalHeight, originalWith});

  // Rescale to the original image size

  std::vector<Result> object_masks;    // Vector to store the individual object masks

  // Loop through each channel (object)
  // Extract the mask for the object (channel corresponds to the object index)
  static const int CHANNEL_MASK    = 0;
  static const int CHANNEL_CONTOUR = 1;

  at::Tensor mask_tensor = tensor_cpu[0][CHANNEL_MASK];    // [height, width]

  // Convert to OpenCV Mat
  cv::Mat mask(originalHeight, originalWith, CV_32F, mask_tensor.data_ptr<float>());

  // Apply a threshold to create a binary mask for the object
  cv::Mat binary_mask;
  cv::threshold(mask, binary_mask, 0.5, 1.0, cv::THRESH_BINARY);

  // Convert the binary mask to 8-bit (if you need)
  binary_mask.convertTo(binary_mask, CV_8U, 255);

  // Now, extract each individual object by finding connected components
  std::vector<cv::Mat> contours;
  std::vector<std::vector<cv::Point>> contours_poly;
  cv::findContours(binary_mask, contours_poly, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  // For each detected object, create a separate mask
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
          for(int c = 0; c < class_probs_cpu.size(1); ++c) {
            float prob = class_probs_cpu[0][c][y][x].item<float>();
            if(prob > max_prob) {
              max_prob        = prob;
              predicted_class = c;
            }
          }
        }
      }
    }

    /// \todo resize to origingl image

    cv::Rect fittedBoundingBox = cv::boundingRect(contours_poly[j]);
    //
    // Fit the bounding box and mask to the new size
    //
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
    object_masks.push_back(AiSegmentation::Result{.boundingBox = fittedBoundingBox,
                                                  .mask        = std::move(shiftedMask),
                                                  .contour     = std::move(contour),
                                                  .classId     = predicted_class,
                                                  .probability = max_prob});
  }

  return object_masks;    // Return the vector of individual object masks
}

}    // namespace joda::ai
#endif
