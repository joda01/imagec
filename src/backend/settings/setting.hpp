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

struct ObjectOutputClass
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

  bool operator<(const ObjectOutputClass &input) const
  {
    return (uint16_t) clusterId < (uint16_t) input.clusterId || (uint16_t) classId < (uint16_t) input.classId;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ObjectOutputClass, clusterId, classId);
};

using ObjectOutputClusters = std::set<ObjectOutputClass>;
using ObjectInputClusters  = std::set<ObjectOutputClass>;
using ObjectInputCluster   = ObjectOutputClass;
using ObjectOutputCluster  = ObjectOutputClass;

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
