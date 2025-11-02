///
/// \file      random_forest.hpp
/// \author    Joachim Danmayr
/// \date      2025-10-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "ann_mlp_pytorch.hpp"
#include <torch/serialize.h>

#undef slots
#include <torch/torch.h>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "models/ann_mlp_model.hpp"
#include <nlohmann/json_fwd.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>

namespace joda::ml {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void AnnMlpPyTorch::predict(const std::filesystem::path &path, const cv::Mat &image, cv::Mat &prediction)
{
  // ============================================
  // Load model together with model settings
  // ============================================
  MLPModel model{};
  torch::load(model, path.string());
  MachineLearningSettings settings = model->getMeta();

  // ============================================
  // Extract features based on model settings
  // ============================================
  const cv::Mat features = extractFeatures(image, settings.featureExtractionPipelines, true);

  // ============================================
  // Move features to tensor
  // ============================================
  cv::Mat temp;
  if(features.type() != CV_32F) {
    features.convertTo(temp, CV_32F);
  } else {
    temp = features;
  }
  auto options       = torch::TensorOptions().dtype(torch::kFloat32);
  torch::Tensor data = torch::from_blob(temp.data, {features.rows, features.cols}, options).clone();

  // ============================================
  // Move tensor to device
  // ============================================
  torch::Device device(torch::kCPU);
  if(torch::cuda::is_available()) {
    device = torch::Device(torch::kCUDA);
  }
  data = data.to(device);
  model->to(device);
  model->eval();

  // ============================================
  // Inference
  // ============================================
  torch::NoGradGuard no_grad;
  auto output = model->forward(data);

  // ============================================
  // Convert prediction to cv::mat
  // ============================================
  auto predictions = std::get<1>(output.max(1));
  predictions      = predictions.to(torch::kCPU);
  prediction.create(static_cast<int32_t>(predictions.size(0)), 1, CV_32S);
  auto predAccessor = predictions.accessor<int64_t, 1>();
  for(int i = 0; i < predictions.size(0); ++i) {
    prediction.at<int>(i, 0) = static_cast<int>(predAccessor[i]);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void AnnMlpPyTorch::train(const ::cv::Mat &trainSamples, const ::cv::Mat &trainLabels, int32_t nrOfClasses,
                          const std::filesystem::path &modelStoragePath, const MachineLearningSettings &settings)
{
  // ============================================
  // Convert cv::Mat to float tensor
  // ============================================
  cv::Mat samplesFloat;
  if(trainSamples.type() != CV_32F) {
    trainSamples.convertTo(samplesFloat, CV_32F);
  } else {
    samplesFloat = trainSamples;
  }
  auto options       = torch::TensorOptions().dtype(torch::kFloat32);
  torch::Tensor data = torch::from_blob(samplesFloat.data, {trainSamples.rows, trainSamples.cols}, options).clone();

  // ============================================
  // Convert labels to tensor [num_samples]
  // ============================================
  torch::Tensor labels = torch::empty({trainLabels.rows}, torch::kInt64);
  for(int i = 0; i < trainLabels.rows; ++i) {
    labels[i] = trainLabels.at<int>(i, 0);
  }

  // Move to device
  torch::Device device(torch::kCPU);
  if(torch::cuda::is_available()) {
    device = torch::Device(torch::kCUDA);
  }

  // ============================================
  // Create model and move model and data to device
  // ============================================
  auto model = MLPModel(trainSamples.cols, mSettings.neuronsLayer, nrOfClasses, settings);
  model->to(device);

  data   = data.to(device);
  labels = labels.to(device);

  // ============================================
  // Optimizer (SGD)
  // ============================================
  int64_t batchSize = (mSettings.batchSize > 0) ? mSettings.batchSize : trainSamples.rows;
  torch::optim::SGD optimizer(model->parameters(), torch::optim::SGDOptions(mSettings.learningRate).momentum(0.9));

  // ============================================
  // Training loop
  // ============================================
  const int64_t numEpochs = mSettings.maxIterations;
  model->train();

  for(int64_t epoch = 0; epoch < numEpochs; ++epoch) {
    float epochLoss = 0.0F;

    for(int64_t i = 0; i < data.size(0); i += batchSize) {
      auto end     = std::min(i + batchSize, data.size(0));
      auto inputs  = data.slice(0, i, end);
      auto targets = labels.slice(0, i, end);

      optimizer.zero_grad();                    // Use zero_grad() to clear previous gradients
      auto outputs = model->forward(inputs);    // Forward pass
      auto loss    = torch::nn::functional::cross_entropy(
          outputs, targets);           // Calculate loss, Ensure functional cross_entropy is being used efficiently by the backend
      loss.backward();                    // Backward pass (gradient calculation)
      optimizer.step();                   // Optimizer step (parameter update)
      epochLoss += loss.item<float>();    // Accumulate loss (use item() after the step)
    }

    if(epoch % 10 == 0) {
      mLastEpoch = "Epoch [" + std::to_string(epoch) + "/" + std::to_string(numEpochs) + "], Loss: " + std::to_string(epochLoss);
      std::cout << mLastEpoch << std::endl;
    }
    if(static_cast<double>(epochLoss) < mSettings.terminationEpsilon || mStopped) {
      break;
    }
  }

  // ============================================
  // Save model and meta
  // ============================================
  torch::save(model, modelStoragePath.string());
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void AnnMlpPyTorch::stopTraining()
{
  mStopped = true;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto AnnMlpPyTorch::getTrainingProgress() -> std::string
{
  return mLastEpoch;
}

}    // namespace joda::ml
