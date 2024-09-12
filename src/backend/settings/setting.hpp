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
  enum class Severity
  {
    JODA_INFO,
    JODA_WARNING,
    JODA_ERROR
  };

  SettingParserLog(Severity sev, const std::string &command, const std::string &msg) :
      severity(sev), commandNameOfOccurrence(command), message(msg)
  {
  }

  Severity severity = Severity::JODA_ERROR;
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
  void getErrorLogRecursive(SettingParserLog_t &settingsParserLog) const                                   \
  {                                                                                                        \
    joda_settings_log.clear();                                                                             \
    check();                                                                                               \
    settingsParserLog.insert(settingsParserLog.end(), joda_settings_log.begin(), joda_settings_log.end()); \
    JODA_SETTINGS_EXPAND(JODA_SETTINGS_PASTE(JODA_SETTINGS_TO, __VA_ARGS__))                               \
  }                                                                                                        \
  void CHECK_ERROR(bool okay, const std::string &what) const                                               \
  {                                                                                                        \
    if(!okay) {                                                                                            \
      const auto name = std::string(typeid(*this).name());                                                 \
      joda_settings_log.emplace_back(SettingParserLog::Severity::JODA_ERROR, name, what);                  \
    }                                                                                                      \
  }                                                                                                        \
  void CHECK_WARNING(bool okay, const std::string &what) const                                             \
  {                                                                                                        \
    if(!(okay)) {                                                                                          \
      const auto name = std::string(typeid(*this).name());                                                 \
      auto data       = SettingParserLog(SettingParserLog::Severity::JODA_WARNING, name, what);            \
      joda_settings_log.emplace_back(data);                                                                \
    }                                                                                                      \
  }                                                                                                        \
  void CHECK_INFO(bool okay, const std::string &what) const                                                \
  {                                                                                                        \
    if(!(okay)) {                                                                                          \
      const auto name = std::string(typeid(*this).name());                                                 \
      auto data       = SettingParserLog(SettingParserLog::Severity::JODA_INFO, name, what);               \
      joda_settings_log.emplace_back(data);                                                                \
    }                                                                                                      \
  }                                                                                                        \
  void THROW_ERROR(const std::string &what) const                                                          \
  {                                                                                                        \
    const auto name = std::string(typeid(*this).name());                                                   \
    auto data       = SettingParserLog(SettingParserLog::Severity::JODA_ERROR, name, what);                \
    joda_settings_log.emplace_back(data);                                                                  \
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
  void getErrorLogRecursive(SettingParserLog_t &settingsParserLog) const                                   \
  {                                                                                                        \
    joda_settings_log.clear();                                                                             \
    check();                                                                                               \
    settingsParserLog.insert(settingsParserLog.end(), joda_settings_log.begin(), joda_settings_log.end()); \
    JODA_SETTINGS_EXPAND(JODA_SETTINGS_PASTE(JODA_SETTINGS_TO, __VA_ARGS__))                               \
  }                                                                                                        \
  void CHECK_ERROR(bool okay, const std::string &what) const                                               \
  {                                                                                                        \
    if(!(okay)) {                                                                                          \
      const auto name = std::string(typeid(*this).name());                                                 \
      auto data       = SettingParserLog(SettingParserLog::Severity::JODA_ERROR, name, what);              \
      joda_settings_log.emplace_back(data);                                                                \
    }                                                                                                      \
  }                                                                                                        \
  void CHECK_WARNING(bool okay, const std::string &what) const                                             \
  {                                                                                                        \
    if(!(okay)) {                                                                                          \
      const auto name = std::string(typeid(*this).name());                                                 \
      auto data       = SettingParserLog(SettingParserLog::Severity::JODA_WARNING, name, what);            \
      joda_settings_log.emplace_back(data);                                                                \
    }                                                                                                      \
  }                                                                                                        \
  void CHECK_INFO(bool okay, const std::string &what) const                                                \
  {                                                                                                        \
    if(!(okay)) {                                                                                          \
      const auto name = std::string(typeid(*this).name());                                                 \
      auto data       = SettingParserLog(SettingParserLog::Severity::JODA_INFO, name, what);               \
      joda_settings_log.emplace_back(data);                                                                \
    }                                                                                                      \
  }                                                                                                        \
  void THROW_ERROR(const std::string &what) const                                                          \
  {                                                                                                        \
    const auto name = std::string(typeid(*this).name());                                                   \
    auto data       = SettingParserLog(SettingParserLog::Severity::JODA_ERROR, name, what);                \
    joda_settings_log.emplace_back(data);                                                                  \
  }
