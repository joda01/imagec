///
/// \file      random_forest_cv.hpp
/// \author    Joachim Danmayr
/// \date      2025-10-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "random_forest.hpp"

namespace joda::ml {

class RandomForestCv : public RandomForest
{
public:
  /////////////////////////////////////////////////////
  using RandomForest::RandomForest;

private:
  /////////////////////////////////////////////////////
  void predict(const std::filesystem::path &path, const cv::Mat &image, cv::Mat &prediction) override;
  void train(const cv::Mat &trainSamples, const cv::Mat &trainLabels, int32_t nrOfClasses) override;
  void storeModel(const std::filesystem::path &path, const std::set<TrainingFeatures> &features,
                  const std::map<enums::ClassId, int32_t> &trainingClasses) override;
  void loadModel(const std::filesystem::path &path, std::set<TrainingFeatures> &features) override;

  cv::Ptr<cv::ml::RTrees> mModel;
};

}    // namespace joda::ml
