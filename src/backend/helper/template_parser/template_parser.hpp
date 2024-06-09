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

#pragma once

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include "backend/settings/channel/channel_settings.hpp"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <opencv2/gapi/infer/onnx.hpp>
#include <opencv2/opencv.hpp>

namespace joda::helper::templates {

namespace fs = std::filesystem;
// using namespace ::onnx;

class TemplateParser
{
public:
  struct Data
  {
    std::string title;
    std::string description;
    std::string path;
  };

  static auto findTemplates(const std::string &directory = "templates") -> std::map<std::string, Data>
  {
    std::map<std::string, Data> templates;
    if(fs::exists(directory) && fs::is_directory(directory)) {
      for(const auto &entry : fs::recursive_directory_iterator(directory)) {
        if(entry.is_regular_file() && entry.path().extension().string() == ".json") {
          std::ifstream ifs(entry.path().string());
          settings::ChannelSettings settings = nlohmann::json::parse(ifs);
          templates.emplace(settings.meta.name,
                            Data{.title = settings.meta.name, .description = "", .path = entry.path().string()});
        }
      }
    }
    return templates;
  };

private:
};
}    // namespace joda::helper::templates
