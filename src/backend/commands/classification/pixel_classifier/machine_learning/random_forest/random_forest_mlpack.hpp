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

#include "random_forest.hpp"
#include <mlpack.hpp>

namespace joda::ml {

class RandomForestMlPack : public RandomForest
{
public:
  /////////////////////////////////////////////////////
  using RandomForest::RandomForest;
  void predict(const std::filesystem::path &path, const cv::Mat &image, const cv::Mat &features, cv::Mat &prediction) override;
  void train(const cv::Mat &trainSamples, const cv::Mat &trainLabels, int32_t nrOfClasses) override;
  void storeModel(const std::filesystem::path &path, const MachineLearningSettings &settings) override;
  void loadModel(const std::filesystem::path &path) override;

private:
  ModelType getModelType() override
  {
    return ModelType::RTrees;
  }
  Framework getFramework() override
  {
    return Framework::MlPack;
  }

  /////////////////////////////////////////////////////
  mlpack::tree::RandomForest<mlpack::GiniGain, mlpack::RandomDimensionSelect, mlpack::BestBinaryNumericSplit, mlpack::AllCategoricalSplit, true,
                             mlpack::DefaultBootstrap>
      rf;
};

}    // namespace joda::ml
