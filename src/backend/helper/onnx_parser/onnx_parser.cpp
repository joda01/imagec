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
#include <onnx/onnx_pb.h>    // The ONNX protobuf headers
#include <mutex>
#include <nlohmann/json_fwd.hpp>

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
  std::lock_guard<std::mutex> lock(lookForMutex);
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
        data.type      = ModelType::ONNX;
        onnxFiles.emplace(entry.path().string(), data);
      } else if(entry.is_regular_file() && entry.path().extension().string() == ".pt") {
        Data data;
        data.classes   = {{"Class 01"}};
        data.modelName = entry.path().filename().string();
        data.modelPath = entry.path();
        data.type      = ModelType::PYTORCH;
        onnxFiles.emplace(entry.path().string(), data);

      } else if(entry.is_regular_file() && entry.path().extension().string() == ".tensor") {
      }
    }
  }
  mCache = onnxFiles;
  return onnxFiles;
};

// Function to get the number of outputs in an ONNX model
std::map<int, std::string> OnnxParser::getONNXModelOutputClasses(const std::filesystem::path &modelPath)
{
  // Read the ONNX model into a protobuf object
  ::onnx::ModelProto model;
  std::ifstream input(modelPath.string(), std::ios::in | std::ios::binary);

  if(!input) {
    throw std::runtime_error("Cannot open ONNX!");
  }

  // Parse the input stream into the ONNX model protobuf structure
  if(!model.ParseFromIstream(&input)) {
    throw std::runtime_error("Cannot parse ONNX!");
  }

  // Get the number of outputs from the model graph
  std::map<int, std::string> output_classes;
  const ::onnx::GraphProto &graph = model.graph();

  for(int i = 0; i < graph.output_size(); ++i) {
    const ::onnx::ValueInfoProto &output_info = graph.output(i);
    output_classes.emplace(i, output_info.name());
  }

  return output_classes;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto OnnxParser::getOnnxInfo(const std::filesystem::path &path) -> Data
{
  std::lock_guard<std::mutex> lock(lookForMutex);
  if(mCache.contains(path)) {
    return mCache.at(path);
  }
  return {};
}

/*
std::map<int, std::string> OnnxParser::getONNXModelOutputClasses(const std::filesystem::path &modelPath)
{
  // Initialize ONNX Runtime
  Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "ONNX Model Output Classes Reader");

  // Define the session options and load the model
  Ort::SessionOptions session_options;
  Ort::Session session(env, modelPath.c_str(), session_options);

  // Get the output count
  size_t output_count = session.GetOutputCount();

  // Create a vector to hold the output classes with their indices
  std::map<int, std::string> output_classes;

  // Allocate memory for output names using ONNX Runtime allocator
  Ort::AllocatorWithDefaultOptions allocator;

  // Get model output names
  for(size_t i = 0; i < output_count; ++i) {
    // Get output name allocated by ONNX Runtime
    auto meta                               = session.GetModelMetadata();
    Ort::AllocatedStringPtr output_name_ptr = session.GetOutputNameAllocated(i, allocator);
    std::string output_name(output_name_ptr.get());    // Convert to std::string
    // Add the index and output name to the vector
    output_classes.emplace(i, output_name);
    //////////////////////////

    Ort::AllocatorWithDefaultOptions customMeta;
    std::vector<Ort::AllocatedStringPtr> customMetaKey = meta.GetCustomMetadataMapKeysAllocated(customMeta);
    for(const auto &m : customMetaKey) {
      std::string key = std::string(m.get());
      if(key == "names") {
        Ort::AllocatorWithDefaultOptions customMetaAlloc;
        auto data     = meta.LookupCustomMetadataMapAllocated(m.get(), customMetaAlloc);
        auto elements = parseName(std::string(data.get()));
        if(elements.contains(i)) {
          output_classes.at(i) = elements.at(i);
        }
      }
    }
  }

  return output_classes;
}
*/

std::map<int, std::string> OnnxParser::parseName(const std::string &inputIn)
{
  // Remove the curly braces
  auto input = inputIn.substr(1, inputIn.size() - 2);

  // Map to store parsed data
  std::map<int, std::string> parsed_data;

  std::istringstream ss(input);
  std::string key_value_pair;

  // Split by commas to get key-value pairs
  while(std::getline(ss, key_value_pair, ',')) {
    // Find the position of the colon (key-value separator)
    size_t colon_pos = key_value_pair.find(':');

    // Extract the key
    std::string key_str = key_value_pair.substr(0, colon_pos);
    key_str.erase(remove(key_str.begin(), key_str.end(), ' '), key_str.end());    // Remove whitespace
    int key = std::stoi(key_str);

    // Extract the value
    std::string value = key_value_pair.substr(colon_pos + 1);
    value.erase(remove(value.begin(), value.end(), ' '), value.end());     // Remove whitespace
    value.erase(remove(value.begin(), value.end(), '\''), value.end());    // Remove quotes

    // Store the parsed key-value pair
    parsed_data[key] = value;
  }

  return parsed_data;
}

}    // namespace joda::onnx
