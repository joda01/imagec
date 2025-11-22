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

#include <mutex>
#include "ann_mlp.hpp"

namespace joda::ml {

class AnnMlpPyTorch : public AnnMlp
{
public:
  /////////////////////////////////////////////////////
  using AnnMlp::AnnMlp;
  void predict(const std::filesystem::path &path, const cv::Mat &image, cv::Mat &prediction) override;
  void train(const cv::Mat &trainSamples, const cv::Mat &trainLabels, int32_t nrOfClasses, const std::filesystem::path &modelStoragePath,
             const MachineLearningSettings &settings) override;
  void stopTraining() override;

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t BATCH_SIZE = 4096;

  /////////////////////////////////////////////////////
  bool mStopped = false;
  static inline std::mutex mGpuMutex;
};

}    // namespace joda::ml
