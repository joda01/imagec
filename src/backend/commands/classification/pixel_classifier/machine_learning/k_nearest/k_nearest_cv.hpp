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

#include "k_nearest.hpp"

namespace joda::ml {

class KNearestCv : public KNearest
{
public:
  /////////////////////////////////////////////////////
  using KNearest::KNearest;

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

  void predict(const std::filesystem::path &path, const cv::Mat &image, const cv::Mat &features, cv::Mat &prediction,
               const std::filesystem::path &modelStoragePath) override;
  void train(const cv::Mat &trainSamples, const cv::Mat &trainLabels, int32_t nrOfClasses, const std::filesystem::path &modelStoragePath) override;
};

}    // namespace joda::ml
