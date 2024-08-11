///
/// \file      onnx_parser.hpp
/// \author    Joachim Danmayr
/// \date      2024-01-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "template_parser.hpp"
#include <duckdb.h>
#include <QDir>
#include <filesystem>
#include "backend/helper/logger/console_logger.hpp"
#include "backend/settings/vchannel/vchannel_intersection.hpp"
#include "backend/settings/vchannel/vchannel_voronoi_settings.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::helper::templates {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
struct MetaFinder
{
  settings::ChannelSettingsMeta meta;
  std::string configSchema;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(MetaFinder, meta, configSchema);
};

auto TemplateParser::findTemplates(const std::map<std::string, bool> &directories) -> std::map<std::string, Data>
{
  std::map<std::string, Data> templates;
  for(const auto &[directory, userTemplate] : directories) {
    if(fs::exists(directory) && fs::is_directory(directory)) {
      for(const auto &entry : fs::recursive_directory_iterator(directory)) {
        if(entry.is_regular_file() && entry.path().extension().string() == TEMPLATE_ENDIAN) {
          std::ifstream ifs(entry.path().string());
          MetaFinder settings = nlohmann::json::parse(ifs);
          std::string order   = userTemplate ? "B" : "A";
          order += settings.meta.name + entry.path().filename().string();
          std::string title = settings.meta.name;
          if(userTemplate) {
            title += " (" + entry.path().filename().string() + ")";
          }
          templates.emplace(order, Data{.title        = title,
                                        .description  = "",
                                        .path         = entry.path().string(),
                                        .icon         = base64ToQPixmap(settings.meta.icon),
                                        .userTemplate = userTemplate});
        }
      }
    }
  }
  return templates;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
struct SchemaFinder
{
  std::string configSchema;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(SchemaFinder, configSchema);
};
auto TemplateParser::loadChannelFromTemplate(const std::filesystem::path &pathToTemplate) -> LoadedChannel
{
  std::ifstream ifs(pathToTemplate.string());
  nlohmann::json json = nlohmann::json::parse(ifs);
  ifs.close();
  return loadChannelFromTemplate(json);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto TemplateParser::loadChannelFromTemplate(const nlohmann::json &templateJson) -> LoadedChannel
{
  SchemaFinder schema = templateJson;

  LoadedChannel loaded;
  if(schema.configSchema == "https://imagec.org/schemas/v1/channel-settings.json") {
    settings::ChannelSettings settings = templateJson;
    loaded.channel                     = settings;
  } else if(schema.configSchema == "https://imagec.org/schemas/v1/voronoi-settings.json") {
    settings::VChannelVoronoi settings = templateJson;
    loaded.voronoi                     = settings;
  } else if(schema.configSchema == "https://imagec.org/schemas/v1/intersectrion-settings.json") {
    settings::VChannelIntersection settings = templateJson;
    loaded.intersection                     = settings;
  }
  return loaded;
}

///
/// \brief      Save template in users home directory
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto TemplateParser::getUsersTemplateDirectory() -> std::filesystem::path
{
  auto homeDir = std::filesystem::path(QDir::homePath().toStdString()) / ".imagec" / "templates";
  if(!fs::exists(homeDir) || !fs::is_directory(homeDir)) {
    try {
      fs::create_directories(homeDir);
    } catch(const fs::filesystem_error &e) {
      joda::log::logError("Cannot create users template directory!");
    }
  }
  return homeDir.string();
}

///
/// \brief      Save template in users home directory
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void TemplateParser::saveTemplate(const LoadedChannel &data, const std::filesystem::path &pathToStoreTemplateIn)
{
  std::string name;
  nlohmann::json json;
  if(data.channel.has_value()) {
    json = data.channel.value();
    name = data.channel->meta.name;
  } else if(data.intersection.has_value()) {
    json = data.intersection.value();
    name = data.intersection->meta.name;
  } else if(data.voronoi.has_value()) {
    json = data.voronoi.value();
    name = data.voronoi->meta.name;
  }
  saveTemplate(json, pathToStoreTemplateIn);
}

///
/// \brief      Save template in users home directory
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void TemplateParser::saveTemplate(nlohmann::json &json, const std::filesystem::path &pathToStoreTemplateIn)
{
  std::string pathToStore = pathToStoreTemplateIn.string();
  if(!pathToStore.ends_with(TEMPLATE_ENDIAN)) {
    pathToStore += TEMPLATE_ENDIAN;
  }
  removeNullValues(json);
  std::ofstream out(pathToStore);
  out << json.dump(2);
  out.close();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
QPixmap TemplateParser::base64ToQPixmap(const std::string &base64String)
{
  if(base64String.empty()) {
    return {};
  }
  QByteArray decodedData = QByteArray::fromBase64(QString(base64String.data()).toLatin1());
  QImage image;
  image.loadFromData(decodedData, "PNG");
  return QPixmap::fromImage(image);
}

}    // namespace joda::helper::templates
