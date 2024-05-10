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
#include <optional>
#include <set>
#include <string>
#include <variant>
#include "backend/helper/xz/archive_reader.hpp"
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/results/results_defines.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/channel/channel_index.hpp"
#include "backend/settings/settings.hpp"
#include <nlohmann/detail/macro_scope.hpp>
#include "results_image_meta.hpp"

using GroupKey   = std::string;
using ObjectKey  = uint64_t;
using ChannelKey = joda::settings::ChannelIndex;
using MeasureKey = joda::results::MeasureChannelKey;

// partial specialization (full specialization works too)
namespace nlohmann {

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

static inline const std::string RESULTS_FOLDER_PATH{"results"};
static inline const std::string REPORT_EXPORT_FOLDER_PATH{"reports"};
static inline const std::string IMAGES_FOLDER_PATH{"images"};
static inline const std::string RESULTS_SUMMARY_FILE_NAME{"results_summary"};
static inline const std::string RESULTS_IMAGE_FILE_NAME{"results_image"};
static inline const std::string CONTROL_IMAGE_FILE_NAME{"control_image"};
static inline const std::string RESULTS_XZ_FILE_NAME{"results"};
static inline const std::string RESULTS_XZ_FILE_EXTENSION{".zip"};
static inline const std::string MESSAGE_PACK_FILE_EXTENSION{".msgpack"};
static inline const std::string CONTROL_IMAGES_FILE_EXTENSION{".png"};

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
  template <Valid_t V>
  void setValidity(V val)
  {
    if constexpr(std::is_same_v<V, func::ParticleValidity>) {
      objectMeta.valid = val == func::ParticleValidity::VALID;
    } else {
      objectMeta.valid = val;
    }
  }
  void setMeta(const ObjectMeta &objectMeta, const std::optional<ImageMeta> &imageMeta);
  Value &emplaceValue(const MeasureKey &key);
  [[nodiscard]] auto getMeasurements() const -> const std::map<MeasureKey, Value> &
  {
    return measurements;
  }

  [[nodiscard]] auto getObjectMeta() const -> const ObjectMeta &
  {
    return objectMeta;
  }

  [[nodiscard]] auto getImageMeta() const -> const std::optional<ImageMeta> &
  {
    return imageMeta;
  }

  Value &at(MeasureKey key)
  {
    return measurements[key];
  }

private:
  /////////////////////////////////////////////////////
  ObjectMeta objectMeta;
  std::optional<ImageMeta> imageMeta;
  std::map<MeasureKey, Value> measurements;    // The measurement channels of this object

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Object, objectMeta, imageMeta, measurements);
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
  Object &emplaceObject(ObjectKey, const ObjectMeta &objectMeta, const std::optional<ImageMeta> &imageMeta);
  void emplaceMeasureChKey(MeasureKey key, const std::string &name);
  void setValidity(joda::func::ResponseDataValidity valid, bool invalidateAllObjects);
  void setMeta(const ChannelMeta &);
  [[nodiscard]] size_t getNrOfObjects() const;
  [[nodiscard]] auto getMeasuredChannels() const -> const std::map<MeasureKey, MeasureChannelMeta> &;
  [[nodiscard]] auto getObjects() const -> const std::map<ObjectKey, Object> &;
  [[nodiscard]] auto getChannelMeta() const -> const ChannelMeta &
  {
    return channelMeta;
  }

  Object &at(ObjectKey key)
  {
    return objects[key];
  }

private:
  /////////////////////////////////////////////////////
  ChannelMeta channelMeta;
  std::map<MeasureKey, MeasureChannelMeta> measuredValues;    ///< List of measured values of this channel
  std::map<ObjectKey, Object> objects;                        ///< Objects of this channel (images, spots, nuclei, ...)

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Channel, channelMeta, measuredValues, objects);
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

  void setMeta(const GroupMeta &meta);
  Channel &emplaceChannel(ChannelKey key, const ChannelMeta &);
  [[nodiscard]] bool containsInvalidChannelWhereOneInvalidatesTheWholeImage() const;
  [[nodiscard]] auto getChannels() const -> const std::map<ChannelKey, Channel> &;
  [[nodiscard]] auto getGroupMeta() const -> const GroupMeta &
  {
    return groupMeta;
  }
  Channel &at(ChannelKey key)
  {
    return channels[key];
  }

private:
  /////////////////////////////////////////////////////
  GroupMeta groupMeta;
  std::map<ChannelKey, Channel> channels;    ///< Channels of this group

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Group, groupMeta, channels);
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

  Group &emplaceGroup(const GroupKey &key, const GroupMeta &meta);
  Channel &emplaceChannel(const ChannelKey &key, const ChannelMeta &);

  [[nodiscard]] auto getGroups() const -> const std::map<GroupKey, Group> &;
  [[nodiscard]] auto getJobMeta() const -> const JobMeta &
  {
    return jobMeta;
  }
  void setMeta(const JobMeta &meta, const std::optional<ExperimentMeta> &experimentMeta,
               std::optional<ImageMeta> imgMeta)
  {
    this->jobMeta        = meta;
    this->experimentMeta = experimentMeta;
    this->imageMeta      = imgMeta;
  }

  [[nodiscard]] auto getExperimentMeta() const -> const std::optional<ExperimentMeta> &
  {
    return experimentMeta;
  }

  [[nodiscard]] auto getImageMeta() const -> const std::optional<ImageMeta> &
  {
    return imageMeta;
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

  void saveToFile(std::string filename, const JobMeta &meta, const std::optional<ExperimentMeta> &experimentMeta,
                  std::optional<ImageMeta> imgMeta);
  void loadFromFile(const std::string &filename);
  void deserialize(const std::string &data);

private:
  /////////////////////////////////////////////////////

  /////////////////////////////////////////////////////
  JobMeta jobMeta;
  std::optional<ImageMeta> imageMeta;
  std::optional<ExperimentMeta> experimentMeta;
  std::map<GroupKey, Group> groups;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(WorkSheet, jobMeta, imageMeta, experimentMeta, groups);
};

///
/// \class      WorkBook
/// \author     Joachim Danmayr
/// \brief      A list of links to worksheets
///
class WorkBook
{
public:
  static void createArchiveFromResults(const std::string &xzFileName, const std::string &pathToResultsFolder,
                                       std::optional<std::string> pathToImagesFolder);
  static auto readWorksheetFromArchive(const std::shared_ptr<joda::helper::xz::Archive> archive,
                                       const std::string &filenameOfFileInArchive) -> WorkSheet;

  static auto readImageFromArchive(const std::shared_ptr<joda::helper::xz::Archive> archive,
                                   const std::string &filenameOfFileInArchive) -> cv::Mat;
};

}    // namespace joda::results
