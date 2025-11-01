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
void AnnMlpPyTorch::predict(const std::filesystem::path &path, const cv::Mat &image, const cv::Mat &features, cv::Mat &prediction,
                            const std::filesystem::path &modelStoragePath)
{
  // Load trained model
  torch::nn::Sequential mModel;
  torch::load(mModel, modelStoragePath.string());

  // Convert features to float tensor
  cv::Mat temp;
  if(features.type() != CV_32F) {
    features.convertTo(temp, CV_32F);
  } else {
    temp = features;
  }

  // Convert to [num_samples, num_features] tensor
  auto options       = torch::TensorOptions().dtype(torch::kFloat32);
  torch::Tensor data = torch::from_blob(temp.data, {features.rows, features.cols}, options).clone();

  // Move to device (match model device)
  torch::Device device(torch::kCPU);
  if(torch::cuda::is_available()) {
    device = torch::Device(torch::kCUDA);
  }
  data = data.to(device);
  mModel->to(device);
  mModel->eval();

  // Forward pass
  torch::NoGradGuard no_grad;
  auto output = mModel->forward(data);

  // Get predicted class indices (argmax)
  auto predictions = std::get<1>(output.max(1));    // [num_samples]
  predictions      = predictions.to(torch::kCPU);

  // Convert to cv::Mat
  prediction.create(predictions.size(0), 1, CV_32S);
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
                          const std::filesystem::path &modelStoragePath)
{
  // Convert cv::Mat to float tensor
  cv::Mat samplesFloat;
  if(trainSamples.type() != CV_32F) {
    trainSamples.convertTo(samplesFloat, CV_32F);
  } else {
    samplesFloat = trainSamples;
  }

  // Convert to torch tensor [num_samples, num_features]
  auto options       = torch::TensorOptions().dtype(torch::kFloat32);
  torch::Tensor data = torch::from_blob(samplesFloat.data, {trainSamples.rows, trainSamples.cols}, options).clone();

  // Convert labels to tensor [num_samples]
  torch::Tensor labels = torch::empty({trainLabels.rows}, torch::kInt64);
  for(int i = 0; i < trainLabels.rows; ++i) {
    labels[i] = trainLabels.at<int>(i, 0);
  }

  // Define model (simple MLP)
  torch::nn::Sequential model;
  int inputSize = trainSamples.cols;

  int prevSize = inputSize;
  for(int neurons : mSettings.neuronsLayer) {
    if(neurons > 0) {
      model->push_back(torch::nn::Linear(prevSize, neurons));
      model->push_back(torch::nn::ReLU());
      prevSize = neurons;
    }
  }

  // Output layer
  model->push_back(torch::nn::Linear(prevSize, nrOfClasses));

  // Move to device
  torch::Device device(torch::kCPU);
  if(torch::cuda::is_available()) {
    device = torch::Device(torch::kCUDA);
  }
  model->to(device);

  data   = data.to(device);
  labels = labels.to(device);

  // Optimizer (SGD)
  int64_t batchSize = (mSettings.batchSize > 0) ? mSettings.batchSize : trainSamples.rows;
  torch::optim::SGD optimizer(model->parameters(), torch::optim::SGDOptions(mSettings.learningRate).momentum(0.9));

  // Training loop
  const int64_t numEpochs = mSettings.maxIterations;
  model->train();

  for(int64_t epoch = 0; epoch < numEpochs; ++epoch) {
    float epochLoss = 0.0f;

    for(int64_t i = 0; i < data.size(0); i += batchSize) {
      auto end = std::min(i + batchSize, data.size(0));
      // The slice operations here are view operations (efficient):
      auto inputs  = data.slice(0, i, end);
      auto targets = labels.slice(0, i, end);

      // Use zero_grad() to clear previous gradients
      optimizer.zero_grad();

      // Forward pass
      auto outputs = model->forward(inputs);

      // Calculate loss
      // Ensure functional cross_entropy is being used efficiently by the backend
      auto loss = torch::nn::functional::cross_entropy(outputs, targets);

      // Backward pass (gradient calculation)
      loss.backward();

      // Optimizer step (parameter update)
      optimizer.step();

      // Accumulate loss (use item() after the step)
      epochLoss += loss.item<float>();
    }

    if(epoch % 10 == 0)
      std::cout << "Epoch [" << epoch << "/" << numEpochs << "], Loss: " << epochLoss << std::endl;
    if(epochLoss < mSettings.terminationEpsilon) {
      break;
    }
  }

  torch::save(model, modelStoragePath.string());
}

}    // namespace joda::ml
