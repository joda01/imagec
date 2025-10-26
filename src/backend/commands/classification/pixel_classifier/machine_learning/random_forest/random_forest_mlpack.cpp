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
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"
#include <mlpack/core/data/load.hpp>
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
void RandomForestMlPack::predict(const std::filesystem::path &path, const cv::Mat &image, cv::Mat &prediction)
{
  std::set<TrainingFeatures> featuresSet;
  loadModel(path, featuresSet);
  cv::Mat features = extractFeatures(image, featuresSet, false);

  // 3. Convert OpenCV Mat -> Armadillo matrix
  // mlpack expects columns = samples, rows = features
  arma::mat armaFeatures(features.cols, features.rows);
  for(int i = 0; i < features.rows; ++i) {
    for(int j = 0; j < features.cols; ++j) {
      armaFeatures(j, i) = features.at<float>(i, j);
    }
  }

  // 5. Predict
  arma::Row<size_t> predictions;
  rf.Classify(armaFeatures, predictions);

  // Convert predictions to cv::Mat
  // cv::Mat predMat(predictions.n_elem, 1, CV_32S);
  prediction = cv::Mat(predictions.n_elem, 1, CV_32S);
  for(size_t i = 0; i < predictions.n_elem; ++i) {
    prediction.at<int>(i, 0) = static_cast<int>(predictions[i]);
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
  arma::mat data(trainSamples.cols, trainSamples.rows);
  for(int r = 0; r < trainSamples.rows; ++r) {
    for(int c = 0; c < trainSamples.cols; ++c) {
      data(c, r) = trainSamples.at<float>(r, c);
    }
  }

  // Convert labels to arma::Row<size_t>
  arma::Row<size_t> labels(trainLabels.rows);
  for(int r = 0; r < trainLabels.rows; ++r) {
    labels(r) = static_cast<size_t>(trainLabels.at<int>(r, 0));
  }

  // --- Train RandomForest ---
  rf.Train(data, labels, nrOfClasses, mSettings.maxNumberOfTrees, mSettings.minSampleCount, 0.0, mSettings.maxTreeDepth);

  // std::cout << "✅ Random forest trained successfully with " << numTrees << " trees and " << numClasses << " classes.\n";
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RandomForestMlPack::storeModel(const std::filesystem::path &path, const std::set<TrainingFeatures> &features,
                                    const std::map<enums::ClassId, int32_t> &trainingClasses)
{
  mlpack::data::Save(path.string(), "rf", rf, true, mlpack::data::format::json);

  /*cv::FileStorage fs(path.string(), cv::FileStorage::WRITE | cv::FileStorage::FORMAT_JSON);


  fs << "model"
     << "{";
  mModel->write(fs);
  fs << "}";

  // Convert set of enums to JSON array of strings
  nlohmann::json j_features = nlohmann::json::array();
  for(auto f : features) {
    j_features.push_back(f);    // uses NLOHMANN_JSON_SERIALIZE_ENUM mapping
  }
  // Write features as an array
  fs << "features"
     << "[";
  for(auto f : features) {
    std::string name = nlohmann::json(f).get<std::string>();    // enum -> string
    fs << name;
  }
  fs << "]";

  fs << "meta"
     << "{";
  fs << "author"
     << "";
  fs << "organization"
     << "";
  fs << "group"
     << "";
  fs << "modifiedAt" << helper::timepointToIsoString(std::chrono::system_clock::now());
  fs << "name"
     << "";
  fs << "notes"
     << "";
  fs << "revision"
     << "";
  fs << "tags"
     << "["
     << "]";
  fs << "uid"
     << "";
  fs << "}";

  fs << "classLabels"
     << "[";

  for(const auto &[_, classsId] : trainingClasses) {
    fs << "{";
    fs << "classId" << classsId;
    fs << "}";
  }

  fs << "]";

  std::string name = nlohmann::json(ModelType::RTrees).get<std::string>();    // enum -> string
  fs << "modelType" << name;
  */
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RandomForestMlPack::loadModel(const std::filesystem::path &path, std::set<TrainingFeatures> &features)
{
  mlpack::data::Load(path.string(), "rf", rf, true, mlpack::data::format::json);

  features.emplace(TrainingFeatures::Intensity);
  features.emplace(TrainingFeatures::Gaussian);
  /*
cv::FileStorage fs(path.string(), cv::FileStorage::READ | cv::FileStorage::FORMAT_JSON);
cv::FileNode fnT       = fs["modelType"];
std::string mlModelStr = fnT.string();
auto modelType         = nlohmann::json(mlModelStr).get<ModelType>();

mModel             = cv::ml::RTrees::create();
cv::FileNode model = fs["model"];
mModel->read(model);

// Read sequence of strings
features.clear();
cv::FileNode fn = fs["features"];
for(int i = 0; i < static_cast<int>(fn.size()); i++) {
  std::string value = static_cast<std::string>(fn[i]);
  features.insert(nlohmann::json(value).get<TrainingFeatures>());
}*/
}
}    // namespace joda::ml
