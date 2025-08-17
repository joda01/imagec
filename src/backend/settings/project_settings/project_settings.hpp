///
/// \file      project_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-04-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///

#pragma once

#include <set>
#include <vector>
#include "backend/enums/enums_classes.hpp"
#include "backend/global_enums.hpp"
#include <nlohmann/json.hpp>
#include "experiment_settings.hpp"
#include "project_address.hpp"
#include "project_class.hpp"
#include "project_classification.hpp"
#include "project_image_setup.hpp"
#include "project_plates.hpp"

namespace joda::settings {

class ProjectSettings
{
public:
  //
  // Job settings
  //
  ExperimentSettings experimentSettings;

  //
  // Plates analyzed by this run
  //
  Plate plate = {};

  //
  // The address of the experiment
  //
  Address address;

  //
  // Classs and classes
  //
  Classification classification;

  //
  // Directory where outputs will be stored
  //
  std::string workingDirectory;

  void check() const
  {
    CHECK_ERROR(!workingDirectory.empty(), "Working directory must not be empty!");
    // Check classes
    {
      CHECK_ERROR(!classification.classes.empty(), "At least one class must be given!");
      std::set<enums::ClassId> ids;
      for(const auto &element : classification.classes) {
        if(element.classId == enums::ClassId::UNDEFINED) {
          THROW_ERROR("Class >UNDEFINED< is not allowed was used twice!");
        }
        if(ids.contains(element.classId)) {
          THROW_ERROR("Class ID >" + std::to_string(static_cast<uint16_t>(element.classId)) + "< was used twice!");
        }
      }
    }
  }

  //
  // This is legacy and will be removed in a further version
  /// \todo REMOVE, legacy
  //
  std::list<Plate> plates;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ProjectSettings, experimentSettings, plate, address, classification, workingDirectory,
                                                       /*LEGACY*/ plates);
};

}    // namespace joda::settings
