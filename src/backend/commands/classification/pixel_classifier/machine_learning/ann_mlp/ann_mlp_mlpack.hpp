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

#include "ann_mlp.hpp"
#include <mlpack.hpp>

namespace joda::ml {

class AnnMlpMlPack : public AnnMlp
{
public:
  /////////////////////////////////////////////////////
  using AnnMlp::AnnMlp;
  void predict(const std::filesystem::path &path, const cv::Mat &image, const cv::Mat &features, cv::Mat &prediction) override;
  void train(const cv::Mat &trainSamples, const cv::Mat &trainLabels, int32_t nrOfClasses) override;
  void storeModel(const std::filesystem::path &path, const MachineLearningSettings &settings) override;
  void loadModel(const std::filesystem::path &path) override;

private:
  ModelType getModelType() override
  {
    return ModelType::ANN_MLP;
  };
  Framework getFramework() override
  {
    return Framework::MlPack;
  };

  /////////////////////////////////////////////////////
  mlpack::ann::FFN<mlpack::NegativeLogLikelihood, mlpack::RandomInitialization> mModel;
};

}    // namespace joda::ml
