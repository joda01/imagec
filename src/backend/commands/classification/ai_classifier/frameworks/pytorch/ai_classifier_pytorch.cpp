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
#if defined(WITH_PYTORCH)

#undef slots
#include "ai_classifier_pytorch.hpp"
#include <ATen/ops/upsample_nearest2d.h>
#include <c10/core/Device.h>
#include <c10/core/ScalarType.h>
#include <c10/core/TensorOptions.h>
#include <c10/util/ArrayRef.h>
#include <torch/csrc/jit/serialization/import.h>
#include <torch/cuda.h>
#include <torch/script.h>
#include <torch/torch.h>
#include <torch/types.h>
#include <stdexcept>
#include <string>
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#define slots Q_SLOTS

namespace joda::ai {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
AiFrameworkPytorch::AiFrameworkPytorch(const std::string &modelPath, const InputParameters &inputParameters) :
    mModelPath(modelPath), mSettings(inputParameters)
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto AiFrameworkPytorch::predict(const cv::Mat &originalImage) -> torch::Tensor
{
  // Check if CUDA is available
  bool cudaAvailable = torch::cuda::is_available();
  int numCudaDevices = torch::cuda::device_count();

  // ===============================
  // 0. Load model
  // ===============================
  torch::jit::script::Module model;
  try {
    model = torch::jit::load(mModelPath);
    model.eval();
    if(numCudaDevices > 0) {
      model.to(at::Device("cuda:0"));
    }
  } catch(const c10::Error &e) {
    throw std::runtime_error("Could not load torch model!");
  }

  // ===============================
  // 1. Prepare image
  // ===============================
  cv::Mat resizedImage = prepareImage(originalImage, mSettings);
  if(!resizedImage.isContinuous()) {
    resizedImage = resizedImage.clone();
  }

  // ===============================
  // 2. Run the Model Inference
  // ===============================
  auto inputTensor = torch::from_blob(resizedImage.data, toTensorOptions(), torch::kFloat32);
  if(!inputTensor.is_contiguous()) {
    inputTensor = inputTensor.to(torch::kFloat).clone();
  }

  // ===============================
  // 3. Run the Model Inference
  // ===============================
  torch::Tensor output = model.forward({inputTensor}).toTensor();
  return output;
}

///
/// \brief      Generate tensor options in correct order
/// \author     Joachim Danmayr
/// \return     Input vector for
///
auto AiFrameworkPytorch::toTensorOptions() const -> std::vector<int64_t>
{
  std::vector<int64_t> retArray(4);
  retArray.at(mSettings.getBatchIndex())   = mSettings.batchSize;
  retArray.at(mSettings.getChannelIndex()) = mSettings.nrOfChannels;
  retArray.at(mSettings.getHeightIndex())  = mSettings.inputHeight;
  retArray.at(mSettings.getWidthIndex())   = mSettings.inputWidth;
  return retArray;
}

}    // namespace joda::ai
#endif
