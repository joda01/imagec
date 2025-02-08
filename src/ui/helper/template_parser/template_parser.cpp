///
/// \file      ai_model_parser.hpp
/// \author    Joachim Danmayr
/// \date      2024-01-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "template_parser.hpp"
#include <duckdb.h>
#include <QDir>
#include <exception>
#include <filesystem>
#include "backend/helper/logger/console_logger.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
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
  settings::PipelineMeta meta;
  std::string configSchema;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(MetaFinder, meta, configSchema);
};

auto TemplateParser::findTemplates(const std::map<std::string, Category> &directories, const std::string &endian)
    -> std::map<Category, std::map<std::string, Data>>
{
  std::map<Category, std::map<std::string, Data>> templates;
  for(const auto &[directory, category] : directories) {
    if(fs::exists(directory) && fs::is_directory(directory)) {
      for(const auto &entry : fs::recursive_directory_iterator(directory)) {
        if(entry.is_regular_file() && entry.path().extension().string() == endian) {
          std::ifstream ifs(entry.path().string());
          try {
            MetaFinder settings = nlohmann::json::parse(ifs);
            std::string name    = settings.meta.name + entry.path().filename().string();
            std::string title   = settings.meta.name;
            if(!settings.meta.revision.empty()) {
              title += ":" + settings.meta.revision;
            }
            if(category == Category::USER) {
              title += " (" + entry.path().filename().string() + ")";
            }
            templates[category].emplace(
                name, Data{.title = title, .description = "", .path = entry.path().string(), .icon = base64ToQPixmap(settings.meta.icon)});
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
  auto homeDir = std::filesystem::path(QDir::toNativeSeparators(QDir::homePath()).toStdString()) / std::filesystem::path("imagec") /
                 std::filesystem::path("templates");
#else
  auto homeDir = std::filesystem::path(QDir::toNativeSeparators(QDir::homePath()).toStdString()) / std::filesystem::path(".imagec") /
                 std::filesystem::path("templates");

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
