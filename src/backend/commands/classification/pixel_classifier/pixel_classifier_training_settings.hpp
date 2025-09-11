///
/// \file      random_forest_settings.hpp
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include "random_forest/random_forest_training_settings.hpp"

namespace joda::settings {

enum class PixelClassifierMethod
{
  UNKNOWN,
  RANDOM_FOREST,
  K_NEAREST,
  ANN_MLP
};

enum class PixelClassifierFeatures
{
  INTENSITY,
  GAUSSIAN,
  LAPLACIAN,
  GRADIENT,
  VARIANCE,
  HESSIAN

};

struct PixelClassifierTrainingSettings
{
  //
  // ID of the class to train
  //
  std::set<int32_t> trainingClasses;

  //
  // Machine learning method to use
  //
  PixelClassifierMethod method = PixelClassifierMethod::UNKNOWN;

  //
  // Features to use for training and prediction
  //
  std::set<PixelClassifierFeatures> features;

  //
  // Output path where the trained model should be stored
  //
  std::filesystem::path outPath;

  //
  // Settings for different algorithms
  //
  std::optional<RandomForestTrainingSettings> randomForest;
};

NLOHMANN_JSON_SERIALIZE_ENUM(PixelClassifierMethod, {{PixelClassifierMethod::UNKNOWN, "Unknown"},
                                                     {PixelClassifierMethod::RANDOM_FOREST, "RandomForest"},
                                                     {PixelClassifierMethod::K_NEAREST, "KNearest"},
                                                     {PixelClassifierMethod::ANN_MLP, "AnnMlp"}});

}    // namespace joda::settings
