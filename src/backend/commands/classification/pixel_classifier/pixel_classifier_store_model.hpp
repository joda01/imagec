///
/// \file      pixel_classifier_store_model.hpp
/// \author    Joachim Danmayr
/// \date      2025-09-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "backend/settings/setting.hpp"
#include "backend/settings/settings_meta.hpp"

namespace joda::ml {

struct PixelClassifierModel
{
  struct Model
  {
  };

  struct ClassLabels
  {
    int32_t classId = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ClassLabels, classId);
  };

  //
  //
  //
  Model model;

  //
  //
  //
  joda::settings::SettingsMeta meta;

  //
  //
  //
  std::list<ClassLabels> classLabels;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PixelClassifierModel, meta, classLabels);
};

}    // namespace joda::ml
