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

#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"
#include "ann_mlp.hpp"

namespace joda::ml {

class AnnMlpCv : public AnnMlp
{
public:
  /////////////////////////////////////////////////////
  using AnnMlp::AnnMlp;

private:
  /////////////////////////////////////////////////////
  ModelType getModelType() override
  {
    return ModelType::ANN_MLP;
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
