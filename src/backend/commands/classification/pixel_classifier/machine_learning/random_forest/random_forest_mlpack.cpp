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

#include "random_forest_mlpack.hpp"
#include <cstddef>
#include <fstream>
#include <string>
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"
#include <mlpack/core/data/load.hpp>
#include <nlohmann/json_fwd.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <mlpack.hpp>

namespace joda::ml {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RandomForestMlPack::predict(const std::filesystem::path &path, const cv::Mat &image, const cv::Mat &features, cv::Mat &prediction)
{
  std::set<TrainingFeatures> featuresSet;
  loadModel(path);

  // 3. Convert OpenCV Mat -> Armadillo matrix
  // mlpack expects columns = samples, rows = features
  arma::mat armaFeatures(static_cast<arma::uword>(features.cols), static_cast<arma::uword>(features.rows));
  for(int i = 0; i < features.rows; ++i) {
    for(int j = 0; j < features.cols; ++j) {
      armaFeatures(static_cast<arma::uword>(j), static_cast<arma::uword>(i)) = static_cast<double>(features.at<float>(i, j));
    }
  }

  // 5. Predict
  arma::Row<size_t> predictions;
  rf.Classify(armaFeatures, predictions);

  // ----------------------
  // 5. Round predictions to nearest class
  // ----------------------
  //  arma::Row<size_t> predictions = arma::conv_to<arma::Row<size_t>>::from(arma::round(predictionsFloat));

  // Convert predictions to cv::Mat
  // cv::Mat predMat(predictions.n_elem, 1, CV_32S);
  prediction = cv::Mat(static_cast<int>(predictions.n_elem), 1, CV_32S);
  for(size_t i = 0; i < predictions.n_elem; ++i) {
    prediction.at<int>(static_cast<int>(i), 0) = static_cast<int>(predictions[i]);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RandomForestMlPack::train(const cv::Mat &trainSamples, const cv::Mat &trainLabels, int32_t nrOfClasses)
{
  // --- Convert OpenCV Mat to Armadillo ---
  arma::mat data(static_cast<arma::uword>(trainSamples.cols), static_cast<arma::uword>(trainSamples.rows));

  for(int r = 0; r < trainSamples.rows; ++r) {
    for(int c = 0; c < trainSamples.cols; ++c) {    // Features
      data(c, r) = static_cast<double>(trainSamples.at<float>(r, c));
    }
  }

  // Convert labels to arma::Row<size_t>
  arma::Row<size_t> labels(static_cast<arma::uword>(trainLabels.rows));
  for(int r = 0; r < trainLabels.rows; ++r) {
    labels(static_cast<arma::uword>(r)) = static_cast<size_t>(trainLabels.at<int>(r, 0));
  }

  // --- Train RandomForest ---
  rf.Train(data, labels, static_cast<size_t>(nrOfClasses), static_cast<size_t>(mSettings.maxNumberOfTrees),
           static_cast<size_t>(mSettings.minSampleCount), 0.0, static_cast<size_t>(mSettings.maxTreeDepth));
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RandomForestMlPack::storeModel(const std::filesystem::path &path, const MachineLearningSettings &settings)
{
  mlpack::data::Save(path.string(), "model", rf, true, mlpack::data::format::json);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RandomForestMlPack::loadModel(const std::filesystem::path &path)
{
  mlpack::data::Load(path.string(), "model", rf, true, mlpack::data::format::json);
}
}    // namespace joda::ml
