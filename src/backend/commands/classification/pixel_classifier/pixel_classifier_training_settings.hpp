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

namespace joda::settings {

enum class PixelClassifierMethod
{
  UNKNOWN,
  RANDOM_FOREST,
  K_NEAREST
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
  // Output path where the trained model should be stored
  //
  std::filesystem::path outPath;
};

NLOHMANN_JSON_SERIALIZE_ENUM(PixelClassifierMethod, {{PixelClassifierMethod::UNKNOWN, "Unknown"},
                                                     {PixelClassifierMethod::RANDOM_FOREST, "RandomForest"},
                                                     {PixelClassifierMethod::K_NEAREST, "KNearest"}});

}    // namespace joda::settings
