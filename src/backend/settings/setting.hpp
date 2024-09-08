///
/// \file      setting.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <functional>
#include <memory>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include <nlohmann/json.hpp>

#define NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Type, ...)                     \
  friend void to_json(nlohmann::json &nlohmann_json_j, const Type &nlohmann_json_t)         \
  {                                                                                         \
    nlohmann_json_t.check();                                                                \
    NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__))                \
  }                                                                                         \
  friend void from_json(const nlohmann::json &nlohmann_json_j, Type &nlohmann_json_t)       \
  {                                                                                         \
    Type nlohmann_json_default_obj;                                                         \
    NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM_WITH_DEFAULT, __VA_ARGS__)) \
    nlohmann_json_t.check();                                                                \
  }

#define NLOHMANN_DEFINE_TYPE_INTRUSIVE_EXTENDED(Type, ...)                            \
  friend void to_json(nlohmann::json &nlohmann_json_j, const Type &nlohmann_json_t)   \
  {                                                                                   \
    nlohmann_json_t.check();                                                          \
    NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__))          \
  }                                                                                   \
  friend void from_json(const nlohmann::json &nlohmann_json_j, Type &nlohmann_json_t) \
  {                                                                                   \
    NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__))        \
    nlohmann_json_t.check();                                                          \
  }

#define CHECK(okay, what)                                              \
  if(!(okay)) {                                                        \
    const auto name = std::string(typeid(*this).name());               \
    joda::log::logError(static_cast<std::string>(name + "::" + what)); \
  }

// throw std::invalid_argument(static_cast<std::string>(name + "::" + what));

#define THROW(what)                                                            \
  {                                                                            \
    const auto name = std::string(typeid(*this).name());                       \
    throw std::invalid_argument(static_cast<std::string>(name + "::" + what)); \
  }

namespace joda::settings {

struct ClassificatorSetting
{
  //
  // Cluster the objects should be assigned if filter matches
  //
  joda::enums::ClusterIdIn clusterId = joda::enums::ClusterIdIn::$;

  //
  // Class the objects should be assigned if filter matches
  //
  joda::enums::ClassId classId = joda::enums::ClassId::NONE;

  void check() const
  {
  }

  bool operator<(const ClassificatorSetting &input) const
  {
    auto toUint32 = [](enums::ClusterIdIn clu, enums::ClassId cl) -> uint32_t {
      uint32_t out = (((uint16_t) clu) << 16) | (((uint16_t) cl));
      return out;
    };

    return toUint32(clusterId, classId) < toUint32(input.clusterId, input.classId);
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ClassificatorSetting, clusterId, classId);
};

struct ClassificatorSettingOut
{
  //
  // Cluster the objects should be assigned if filter matches
  //
  joda::enums::ClusterId clusterId = joda::enums::ClusterId::UNDEFINED;

  //
  // Class the objects should be assigned if filter matches
  //
  joda::enums::ClassId classId = joda::enums::ClassId::NONE;

  bool operator<(const ClassificatorSettingOut &input) const
  {
    auto toUint32 = [](enums::ClusterId clu, enums::ClassId cl) -> uint32_t {
      uint32_t out = (((uint16_t) clu) << 16) | (((uint16_t) cl));
      return out;
    };

    return toUint32(clusterId, classId) < toUint32(input.clusterId, input.classId);
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ClassificatorSettingOut, clusterId, classId);
};

using ObjectOutputClusters   = std::set<ClassificatorSetting>;
using ObjectInputClusters    = std::set<ClassificatorSetting>;
using ObjectInputCluster     = ClassificatorSetting;
using ObjectOutputCluster    = ClassificatorSetting;
using ObjectInputClustersExp = std::set<ClassificatorSettingOut>;

class SettingBase
{
public:
  SettingBase() = default;
  [[nodiscard]] virtual std::set<enums::ClusterIdIn> getInputClusters() const
  {
    return {};
  }

  [[nodiscard]] virtual ObjectOutputClusters getOutputClasses() const
  {
    return {};
  }
};

}    // namespace joda::settings

// namespace joda::settings
