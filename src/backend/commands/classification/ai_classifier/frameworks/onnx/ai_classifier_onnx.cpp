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
#include <ATen/ops/from_blob.h>
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
auto AiFrameworkOnnx::predict(const cv::Mat &originalImage) -> at::IValue
{
  if(originalImage.empty()) {
    throw std::runtime_error("Failed to load image!");
  }

  // Initialize ONNX Runtime
  Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "Segmentation");
  Ort::SessionOptions session_options;
#ifdef _WIN32
  std::wstring widestr = std::wstring(mModelPath.begin(), mModelPath.end());
  Ort::Session session(env, widestr.c_str(), session_options);
#else
  Ort::Session session(env, mModelPath.c_str(), session_options);
#endif

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

  // ===============================
  // 1. Prepare image
  // ===============================
  cv::Mat blob = prepareImage(originalImage, mSettings, cv::COLOR_GRAY2BGR);

  std::array<int64_t, 4> inputDims = {mSettings.batchSize, mSettings.nrOfChannels, mSettings.inputWidth, mSettings.inputHeight};

  // ===============================
  // 2. Prepare input tensor
  // ===============================
  Ort::MemoryInfo memoryInfo     = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeDefault);
  std::vector<float> inputTensor = convertToTensor(blob);
  Ort::Value inputTensorOnnx =
      Ort::Value::CreateTensor<float>(memoryInfo, inputTensor.data(), inputTensor.size(), inputDims.data(), inputDims.size());

  // ===============================
  // 3. Run the Model Inference
  // ===============================
  std::vector<Ort::Value> outputTensors =
      session.Run(Ort::RunOptions{}, inputNames.data(), &inputTensorOnnx, inputCount, outputNames.data(), outputCount);

  // ----------------------------
  // Convert to torch tensor
  // ----------------------------

  // Create a vector to hold the converted torch::Tensor objects.
  std::vector<torch::Tensor> torchTensors;

  // Iterate over each Ort::Value and convert it to a torch::Tensor.
  for(auto &ortValue : outputTensors) {
    // Get the shape of the tensor as a vector of int64_t.
    Ort::TensorTypeAndShapeInfo tensorInfo = ortValue.GetTensorTypeAndShapeInfo();
    std::vector<int64_t> tensor_shape      = tensorInfo.GetShape();

    // Retrieve the pointer to the raw data.
    // In this example, we assume that the data type is float.
    auto *dataPtr = ortValue.GetTensorMutableData<float>();

    // Specify the tensor options (data type, device, etc.).
    auto options = torch::TensorOptions().dtype(torch::kFloat32);

    // Create a torch tensor from the raw data.
    // Note: torch::from_blob does not copy the data by default, so we call .clone()
    // to ensure that the tensor owns its memory.
    torch::Tensor tensor = torch::from_blob(dataPtr, tensor_shape, options).clone();

    // Append the tensor to our vector.
    torchTensors.push_back(tensor);
  }

  // First, convert the vector of tensors into a vector of IValues.
  std::vector<torch::IValue> tuple_elems;
  tuple_elems.reserve(torchTensors.size());
  for(const auto &t : torchTensors) {
    tuple_elems.emplace_back(t);
  }

  // Create the tuple from the vector of IValues.
  auto tensorTuple = c10::ivalue::Tuple::create(tuple_elems);
  return tensorTuple;
}

}    // namespace joda::ai
