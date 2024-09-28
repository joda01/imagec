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
  /////////////////////////////////////////////////////
  struct Data
  {
    std::string modelName;
    std::string description;
    std::filesystem::path modelPath;
    std::vector<std::string> classes;
  };

  static auto findOnnxFiles(const std::string &directory = "models") -> std::map<std::filesystem::path, Data>;
  static auto getOnnxInfo(const std::filesystem::path &) -> Data;

private:
  /////////////////////////////////////////////////////
  static std::vector<std::pair<int, std::string>> getONNXModelOutputClasses(const std::filesystem::path &modelPath);
  static inline std::map<std::filesystem::path, Data> mCache;
  static inline std::mutex lookForMutex;
};

}    // namespace joda::onnx
