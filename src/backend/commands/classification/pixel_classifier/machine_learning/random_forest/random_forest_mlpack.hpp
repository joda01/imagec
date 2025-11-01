///
/// \file      random_forest_mlpack.hpp
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
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"
#include "random_forest.hpp"

namespace joda::ml {

class RandomForestMlPack : public RandomForest
{
public:
  /////////////////////////////////////////////////////
  using RandomForest::RandomForest;
  void predict(const std::filesystem::path &path, const cv::Mat &image, cv::Mat &prediction) override;
  void train(const cv::Mat &trainSamples, const cv::Mat &trainLabels, int32_t nrOfClasses, const std::filesystem::path &modelStoragePath,
             const MachineLearningSettings &settings) override;
  void stopTraining() override;
  auto getTrainingProgress() -> std::string override;
};

}    // namespace joda::ml
