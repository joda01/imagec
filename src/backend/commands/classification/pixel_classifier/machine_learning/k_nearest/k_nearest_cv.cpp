///
/// \file      random_forest_cv.cpp
/// \author    Joachim Danmayr
/// \date      2025-10-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "k_nearest_cv.hpp"
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"

namespace joda::ml {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void KNearestCv::predict(const std::filesystem::path &path, const cv::Mat &image, cv::Mat &prediction)
{
  std::set<TrainingFeatures> featuresSet;
  loadModel(path, featuresSet);
  cv::Mat features = extractFeatures(image, featuresSet, false);
  if(features.type() != CV_32F || !features.isContinuous()) {
    features.convertTo(features, CV_32F);
  }
  mModel->predict(features, prediction);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void KNearestCv::train(const cv::Mat &trainSamples, const cv::Mat &trainLabels, int32_t /*nrOfClasses*/)
{
  mModel = cv::ml::KNearest::create();
  mModel->setDefaultK(mSettings.defaultK);
  mModel->setIsClassifier(true);
  cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create(trainSamples, cv::ml::ROW_SAMPLE, trainLabels);
  mModel->train(td);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void KNearestCv::storeModel(const std::filesystem::path &path, const std::set<TrainingFeatures> &features,
                            const std::map<enums::ClassId, int32_t> &trainingClasses)
{
  cv::FileStorage fs(path.string(), cv::FileStorage::WRITE | cv::FileStorage::FORMAT_JSON);
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

  std::string name = nlohmann::json(ModelType::KNearest).get<std::string>();    // enum -> string
  fs << "modelType" << name;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void KNearestCv::loadModel(const std::filesystem::path &path, std::set<TrainingFeatures> &features)
{
  cv::FileStorage fs(path.string(), cv::FileStorage::READ | cv::FileStorage::FORMAT_JSON);
  cv::FileNode fnT       = fs["modelType"];
  std::string mlModelStr = fnT.string();
  // auto modelType         = nlohmann::json(mlModelStr).get<ModelType>();

  mModel             = cv::ml::KNearest::create();
  cv::FileNode model = fs["model"];
  mModel->read(model);

  // Read sequence of strings
  features.clear();
  cv::FileNode fn = fs["features"];
  for(int i = 0; i < static_cast<int>(fn.size()); i++) {
    std::string value = static_cast<std::string>(fn[i]);
    features.insert(nlohmann::json(value).get<TrainingFeatures>());
  }
}

}    // namespace joda::ml
