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

#include <cstddef>
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

namespace joda::results {

using GroupKey   = std::string;
using ObjectKey  = uint64_t;
using ChannelKey = joda::settings::ChannelIndex;
using MeasureKey = MeasureChannelKey;

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
  };

  struct MetaMeasureCh
  {
    std::string name;    ///< Name of the channel
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

private:
  /////////////////////////////////////////////////////
  Meta meta;
  std::map<GroupKey, Group> groups;
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
};

}    // namespace joda::results
