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
#include <ensmallen_bits/callbacks/progress_bar.hpp>
#include <mlpack/core.hpp>
#include <mlpack/core/data/load.hpp>
#include <mlpack/methods/ann/ffn.hpp>
#include <mlpack/methods/ann/init_rules/random_init.hpp>
#include <mlpack/methods/ann/layer/base_layer.hpp>
#include <mlpack/methods/ann/layer/layer.hpp>
#include <mlpack/methods/ann/loss_functions/negative_log_likelihood.hpp>
#include <nlohmann/json_fwd.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <ensmallen.hpp>

namespace joda::ml {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void AnnMlpMlPack::predict(const std::filesystem::path &path, const cv::Mat &image, const cv::Mat &features, cv::Mat &prediction)
{
  loadModel(path);

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

  arma::mat predictionTemp;
  mModel.Predict(data, predictionTemp);

  std::cout << "Cols " << std::to_string(predictionTemp.n_cols) << std::endl;
  std::cout << "Rows " << std::to_string(predictionTemp.n_rows) << std::endl;

  data.save("data_fe.txt", arma::arma_ascii);

  predictionTemp.save("predictionTemp.txt", arma::arma_ascii);

  // Find index of max prediction for each data point and store in "predictionInt"
  arma::Row<size_t> predictions(data.n_cols);
  for(size_t i = 0; i < data.n_cols; ++i) {
    predictions(i) = arma::as_scalar(arma::find(arma::max(predictionTemp.col(i)) == predictionTemp.col(i), 1));
  }
  predictions.save("predictionIdx.txt", arma::arma_ascii);

  std::cout << "--------" << std::endl;

  // Convert predictions to cv::Mat
  // cv::Mat predMat(predictions.n_elem, 1, CV_32S);
  prediction.create(static_cast<int>(predictions.n_elem), 1, CV_32S);
  for(size_t i = 0; i < predictions.n_elem; ++i) {
    prediction.at<int>(static_cast<int>(i), 0) = static_cast<int>(predictions(i));
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void AnnMlpMlPack::train(const ::cv::Mat &trainSamples, const ::cv::Mat &trainLabels, int32_t nrOfClasses)
{
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

  std::cout << "data: " << data.n_rows << " x " << data.n_cols << " (features x samples)\n";
  std::cout << "labels: " << labels.n_rows << " x " << labels.n_cols << " (if mat) or length " << labels.n_elem << " (if Row<size_t>)\n";

  data.save("data.txt", arma::arma_ascii);
  labels.save("labels.txt", arma::arma_ascii);

  const size_t inputSize  = trainSamples.cols;
  const size_t outputSize = nrOfClasses;

  std::cout << "Input size" << std::to_string(inputSize) << std::endl;
  std::cout << "outputSize size" << std::to_string(outputSize) << std::endl;

  // Input layer
  // mModel.Add<mlpack::Linear>(inputSize);
  // mModel.Add<mlpack::TanH>();

  // Hidden layers
  for(int neurons : mSettings.neuronsLayer) {
    if(neurons > 0) {
      mModel.Add<mlpack::Linear>(neurons);
      mModel.Add<mlpack::Sigmoid>();
    }
  }

  // Output layer
  mModel.Add<mlpack::Linear>(outputSize);
  mModel.Add<mlpack::LogSoftMax>();

  // Optimizer (Backprop with SGD)
  auto batchSize = mSettings.batchSize;
  if(batchSize <= 0) {
    batchSize = trainSamples.rows;
  }
  std::cout << "Samlples" << std::to_string(trainSamples.rows) << std::endl;
  ens::StandardSGD optimizer(0.0005, batchSize, mSettings.maxIterations, mSettings.terminationEpsilon, true);

  // Train
  data = (data - arma::repmat(arma::mean(data, 1), 1, data.n_cols)) / arma::repmat(arma::stddev(data, 0, 1), 1, data.n_cols);
  mModel.Train(data, labels, optimizer, ens::ProgressBar(100));    // prints progress every 5%);
  double avgLoss = mModel.Evaluate(data, labels) / data.n_cols;
  std::cout << "Average loss per sample: " << avgLoss << std::endl;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void AnnMlpMlPack::storeModel(const std::filesystem::path &path, const MachineLearningSettings &settings)
{
  mlpack::data::Save(path.string(), "model", mModel, true, mlpack::data::format::json);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void AnnMlpMlPack::loadModel(const std::filesystem::path &path)
{
  mlpack::data::Load(path.string(), "model", mModel, true, mlpack::data::format::json);
}
}    // namespace joda::ml
