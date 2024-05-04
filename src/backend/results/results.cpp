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
void Object::setNameAndRef(const std::string &name, const std::string &ref)
{
  meta.name = name;
  meta.ref  = ref;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Object &Channel::emplaceObject(ObjectKey key, const std::string &name, const std::string &ref)
{
  Object &obj = objects[key];
  obj.setNameAndRef(name, ref);
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
  meta.valid                = valid;
  meta.invalidateAllObjects = invalidateAllObjects;
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
  measuredValues.emplace(key, MetaMeasureCh{.name = name});
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Channel::setName(const std::string &name)
{
  meta.name = name;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] auto Channel::getName() const -> const std::string &
{
  return meta.name;
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
[[nodiscard]] auto Channel::getMeasuredChannels() const -> const std::map<MeasureKey, MetaMeasureCh> &
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
void Group::setName(const std::string &name)
{
  meta.name = name;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Channel &Group::emplaceChannel(ChannelKey key, const std::string &name)
{
  Channel &channel = channels[key];
  channel.setName(name);
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
    if(channel.getMeta().invalidateAllObjects && func::ResponseDataValidity::VALID != channel.getMeta().valid) {
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
Group &WorkSheet::emplaceGroup(const GroupKey &key, const std::string &name)
{
  Group &group = groups[key];
  group.setName(name);
  return group;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Channel &WorkSheet::emplaceChannel(const ChannelKey &key, const std::string &name)
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
void WorkSheet::saveToFile(std::string filename, const Meta &meta)
{
  this->meta = meta;
  if(!filename.empty()) {
    nlohmann::json json = *this;
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
