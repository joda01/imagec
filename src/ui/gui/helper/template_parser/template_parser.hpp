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

#pragma once

#include <qpixmap.h>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <string>
#include <vector>
#include "backend/enums/enums_file_endians.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <opencv2/gapi/infer/onnx.hpp>
#include <opencv2/opencv.hpp>

namespace joda::templates {

namespace fs = std::filesystem;
// using namespace ::onnx;

class TemplateParser
{
public:
  using Group = std::string;
  struct Data
  {
    Group group;
    std::string title;
    std::string description;
    std::string path;
    std::vector<std::string> tags;
    QPixmap icon;
    std::optional<std::string> author;
    std::optional<std::string> organization;
  };

  static void saveTemplate(const joda::settings::Pipeline &data, const std::filesystem::path &pathToStoreTemplateIn);
  static std::filesystem::path saveTemplate(nlohmann::json &, const std::filesystem::path &pathToStoreTemplateIn,
                                            const std::string &endian = joda::fs::EXT_PIPELINE_TEMPLATE);

  static auto findTemplates(const std::set<std::string> &directories, const std::string &endian = joda::fs::EXT_PIPELINE_TEMPLATE)
      -> std::map<Group, std::map<std::string, Data>>;
  static auto loadChannelFromTemplate(const std::filesystem::path &pathToTemplate) -> joda::settings::Pipeline;
  static auto loadTemplate(const std::filesystem::path &pathToTemplate) -> nlohmann::json;
  static auto getUsersTemplateDirectory() -> std::filesystem::path;

private:
  /////////////////////////////////////////////////////
  static QPixmap base64ToQPixmap(const std::string &base64String);
};
}    // namespace joda::templates
