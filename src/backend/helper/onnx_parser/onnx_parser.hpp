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

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <opencv2/gapi/infer/onnx.hpp>
#include <opencv2/opencv.hpp>

namespace joda::onnx {

namespace fs = std::filesystem;
// using namespace ::onnx;

class OnnxParser
{
public:
  enum class ModelType
  {
    UNKNOWN,
    ONNX,
    PYTORCH,
    TENSORFLOW
  };

  /////////////////////////////////////////////////////
  struct Data
  {
    std::string modelName;
    std::string description;
    std::filesystem::path modelPath;
    std::vector<std::string> classes;
    ModelType type      = ModelType::UNKNOWN;
    int32_t inputWith   = 256;
    int32_t inputHeight = 256;
    int32_t channels    = 1;
    int32_t batchSize   = 1;
  };

  static auto findAiModelFiles(const std::string &directory = "models") -> std::map<std::filesystem::path, Data>;
  static auto getModelInfo(const std::filesystem::path &modelPath) -> Data;

private:
  /////////////////////////////////////////////////////
  static auto parseResourceDescriptionFile(const std::filesystem::path &rdfYaml) -> Data;

  static std::map<int, std::string> getONNXModelOutputClasses(const std::filesystem::path &modelPath);
  static inline std::map<std::filesystem::path, Data> mCache;
  static inline std::mutex lookForMutex;
};

}    // namespace joda::onnx
