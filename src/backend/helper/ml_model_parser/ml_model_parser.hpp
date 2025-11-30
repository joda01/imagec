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

namespace joda::ml {

namespace fs = std::filesystem;
// using namespace ::onnx;

class MlModelParser
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
    std::vector<Author> authors;
    std::string toString() const;
  };

  static auto findMlModelFiles(const std::filesystem::path &workingDirectory) -> std::map<std::filesystem::path, Data>;
  static auto parseOpenCVModelXMLDescriptionFile(const std::filesystem::path &rdfYaml, const std::filesystem::path &workingDirectory) -> Data;
  static auto getUsersMlModelDirectory(const std::filesystem::path &workingDirectory) -> std::filesystem::path;
  static auto getGlobalMlModelDirectory() -> std::filesystem::path;

private:
  /////////////////////////////////////////////////////
  static inline std::map<std::filesystem::path, Data> mCache;
  static inline std::mutex lookForMutex;
};

}    // namespace joda::ml
