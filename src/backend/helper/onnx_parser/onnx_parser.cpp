///
/// \file      onnx_parser.cpp
/// \author    Joachim Danmayr
/// \date      2024-09-28
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "onnx_parser.hpp"
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>

namespace joda::onnx {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto OnnxParser::findOnnxFiles(const std::string &directory) -> std::map<std::filesystem::path, Data>
{
  std::map<std::filesystem::path, Data> onnxFiles;
  if(fs::exists(directory) && fs::is_directory(directory)) {
    for(const auto &entry : fs::recursive_directory_iterator(directory)) {
      if(entry.is_regular_file() && entry.path().extension().string() == ".onnx") {
        // auto data = readMetaJson(entry.path().string());

        Data data;

        if(!mCache.contains(entry.path().string())) {
          auto result = getONNXModelOutputClasses(entry.path());
          for(auto const &[idx, value] : result) {
            data.classes.push_back(value);
          }
        } else {
          // Read form cache
          data.classes = mCache.at(entry.path()).classes;
        }

        data.modelName = entry.path().filename().string();
        data.modelPath = entry.path();
        onnxFiles.emplace(entry.path().string(), data);
      }
    }
  }
  mCache = onnxFiles;
  return onnxFiles;
};

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto OnnxParser::getOnnxInfo(const std::filesystem::path &path) -> Data
{
  if(mCache.contains(path)) {
    return mCache.at(path);
  }
  return {};
}

std::vector<std::pair<int, std::string>> OnnxParser::getONNXModelOutputClasses(const std::filesystem::path &modelPath)
{
  // Initialize ONNX Runtime
  Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "ONNX Model Output Classes Reader");

  // Define the session options and load the model
  Ort::SessionOptions session_options;
  Ort::Session session(env, modelPath.c_str(), session_options);

  // Get the output count
  size_t output_count = session.GetOutputCount();

  // Create a vector to hold the output classes with their indices
  std::vector<std::pair<int, std::string>> output_classes;

  // Allocate memory for output names using ONNX Runtime allocator
  Ort::AllocatorWithDefaultOptions allocator;

  // Get model output names
  for(size_t i = 0; i < output_count; ++i) {
    // Get output name allocated by ONNX Runtime
    Ort::AllocatedStringPtr output_name_ptr = session.GetOutputNameAllocated(i, allocator);
    std::string output_name(output_name_ptr.get());    // Convert to std::string

    // Add the index and output name to the vector
    output_classes.emplace_back(i, output_name);
  }

  return output_classes;
}

}    // namespace joda::onnx
