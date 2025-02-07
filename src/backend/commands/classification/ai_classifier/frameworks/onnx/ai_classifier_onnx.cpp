///
/// \file      ai_lassifier_onnx.cpp
/// \author    Joachim Danmayr
/// \date      2025-01-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "ai_classifier_onnx.hpp"
#include <onnxruntime_cxx_api.h>
#include <torch/types.h>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "backend/helper/uuid.hpp"
#include <opencv2/opencv.hpp>

namespace joda::ai {

AiFrameworkOnnx::AiFrameworkOnnx(const std::string &modelPath, const InputParameters &inputParameters) :
    mModelPath(modelPath), mSettings(inputParameters)
{
}

std::vector<float> convertToTensor(const cv::Mat &img)
{
  std::vector<float> tensor_data;
  int channels = img.channels();
  int height   = img.rows;
  int width    = img.cols;

  // Rearrange HWC to CHW
  for(int c = 0; c < channels; ++c) {
    for(int h = 0; h < height; ++h) {
      for(int w = 0; w < width; ++w) {
        tensor_data.push_back(img.at<cv::Vec3f>(h, w)[c]);
      }
    }
  }

  return tensor_data;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto AiFrameworkOnnx::predict(const cv::Mat &originalImage) -> torch::Tensor
{
  if(originalImage.empty()) {
    throw std::runtime_error("Failed to load image!");
  }

  // Initialize ONNX Runtime
  Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "Segmentation");
  Ort::SessionOptions session_options;
  Ort::Session session(env, mModelPath.c_str(), session_options);

  Ort::AllocatorWithDefaultOptions allocator;
  std::vector<Ort::AllocatedStringPtr> allocateHolder;

  std::vector<const char *> inputNames;
  size_t inputCount = session.GetInputCount();
  for(size_t n = 0; n < inputCount; n++) {
    auto i = session.GetInputNameAllocated(n, allocator);
    inputNames.emplace_back(i.get());
    allocateHolder.push_back(std::move(i));    // Move to avoid premature deallocation
  }

  std::vector<const char *> outputNames;
  size_t outputCount = session.GetOutputCount();
  for(size_t n = 0; n < outputCount; n++) {
    auto i = session.GetOutputNameAllocated(n, allocator);
    outputNames.emplace_back(i.get());
    allocateHolder.push_back(std::move(i));    // Move to avoid premature deallocation
  }

  // Prepare image
  cv::Mat blob = prepareImage(originalImage, mSettings);

  std::array<int64_t, 4> inputDims = {mSettings.batchSize, mSettings.nrOfChannels, mSettings.inputWidth, mSettings.inputHeight};

  // Create input tensor
  Ort::MemoryInfo memoryInfo     = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeDefault);
  std::vector<float> inputTensor = convertToTensor(blob);
  Ort::Value inputTensorOnnx =
      Ort::Value::CreateTensor<float>(memoryInfo, inputTensor.data(), inputTensor.size(), inputDims.data(), inputDims.size());

  auto outputTensors = session.Run(Ort::RunOptions{}, inputNames.data(), &inputTensorOnnx, inputCount, outputNames.data(), outputCount);

  // Convert output to torch::Tensor
  float *outputData = outputTensors.front().GetTensorMutableData<float>();
  std::vector<int64_t> outputDims(outputTensors.front().GetTensorTypeAndShapeInfo().GetShape());
  return at::from_blob(outputData, at::IntArrayRef(outputDims), torch::kFloat32).clone();
}

}    // namespace joda::ai
