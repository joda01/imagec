

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

using GroupKey  = std::string;
using ObjectKey = uint64_t;

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
  std::variant<double, func::ParticleValidity> val;    ///< Value

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
};

///
/// \class      Object
/// \author     Joachim Danmayr
/// \brief      A measured object (ROI)
///
class Object
{
public:
  struct Meta
  {
    std::string name;    ///< Name for the object
    std::string ref;     ///< Link to the object
    bool valid;          ///< True if the object is valid, else false
  };
  Meta meta;

  template <Valid_t V>
  void setValidity(V val)
  {
    if constexpr(std::is_same_v<V, func::ParticleValidity>) {
      meta.valid = val == func::ParticleValidity::VALID;
    } else {
      meta.valid = val;
    }
  }

  void setNameAndRef(const std::string &name, const std::string &ref)
  {
    meta.name = name;
    meta.ref  = ref;
  }

  Value &operator[](const MeasureChannelKey &key)
  {
    return measurements[key];
  }
  std::map<MeasureChannelKey, Value> measurements;    // The measurement channels of this object
};

///
/// \class      Channel
/// \author     Joachim Danmayr
/// \brief      The objects detected within image channel
///
class Channel
{
public:
  struct Meta
  {
    std::string name;    ///< Name of the channel
    joda::func::ResponseDataValidity valid =
        joda::func::ResponseDataValidity::VALID;    ///< True if the value is valid, else false
    bool invalidateAllObjects = false;
  };
  Object &operator[](const ObjectKey &key)
  {
    return objects[key];
  }
  void emplaceMeasureChKey(MeasureChannelKey key)
  {
    measuredValues.emplace(key);
  }
  void setValidity(joda::func::ResponseDataValidity valid, bool invalidateAllObjects)
  {
    meta.valid                = valid;
    meta.invalidateAllObjects = invalidateAllObjects;
  }
  void setName(const std::string &name)
  {
    meta.name = name;
  }
  [[nodiscard]] auto getName() const -> const std::string &
  {
    return meta.name;
  }
  [[nodiscard]] size_t getNrOfObjects() const
  {
    return objects.size();
  }
  [[nodiscard]] auto getMeasuredChannels() const -> const std::set<MeasureChannelKey> &
  {
    return measuredValues;
  }

  [[nodiscard]] auto getObjects() const -> const std::map<ObjectKey, Object> &
  {
    return objects;
  }

  Meta meta;
  std::set<MeasureChannelKey> measuredValues;    ///< List of measured values of this channel
  std::map<ObjectKey, Object> objects;           ///< Objects of this channel (images, spots, nuclei, ...)
};

///
/// \class      Group
/// \author     Joachim Danmayr
/// \brief      A group of channels
///
class Group
{
public:
  struct Meta
  {
    std::string name;    ///< Name of the group
  };
  Meta meta;
  std::map<joda::settings::ChannelIndex, Channel> channels;    ///< Channels of this group

  Channel &operator[](const joda::settings::ChannelIndex &key)
  {
    return channels[key];
  }

  [[nodiscard]] auto getChannels() const -> const std::map<joda::settings::ChannelIndex, Channel> &
  {
    return channels;
  }

  [[nodiscard]] auto at(joda::settings::ChannelIndex key) const -> const Channel &
  {
    return channels.at(key);
  }

  [[nodiscard]] bool containsInvalidChannelWhereOneInvalidatesTheWholeImage() const
  {
    for(const auto &[_, channel] : channels) {
      if(channel.meta.invalidateAllObjects && func::ResponseDataValidity::VALID != channel.meta.valid) {
        return true;
      }
    }
    return false;
  }
};

///
/// \class      WorkSheet
/// \author     Joachim Danmayr
/// \brief      A worksheet contains any number of groups
///
class WorkSheet
{
public:
  struct Meta
  {
    std::string swVersion;
    std::string buildTime;
    std::string jobName;
    std::chrono::system_clock::time_point timeStarted;
    std::chrono::system_clock::time_point timeFinished;
    std::string nrOfChannels;
  };

  Meta meta;
  std::map<GroupKey, Group> groups;

  Group &operator[](const GroupKey &key)
  {
    return groups[key];
  }

  [[nodiscard]] const Group &root() const
  {
    return groups.at("");
  }

  Channel &operator[](const joda::settings::ChannelIndex &key)
  {
    return groups[""][key];
  }
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
