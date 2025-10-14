///
/// \file      ai_model_parser.hpp
/// \author    Joachim Danmayr
/// \date      2024-01-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "template_parser.hpp"
#include <duckdb.h>
#include <QDir>
#include <exception>
#include <filesystem>
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/system/directories.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "backend/settings/settings_meta.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::templates {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
struct MetaFinder
{
  settings::SettingsMeta meta;
  std::string configSchema;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(MetaFinder, meta, configSchema);
};

auto TemplateParser::findTemplates(const std::set<std::string> &directories, const std::string &endian)
    -> std::map<Group, std::map<std::string, Data>>
{
  std::map<Group, std::map<std::string, Data>> templates;
  for(const auto &directory : directories) {
    if(fs::exists(directory) && fs::is_directory(directory)) {
      for(const auto &entry : fs::recursive_directory_iterator(directory)) {
        if(entry.is_regular_file() && entry.path().extension().string() == endian) {
          std::ifstream ifs(entry.path().string());
          try {
            MetaFinder settings = nlohmann::json::parse(ifs);
            std::string name    = entry.path().filename().string() + settings.meta.name;
            std::string title   = settings.meta.name;
            std::string group   = settings.meta.group.value_or("Userdefined");
            if(!settings.meta.revision.empty()) {
              title += ":" + settings.meta.revision;
            }
            if(group == "Userdefined") {
              title += " (" + entry.path().filename().string() + ")";
            }
            templates[group].emplace(name, Data{.group        = group,
                                                .title        = title,
                                                .description  = settings.meta.notes,
                                                .path         = entry.path().string(),
                                                .tags         = settings.meta.tags,
                                                .icon         = base64ToQPixmap(settings.meta.icon),
                                                .author       = settings.meta.author,
                                                .organization = settings.meta.organization});
          } catch(const std::exception &ex) {
            std::cout << "Error " << ex.what() << std::endl;
          }
          ifs.close();
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
auto TemplateParser::loadChannelFromTemplate(const std::filesystem::path &pathToTemplate) -> settings::Pipeline
{
  std::ifstream ifs(pathToTemplate.string());
  nlohmann::json json = nlohmann::json::parse(ifs);
  ifs.close();
  return json;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto TemplateParser::loadTemplate(const std::filesystem::path &pathToTemplate) -> nlohmann::json
{
  std::ifstream ifs(pathToTemplate.string());
  nlohmann::json json = nlohmann::json::parse(ifs);
  ifs.close();
  return json;
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
#ifdef _WIN32
  auto homeDir = std::filesystem::path(QDir::toNativeSeparators(QDir::homePath()).toStdString()) /
                 std::filesystem::path(joda::fs::USER_SETTINGS_PATH) / std::filesystem::path("templates");
#else
  auto homeDir = std::filesystem::path(QDir::toNativeSeparators(QDir::homePath()).toStdString()) /
                 std::filesystem::path("." + joda::fs::USER_SETTINGS_PATH) / std::filesystem::path("templates");

#endif
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
auto TemplateParser::getGlobalTemplateDirectory(const std::string &subPath) -> std::filesystem::path
{
  std::filesystem::path path = joda::system::getExecutablePath() / "templates" / subPath;
  return path;
}

///
/// \brief      Save template in users home directory
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void TemplateParser::saveTemplate(const settings::Pipeline &data, const std::filesystem::path &pathToStoreTemplateIn)
{
  std::string name;
  nlohmann::json json;
  json = data;
  name = data.meta.name;
  saveTemplate(json, pathToStoreTemplateIn);
}

///
/// \brief      Save template in users home directory
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::filesystem::path TemplateParser::saveTemplate(nlohmann::json &json, const std::filesystem::path &pathToStoreTemplateIn,
                                                   const std::string &endian)
{
  std::string pathToStore = pathToStoreTemplateIn.string();
  if(!pathToStore.ends_with(endian)) {
    pathToStore += endian;
  }
  removeNullValues(json);
  std::ofstream out(pathToStore);
  out << json.dump(2);
  out.close();

  return std::filesystem::path(pathToStore);
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

}    // namespace joda::templates
