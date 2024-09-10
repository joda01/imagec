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
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include <nlohmann/json.hpp>
#include "setting_macro_iterator.hpp"

using PipelineName_t = std::string;

struct SettingParserLog
{
  enum Severity
  {
    INFO,
    WARNING,
    ERROR
  };

  Severity severity = Severity::ERROR;
  std::string commandNameOfOccurrence;
  std::string message;

  void print() const;
};

using SettingParserLog_t = std::vector<SettingParserLog>;

// Example action macro that processes each element
#define ADD_TO_LOG(x) x.getErrorLogRecursive(log);

#define NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Type, ...)                                    \
  mutable SettingParserLog_t joda_settings_log;                                                            \
  friend void to_json(nlohmann::json &nlohmann_json_j, const Type &nlohmann_json_t)                        \
  {                                                                                                        \
    NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__))                               \
  }                                                                                                        \
  friend void from_json(const nlohmann::json &nlohmann_json_j, Type &nlohmann_json_t)                      \
  {                                                                                                        \
    Type nlohmann_json_default_obj;                                                                        \
    NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM_WITH_DEFAULT, __VA_ARGS__))                \
  }                                                                                                        \
  void getErrorLogRecursive(SettingParserLog_t &settingsParserLog)                                         \
  {                                                                                                        \
    joda_settings_log.clear();                                                                             \
    check();                                                                                               \
    settingsParserLog.insert(settingsParserLog.end(), joda_settings_log.begin(), joda_settings_log.end()); \
    JODA_SETTINGS_EXPAND(JODA_SETTINGS_PASTE(JODA_SETTINGS_TO, __VA_ARGS__))                               \
  }

#define NLOHMANN_DEFINE_TYPE_INTRUSIVE_EXTENDED(Type, ...)                                                 \
  mutable SettingParserLog_t joda_settings_log;                                                            \
  friend void to_json(nlohmann::json &nlohmann_json_j, const Type &nlohmann_json_t)                        \
  {                                                                                                        \
    auto &typeIn = const_cast<Type &>(nlohmann_json_t);                                                    \
    typeIn.check();                                                                                        \
    NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__))                               \
  }                                                                                                        \
  friend void from_json(const nlohmann::json &nlohmann_json_j, Type &nlohmann_json_t)                      \
  {                                                                                                        \
    NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__))                             \
  }                                                                                                        \
  void getErrorLogRecursive(SettingParserLog_t &settingsParserLog)                                         \
  {                                                                                                        \
    joda_settings_log.clear();                                                                             \
    check();                                                                                               \
    settingsParserLog.insert(settingsParserLog.end(), joda_settings_log.begin(), joda_settings_log.end()); \
    JODA_SETTINGS_EXPAND(JODA_SETTINGS_PASTE(JODA_SETTINGS_TO, __VA_ARGS__))                               \
  }

#define CHECK_ERROR(okay, what)                                                                              \
  if(!(okay)) {                                                                                              \
    const auto name = std::string(typeid(*this).name());                                                     \
    joda_settings_log.emplace_back(SettingParserLog{                                                         \
        .severity = SettingParserLog::Severity::ERROR, .commandNameOfOccurrence = name, .message = (what)}); \
  }

#define CHECK_WARNING(okay, what)                                                                              \
  if(!(okay)) {                                                                                                \
    const auto name = std::string(typeid(*this).name());                                                       \
    joda_settings_log.emplace_back(SettingParserLog{                                                           \
        .severity = SettingParserLog::Severity::WARNING, .commandNameOfOccurrence = name, .message = (what)}); \
  }

#define CHECK_INFO(okay, what)                                                                              \
  if(!(okay)) {                                                                                             \
    const auto name = std::string(typeid(*this).name());                                                    \
    joda_settings_log.emplace_back(SettingParserLog{                                                        \
        .severity = SettingParserLog::Severity::INFO, .commandNameOfOccurrence = name, .message = (what)}); \
  }

#define THROW_ERROR(what)                              \
  const auto name = std::string(typeid(*this).name()); \
  joda_settings_log.emplace_back(SettingParserLog{     \
      .severity = SettingParserLog::Severity::ERROR, .commandNameOfOccurrence = name, .message = (what)});
