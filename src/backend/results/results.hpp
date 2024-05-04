///
/// \file      imagec_data.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <variant>
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/results/results_defines.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/channel/channel_index.hpp"
#include "backend/settings/channel/channel_reporting_settings.hpp"
#include "backend/settings/settings.hpp"
#include <nlohmann/detail/macro_scope.hpp>

using GroupKey   = std::string;
using ObjectKey  = uint64_t;
using ChannelKey = joda::settings::ChannelIndex;
using MeasureKey = joda::results::MeasureChannelKey;

// partial specialization (full specialization works too)
namespace nlohmann {
template <>
struct adl_serializer<std::chrono::system_clock::time_point>
{
  static void to_json(nlohmann::json &j, const std::chrono::system_clock::time_point &tp)
  {
    // Convert time_point to time_t
    std::time_t time = std::chrono::system_clock::to_time_t(tp);

    // Convert time_t to tm structure
    std::tm timeInfo = *std::localtime(&time);

    // Format tm structure into a string
    std::stringstream ss;
    ss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");

    // Assign the string to JSON
    j = ss.str();
  }

  // Convert JSON to std::chrono::system_clock::time_point
  static void from_json(const nlohmann::json &j, std::chrono::system_clock::time_point &tp)
  {
    std::tm tm = {};
    std::istringstream ss(j.get<std::string>());
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
  }
};

template <typename T>
struct adl_serializer<std::map<MeasureKey, T>>
{
  static void from_json(const nlohmann::json &j, std::map<MeasureKey, T> &ret)
  {
    for(auto it = j.begin(); it != j.end(); ++it) {
      T d                       = it.value().get<T>();
      ret[MeasureKey(it.key())] = d;
    }
  }

  static void to_json(json &result, const std::map<MeasureKey, T> &map)
  {
    for(const auto &[key, value] : map) {
      result[key.toString()] = value;
    }
  }
};

template <typename T>
struct adl_serializer<std::map<ObjectKey, T>>
{
  static void from_json(const nlohmann::json &j, std::map<ObjectKey, T> &ret)
  {
    for(auto it = j.begin(); it != j.end(); ++it) {
      T d                        = it.value().get<T>();
      ret[std::stoull(it.key())] = d;
    }
  }

  static void to_json(json &result, const std::map<ObjectKey, T> &map)
  {
    for(const auto &[key, value] : map) {
      result[std::to_string(key)] = value;
    }
  }
};

template <typename T>
struct adl_serializer<std::map<ChannelKey, T>>
{
  static void from_json(const nlohmann::json &j, std::map<ChannelKey, T> &ret)
  {
    for(auto it = j.begin(); it != j.end(); ++it) {
      T d                                        = it.value().get<T>();
      ret[joda::settings::from_string(it.key())] = d;
    }
  }

  static void to_json(json &result, const std::map<ChannelKey, T> &map)
  {
    for(const auto &[key, value] : map) {
      result[joda::settings::to_string(key)] = value;
    }
  }
};

}    // namespace nlohmann

namespace joda::results {

template <class T>
concept Valid_t = std::is_same_v<T, bool> || std::is_same_v<T, func::ParticleValidity>;

template <class T>
concept Value_t = std::is_same_v<T, double> || std::is_same_v<T, float> || std::is_same_v<T, int64_t> ||
                  std::is_same_v<T, uint32_t> || std::is_same_v<T, int32_t> || std::is_same_v<T, uint64_t> ||
                  std::is_same_v<T, func::ParticleValidity>;

///
/// \class      Value
/// \author     Joachim Danmayr
/// \brief      Represents a single value
///
class Value
{
public:
  /////////////////////////////////////////////////////
  template <Value_t V>
  Value &operator=(V val)
  {
    set(val);
    return *this;
  }

  template <Value_t V>
  void set(V val)
  {
    if constexpr(!std::is_same_v<V, func::ParticleValidity>) {
      this->val = (double) val;
    } else {
      this->val = val;
    }
  }

  [[nodiscard]] auto getVal() const -> const std::variant<double, func::ParticleValidity> &
  {
    return val;
  }

  friend void to_json(nlohmann ::json &nlohmann_json_j, const Value &nlohmann_json_t)
  {
    if(std::holds_alternative<double>(nlohmann_json_t.val)) {
      nlohmann_json_j["val"] = std::get<double>(nlohmann_json_t.val);
    } else if(std::holds_alternative<func::ParticleValidity>(nlohmann_json_t.val)) {
      nlohmann_json_j["val"] = std::get<func::ParticleValidity>(nlohmann_json_t.val);
    }
  }
  friend void from_json(const nlohmann ::json &nlohmann_json_j, Value &nlohmann_json_t)
  {
    if(nlohmann_json_j.at("val").is_number()) {
      nlohmann_json_t.val = (double) nlohmann_json_j.at("val");
    } else if(nlohmann_json_j.at("val").is_string()) {
      nlohmann_json_t.val = (func::ParticleValidity) nlohmann_json_j.at("val");
    }
  };

private:
  /////////////////////////////////////////////////////
  std::variant<double, func::ParticleValidity> val;    ///< Value
};

///
/// \class      Object
/// \author     Joachim Danmayr
/// \brief      A measured object (ROI)
///
class Object
{
public:
  /////////////////////////////////////////////////////
  struct Meta
  {
    std::string name;    ///< Name for the object
    std::string ref;     ///< Link to the object
    bool valid;          ///< True if the object is valid, else false
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Meta, name, ref, valid);
  };

  template <Valid_t V>
  void setValidity(V val)
  {
    if constexpr(std::is_same_v<V, func::ParticleValidity>) {
      meta.valid = val == func::ParticleValidity::VALID;
    } else {
      meta.valid = val;
    }
  }
  void setNameAndRef(const std::string &name, const std::string &ref);
  Value &emplaceValue(const MeasureKey &key);
  [[nodiscard]] auto getMeasurements() const -> const std::map<MeasureKey, Value> &
  {
    return measurements;
  }

  [[nodiscard]] auto getMeta() const -> const Meta &
  {
    return meta;
  }

  Value &at(MeasureKey key)
  {
    return measurements[key];
  }

private:
  /////////////////////////////////////////////////////
  Meta meta;
  std::map<MeasureKey, Value> measurements;    // The measurement channels of this object

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Object, meta, measurements);
};

///
/// \class      Channel
/// \author     Joachim Danmayr
/// \brief      The objects detected within image channel
///
class Channel
{
public:
  /////////////////////////////////////////////////////
  struct Meta
  {
    std::string name;    ///< Name of the channel
    joda::func::ResponseDataValidity valid =
        joda::func::ResponseDataValidity::VALID;    ///< True if the value is valid, else false
    bool invalidateAllObjects = false;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Meta, name, valid, invalidateAllObjects);
  };

  struct MetaMeasureCh
  {
    std::string name;    ///< Name of the channel
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MetaMeasureCh, name);
  };

  Object &emplaceObject(ObjectKey, const std::string &name, const std::string &ref);
  void emplaceMeasureChKey(MeasureKey key, const std::string &name);
  void setValidity(joda::func::ResponseDataValidity valid, bool invalidateAllObjects);
  void setName(const std::string &name);
  [[nodiscard]] auto getName() const -> const std::string &;
  [[nodiscard]] size_t getNrOfObjects() const;
  [[nodiscard]] auto getMeasuredChannels() const -> const std::map<MeasureKey, MetaMeasureCh> &;
  [[nodiscard]] auto getObjects() const -> const std::map<ObjectKey, Object> &;
  [[nodiscard]] auto getMeta() const -> const Meta &
  {
    return meta;
  }

  Object &at(ObjectKey key)
  {
    return objects[key];
  }

private:
  /////////////////////////////////////////////////////
  Meta meta;
  std::map<MeasureKey, MetaMeasureCh> measuredValues;    ///< List of measured values of this channel
  std::map<ObjectKey, Object> objects;                   ///< Objects of this channel (images, spots, nuclei, ...)

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Channel, meta, measuredValues, objects);
};

///
/// \class      Group
/// \author     Joachim Danmayr
/// \brief      A group of channels
///
class Group
{
public:
  /////////////////////////////////////////////////////
  struct Meta
  {
    std::string name;    ///< Name of the group
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Meta, name);
  };

  void setName(const std::string &name);
  Channel &emplaceChannel(ChannelKey key, const std::string &name);
  [[nodiscard]] bool containsInvalidChannelWhereOneInvalidatesTheWholeImage() const;
  [[nodiscard]] auto getChannels() const -> const std::map<ChannelKey, Channel> &;
  [[nodiscard]] auto getMeta() const -> const Meta &
  {
    return meta;
  }
  Channel &at(ChannelKey key)
  {
    return channels[key];
  }

private:
  /////////////////////////////////////////////////////
  Meta meta;
  std::map<ChannelKey, Channel> channels;    ///< Channels of this group

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Group, meta, channels);
};

///
/// \class      WorkSheet
/// \author     Joachim Danmayr
/// \brief      A worksheet contains any number of groups
///
class WorkSheet
{
public:
  /////////////////////////////////////////////////////
  struct Meta
  {
    std::string swVersion;
    std::string buildTime;
    std::string jobName;
    std::chrono::system_clock::time_point timeStarted;
    std::chrono::system_clock::time_point timeFinished;
    std::string nrOfChannels;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Meta, swVersion, buildTime, jobName, timeStarted, timeFinished, nrOfChannels);
  };

  Group &emplaceGroup(const GroupKey &key, const std::string &name);
  Channel &emplaceChannel(const ChannelKey &key, const std::string &name);

  [[nodiscard]] auto getGroups() const -> const std::map<GroupKey, Group> &;
  [[nodiscard]] auto getMeta() const -> const Meta &
  {
    return meta;
  }
  Group &at(const GroupKey &key)
  {
    return groups.at(key);
  }

  Channel &at(const ChannelKey &key)
  {
    return groups[""].at(key);
  }

  [[nodiscard]] const Group &root() const
  {
    return groups.at("");
  }

  void saveToFile(std::string filename) const;
  void loadFromFile(const std::string &filename);

private:
  /////////////////////////////////////////////////////
  Meta meta;
  std::map<GroupKey, Group> groups;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(WorkSheet, meta, groups);
};

///
/// \class      WorkBook
/// \author     Joachim Danmayr
/// \brief      A list of links to worksheets
///
class WorkBook
{
public:
  std::set<std::string> worksheets;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(WorkBook, worksheets);
};

}    // namespace joda::results
