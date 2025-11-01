///
/// \file      machine_learning.hpp
/// \author    Joachim Danmayr
/// \date      2025-10-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <filesystem>
#include "backend/artifacts/object_list/object_list.hpp"
#include <opencv2/core/mat.hpp>
#include "machine_learning_settings.hpp"

namespace joda::ml {

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class MachineLearning
{
public:
  virtual ~MachineLearning() = default;
  void forward(const std::filesystem::path &path, cv::Mat &image);
  void train(const MachineLearningSettings &settings, const cv::Mat &image, const enums::TileInfo &tileInfo, const atom::ObjectList &result);
  virtual void stopTraining()                       = 0;
  virtual auto getTrainingProgress() -> std::string = 0;

protected:
  /////////////////////////////////////////////////////
  static cv::Mat extractFeatures(const cv::Mat &img, const std::set<TrainingFeatures> &features, bool normalizeForMLP);
  static void prepareTrainingDataFromROI(const cv::Mat &image, const enums::TileInfo &tileInfo,
                                         const std::map<enums::ClassId, int32_t> &classesToTrain, joda::atom::ROI::Category categoryToTain,
                                         const atom::ObjectList &regionOfInterest, cv::Mat &trainSamples, cv::Mat &trainLabels,
                                         const std::set<TrainingFeatures> &featuresSet, bool normalizeForMLP);

private:
  /////////////////////////////////////////////////////
  virtual void predict(const std::filesystem::path &path, const cv::Mat &image, cv::Mat &prediction) = 0;
  virtual void train(const cv::Mat &trainSamples, const cv::Mat &trainLabels, int32_t nrOfClasses, const std::filesystem::path &modelStoragePath,
                     const MachineLearningSettings &settings)                                        = 0;
};
}    // namespace joda::ml
