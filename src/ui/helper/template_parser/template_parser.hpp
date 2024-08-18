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
  /////////////////////////////////////////////////////
  static const inline std::string TEMPLATE_ENDIAN = ".ictempl";

  struct Data
  {
    std::string title;
    std::string description;
    std::string path;
    QPixmap icon;
    bool userTemplate = false;
  };

  static void saveTemplate(const joda::settings::Pipeline &data, const std::filesystem::path &pathToStoreTemplateIn);
  static void saveTemplate(nlohmann::json &, const std::filesystem::path &pathToStoreTemplateIn);

  static auto findTemplates(const std::map<std::string, bool> &directories = {
                                {"templates", false},
                                {getUsersTemplateDirectory().string(), true}}) -> std::map<std::string, Data>;
  static auto loadChannelFromTemplate(const std::filesystem::path &pathToTemplate) -> joda::settings::Pipeline;
  static auto getUsersTemplateDirectory() -> std::filesystem::path;

private:
  /////////////////////////////////////////////////////
  static QPixmap base64ToQPixmap(const std::string &base64String);
};
}    // namespace joda::templates
