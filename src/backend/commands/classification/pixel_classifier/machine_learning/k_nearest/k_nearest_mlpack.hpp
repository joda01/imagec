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

#include "k_nearest.hpp"

namespace joda::ml {

class KNearestMlPack : public KNearest
{
public:
  /////////////////////////////////////////////////////
  using KNearest::KNearest;
  void train(const cv::Mat &trainSamples, const cv::Mat &trainLabels, int32_t nrOfClasses) override;

private:
  /////////////////////////////////////////////////////
  ModelType getModelType() override
  {
    return ModelType::KNearest;
  };
  Framework getFramework() override
  {
    return Framework::OpenCv;
  };

  void predict(const std::filesystem::path &path, const cv::Mat &image, const cv::Mat &features, cv::Mat &prediction) override;
  void storeModel(const std::filesystem::path &path, const MachineLearningSettings &settings) override;
  void loadModel(const std::filesystem::path &path) override;
};

}    // namespace joda::ml
