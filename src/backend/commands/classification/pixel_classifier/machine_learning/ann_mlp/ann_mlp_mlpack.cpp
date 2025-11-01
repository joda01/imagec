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

#include "ann_mlp_mlpack.hpp"
#include <cstddef>
#include <fstream>
#include <string>
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <nlohmann/json_fwd.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>

#include <mlpack/core.hpp>
#include <mlpack/methods/ann/ffn.hpp>
#include <mlpack/methods/ann/layer/layer.hpp>

namespace joda::ml {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void AnnMlpMlPack::predict(const std::filesystem::path &path, const cv::Mat &image, const cv::Mat &features, cv::Mat &prediction,
                           const std::filesystem::path &modelStoragePath)
{
  mlpack::FFN<mlpack::NegativeLogLikelihood, mlpack::RandomInitialization> mModel;

  mlpack::data::Load(modelStoragePath.string(), "model", mModel, true, mlpack::data::format::json);

  //
  // Convert to Armadillo matrices
  //
  cv::Mat temp;
  if(features.type() != CV_64F) {
    features.convertTo(temp, CV_64F);
  } else {
    temp = features;
  }
  arma::mat data(reinterpret_cast<double *>(temp.data), temp.cols, temp.rows, false, true);

  auto idx = DurationCount::start("Predict");
  arma::mat predictionTemp;
  mModel.Predict(data, predictionTemp);
  DurationCount::stop(idx);

  data.save("data_fe.txt", arma::arma_ascii);
  predictionTemp.save("predictionTemp.txt", arma::arma_ascii);

  // Find index of max prediction for each data point and store in "predictionInt"
  prediction.create(static_cast<int>(data.n_cols), 1, CV_32S);
  for(size_t i = 0; i < data.n_cols; ++i) {
    prediction.at<int>(static_cast<int>(i), 0) = arma::as_scalar(arma::find(arma::max(predictionTemp.col(i)) == predictionTemp.col(i), 1));
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void AnnMlpMlPack::train(const ::cv::Mat &trainSamples, const ::cv::Mat &trainLabels, int32_t nrOfClasses,
                         const std::filesystem::path &modelStoragePath)
{
  mlpack::FFN<mlpack::NegativeLogLikelihood, mlpack::RandomInitialization> mModel;

  //
  // Convert to Armadillo matrices
  //
  cv::Mat temp;
  if(trainSamples.type() != CV_64F) {
    trainSamples.convertTo(temp, CV_64F);
  } else {
    temp = trainSamples;
  }
  arma::mat data(reinterpret_cast<double *>(temp.data), temp.cols, temp.rows, false, true);

  //
  // Convert labels to arma::Row<size_t>
  //
  arma::mat labels = arma::zeros<arma::mat>(1, trainLabels.rows);
  for(int i = 0; i < trainLabels.rows; ++i) {
    labels.col(i) = static_cast<double>(trainLabels.at<int>(i, 0));
  }

  data.save("data.txt", arma::arma_ascii);
  labels.save("labels.txt", arma::arma_ascii);

  // Hidden layers
  for(int neurons : mSettings.neuronsLayer) {
    if(neurons > 0) {
      mModel.Add<mlpack::Linear>(neurons);
      mModel.Add<mlpack::ReLU>();
    }
  }

  // Output layer
  mModel.Add<mlpack::Linear>(nrOfClasses);
  mModel.Add<mlpack::LogSoftMax>();

  // Optimizer (Backprop with SGD)
  auto batchSize = mSettings.batchSize;
  if(batchSize <= 0) {
    batchSize = trainSamples.rows;
  }

  // Stochastic Gradient Descent
  ens::StandardSGD optimizer(mSettings.learningRate, batchSize, mSettings.maxIterations, mSettings.terminationEpsilon, true);

  // Train
  mModel.Train(data, labels, optimizer, ens::ProgressBar(100));    // prints progress every 5%);
  mlpack::data::Save(modelStoragePath.string(), "model", mModel, true, mlpack::data::format::json);
}

}    // namespace joda::ml
