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

#include "results.hpp"
#include "backend/results/results_defines.hpp"

namespace joda::results {

Value &Object::emplaceValue(const MeasureKey &key)
{
  Value &val = measurements[key];
  return val;
}

/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Object::setMeta(const ObjectMeta &objectMeta, const std::optional<ImageMeta> &imageMeta)
{
  this->objectMeta = objectMeta;
  this->imageMeta  = imageMeta;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Object &Channel::emplaceObject(ObjectKey key, const ObjectMeta &objectMeta, const std::optional<ImageMeta> &imageMeta)
{
  Object &obj = objects[key];
  obj.setMeta(objectMeta, imageMeta);
  return obj;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Channel::setValidity(joda::func::ResponseDataValidity valid, bool invalidateAllObjects)
{
  channelMeta.valid                = valid;
  channelMeta.invalidateAllObjects = invalidateAllObjects;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Channel::emplaceMeasureChKey(MeasureKey key, const std::string &name)
{
  measuredValues.emplace(key, MeasureChannelMeta{.name = name});
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Channel::setMeta(const ChannelMeta &meta)
{
  channelMeta = meta;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] auto Channel::getObjects() const -> const std::map<ObjectKey, Object> &
{
  return objects;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] size_t Channel::getNrOfObjects() const
{
  return objects.size();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] auto Channel::getMeasuredChannels() const -> const std::map<MeasureKey, MeasureChannelMeta> &
{
  return measuredValues;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Group::setMeta(const GroupMeta &meta)
{
  groupMeta = meta;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Channel &Group::emplaceChannel(ChannelKey key, const ChannelMeta &meta)
{
  Channel &channel = channels[key];
  channel.setMeta(meta);
  return channel;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] auto Group::getChannels() const -> const std::map<ChannelKey, Channel> &
{
  return channels;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
bool Group::containsInvalidChannelWhereOneInvalidatesTheWholeImage() const
{
  for(const auto &[_, channel] : channels) {
    if(channel.getChannelMeta().invalidateAllObjects &&
       func::ResponseDataValidity::VALID != channel.getChannelMeta().valid) {
      return true;
    }
  }
  return false;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Group &WorkSheet::emplaceGroup(const GroupKey &key, const GroupMeta &meta)
{
  Group &group = groups[key];
  group.setMeta(meta);
  return group;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Channel &WorkSheet::emplaceChannel(const ChannelKey &key, const ChannelMeta &name)
{
  return groups[""].emplaceChannel(key, name);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto WorkSheet::getGroups() const -> const std::map<GroupKey, Group> &
{
  return groups;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WorkSheet::saveToFile(std::string filename, const JobMeta &meta,
                           const std::optional<ExperimentMeta> &experimentMeta, std::optional<ImageMeta> imgMeta)
{
  this->jobMeta        = meta;
  this->experimentMeta = experimentMeta;
  this->imageMeta      = imgMeta;
  if(!filename.empty()) {
    nlohmann::json json = *this;
    settings::removeNullValues(json);
    if(!filename.ends_with(".json")) {
      filename += ".json";
    }
    std::ofstream out(filename);
    out << json.dump(2);
    out.close();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WorkSheet::loadFromFile(const std::string &filename)
{
  std::ifstream ifs(filename);
  *this = nlohmann::json::parse(ifs);
  ifs.close();
}

}    // namespace joda::results
