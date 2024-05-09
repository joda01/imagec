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
#include <filesystem>
#include <vector>
#include "backend/duration_count/duration_count.h"
#include "backend/helper/filesystem.hpp"
#include "backend/helper/xz/xz_wrapper.hpp"
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
  auto channelIterator = channels.find(key);
  if(channelIterator == channels.end()) {
    Channel &channel = channels[key];
    channel.setMeta(meta);
    return channel;
  }
  Channel &channel = channels[key];
  if(meta.valid != func::ResponseDataValidity::VALID) {
    channel.setValidity(meta.valid, meta.invalidateAllObjects);
  }
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

std::string removeFileExtension(const std::string &filename)
{
  if(filename.size() >= 5 && filename.substr(filename.size() - 5) == ".json") {
    return filename.substr(0, filename.size() - 5);
  }
  return filename;
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
  filename             = removeFileExtension(filename);
  if(!filename.empty()) {
    nlohmann::json json = *this;
    removeNullValues(json);

    // Write as json

    // std::ofstream out(filename + ".json");
    // out << json.dump(2);
    // out.close();

    // Write as message pack
    {
      std::ofstream out(filename + MESSAGE_PACK_FILE_EXTENSION, std::ios::out | std::ios::binary);
      std::vector<uint8_t> data = nlohmann::json::to_msgpack(json);
      out.write(reinterpret_cast<const char *>(data.data()), data.size());
      out.close();
    }
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
  std::ifstream ifs(filename, std::ios::binary);
  *this = nlohmann::json::from_msgpack(ifs);
  ifs.close();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WorkSheet::deserialize(const std::string &data)
{
  *this = nlohmann::json::from_msgpack(data);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto WorkBook::listResultsFiles(const std::string &xzFileName, const std::string &fileExt)
    -> std::vector<std::filesystem::path>
{
  return helper::xz::listFiles(xzFileName, fileExt);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WorkBook::createArchiveFromResults(const std::string &xzFileName, const std::string &pathToResultsFolder,
                                        std::optional<std::string> pathToImagesFolder)
{
  auto id = DurationCount::start("Start compression");
  std::vector<helper::xz::FolderToAdd> foldersToAdd;

  foldersToAdd.push_back({.pathToFolderToAdd         = pathToResultsFolder,
                          .fileExtensionToAdd        = MESSAGE_PACK_FILE_EXTENSION,
                          .subFolderInArchiveToAddTo = RESULTS_FOLDER_PATH});
  if(pathToImagesFolder.has_value()) {
    foldersToAdd.push_back({.pathToFolderToAdd         = pathToImagesFolder.value(),
                            .fileExtensionToAdd        = CONTROL_IMAGES_FILE_EXTENSION,
                            .subFolderInArchiveToAddTo = IMAGES_FOLDER_PATH});
  }

  helper::xz::createAndAddFiles(xzFileName + RESULTS_XZ_FILE_EXTENSION, foldersToAdd);
  std::filesystem::remove_all(pathToResultsFolder);
  DurationCount::stop(id);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto WorkBook::readWorksheetFromArchive(const std::string &xzFileName, const std::string &filenameOfFileInArchive)
    -> WorkSheet
{
  WorkSheet sheet;
  sheet.deserialize(helper::xz::readFile(xzFileName, filenameOfFileInArchive));
  return sheet;
}

}    // namespace joda::results
