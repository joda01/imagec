///
/// \file      object_math_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <cstdint>
#include <set>
#include <vector>
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/global_enums.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ObjectsToImageSettings : public SettingBase
{
  enum class Function
  {
    UNKNOWN,
    NONE,
    NOT,
    AND,
    OR,
    XOR,
    AND_NOT
  };

  //
  // What should happen when an intersection was found
  //
  Function function = Function::AND;

  //
  // Objects to use for intersection calculation
  //
  ObjectInputCluster inputObjectFirst;

  //
  // Objects to calc the intersection with
  //
  ObjectInputCluster inputObjectSecond = {joda::enums::ClusterIdIn::UNDEFINED, joda::enums::ClassIdIn::UNDEFINED};

  /////////////////////////////////////////////////////
  void check() const
  {
    if(function != Function::NOT && function != Function::NONE) {
      CHECK_ERROR(inputObjectSecond.clusterId != joda::enums::ClusterIdIn::UNDEFINED &&
                      inputObjectSecond.classId != joda::enums::ClassIdIn::UNDEFINED,
                  "Object to image needs a second operand!");
    }
  }

  settings::ObjectInputClusters getInputClustersAndClasses() const override
  {
    settings::ObjectInputClusters clusters;
    clusters.emplace(inputObjectFirst);

    if(function != Function::NOT && function != Function::NONE) {
      clusters.emplace(inputObjectSecond);
    }

    return clusters;
  }

  [[nodiscard]] ObjectOutputClusters getOutputClustersAndClasses() const override
  {
    ObjectOutputClusters out;
    return out;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ObjectsToImageSettings, function, inputObjectFirst, inputObjectSecond);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ObjectsToImageSettings::Function, {
                                                                   {ObjectsToImageSettings::Function::NONE, "NONE"},
                                                                   {ObjectsToImageSettings::Function::NOT, "NOT"},
                                                                   {ObjectsToImageSettings::Function::AND, "AND"},
                                                                   {ObjectsToImageSettings::Function::AND_NOT, "AND-NOT"},
                                                                   {ObjectsToImageSettings::Function::OR, "OR"},
                                                                   {ObjectsToImageSettings::Function::XOR, "XOR"},
                                                               });

}    // namespace joda::settings
