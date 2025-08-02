///
/// \file      ai_classifier_pytorch.cpp
/// \author    Joachim Danmayr
/// \date      2025-01-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///
#include <vector>
#include "backend/helper/duration_count/duration_count.h"
#if defined(WITH_PYTORCH)

#undef slots
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
#include "ai_classifier_pytorch.hpp"
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
auto AiFrameworkPytorch::predict(const cv::Mat &originalImage) -> at::IValue
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
      joda::log::logDebug("Using GPU: cuda:0");
      model.to(at::Device("cuda:0"));
    } else {
      joda::log::logDebug("Using CPU.");
    }
  } catch(const c10::Error &e) {
    throw std::runtime_error(e.what());
  }

  // ===============================
  // 1. Prepare image
  // ===============================
  cv::Mat resizedImage = prepareImage(originalImage, mSettings, cv::COLOR_GRAY2RGB);
  if(!resizedImage.isContinuous()) {
    resizedImage = resizedImage.clone();
  }

  // ===============================
  // 2. Prepare input tensor
  // ===============================
  auto inputTensor = torch::from_blob(resizedImage.data, {resizedImage.rows, resizedImage.cols, resizedImage.channels()}, torch::kFloat32);
  inputTensor      = inputTensor.permute({2, 0, 1}).to(torch::kFloat32);    // Now shape is (C, H, W)
  inputTensor      = inputTensor.unsqueeze(0);                              // Now shape is (B, C, H, W)
  inputTensor = inputTensor.permute({mSettings.getBatchIndex(), mSettings.getChannelIndex(), mSettings.getHeightIndex(), mSettings.getWidthIndex()})
                    .to(torch::kFloat32);

  if(!inputTensor.is_contiguous()) {
    inputTensor = inputTensor.to(torch::kFloat).clone();
  }

  auto idx = DurationCount::start("Forward to libtorch");

  if(numCudaDevices > 0) {
    inputTensor = inputTensor.to(at::Device("cuda:0"));
  }

  // ===============================
  // 3. Run the Model Inference
  // ===============================
  at::IValue output = model.forward({inputTensor});

  DurationCount::stop(idx);

  return output;
}

}    // namespace joda::ai
#endif
