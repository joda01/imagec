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

#pragma once

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include "backend/commands/classification/ai_classifier/ai_classifier_settings.hpp"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <opencv2/gapi/infer/onnx.hpp>
#include <opencv2/opencv.hpp>

namespace joda::ai {

namespace fs = std::filesystem;
// using namespace ::onnx;

class AiModelParser
{
public:
  /////////////////////////////////////////////////////
  struct Data
  {
    struct Author
    {
      std::string affiliation;
      std::string authorName;
    };

    std::string modelName;
    std::string description;
    std::string version;
    std::filesystem::path modelPath;
    std::vector<std::string> classes;
    settings::AiClassifierSettings::ModelParameters modelParameter;
    std::map<std::string, settings::AiClassifierSettings::NetInputParameters> inputs;
    std::map<std::string, settings::AiClassifierSettings::NetOutputParameters> outputs;
    std::vector<Author> authors;
    std::string toString() const;
  };

  static auto findAiModelFiles(const std::string &directory = "models") -> std::map<std::filesystem::path, Data>;
  static auto parseResourceDescriptionFile(std::filesystem::path rdfYaml) -> Data;

private:
  /////////////////////////////////////////////////////

  static std::map<int, std::string> getONNXModelOutputClasses(const std::filesystem::path &modelPath);
  static inline std::map<std::filesystem::path, Data> mCache;
  static inline std::mutex lookForMutex;
};

}    // namespace joda::ai
