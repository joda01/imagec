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
#include <c4/yml/parse.hpp>
#include <nlohmann/json_fwd.hpp>
#include <ryml.hpp>

namespace joda::onnx {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto OnnxParser::findAiModelFiles(const std::string &directory) -> std::map<std::filesystem::path, Data>
{
  std::lock_guard<std::mutex> lock(lookForMutex);
  std::map<std::filesystem::path, Data> onnxFiles;
  if(fs::exists(directory) && fs::is_directory(directory)) {
    for(const auto &entry : fs::recursive_directory_iterator(directory)) {
      std::string endian;
      if(entry.is_regular_file()) {
        endian = entry.path().extension().string();
      }

      if(entry.is_regular_file() && endian == ".onnx") {
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
      } else if(entry.is_regular_file() && (endian == ".pt" || endian == ".pth")) {
        if(!mCache.contains(entry.path().string())) {
          Data data = parseResourceDescriptionFile(entry.path().parent_path() / "rdf.yaml");
          if(data.modelName.empty()) {
            data.modelName = entry.path().filename().string();
          }
          data.modelPath = entry.path();
          data.type      = ModelType::PYTORCH;
          onnxFiles.emplace(entry.path().string(), data);
        } else {
          // Read form cache
          onnxFiles.emplace(entry.path().string(), mCache.at(entry.path()));
        }
      } else if(entry.is_regular_file() && endian == ".zip") {
        if(!mCache.contains(entry.path().string())) {
          Data data = parseResourceDescriptionFile(entry.path().parent_path() / "rdf.yaml");
          if(data.modelName.empty()) {
            data.modelName = entry.path().filename().string();
          }
          data.modelPath = entry.path();
          data.type      = ModelType::TENSORFLOW;
          onnxFiles.emplace(entry.path().string(), data);
        } else {
          // Read form cache
          onnxFiles.emplace(entry.path().string(), mCache.at(entry.path()));
        }
      }
    }
  }
  mCache = onnxFiles;
  return onnxFiles;
}

auto OnnxParser::getModelInfo(const std::filesystem::path &modelPath) -> Data
{
  if(mCache.contains(modelPath.string())) {
    return mCache.at(modelPath.string());
  }
  return {};
}

auto OnnxParser::parseResourceDescriptionFile(const std::filesystem::path &rdfYaml) -> Data
{
  if(!std::filesystem::exists(rdfYaml)) {
    return {};
  }
  // Open the YAML file
  std::ifstream file(rdfYaml.string());
  if(!file.is_open()) {
    std::cerr << "Failed to open YAML file!" << std::endl;
    return {};
  }

  Data response;

  // Read the entire file into a string
  std::string yamlContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  auto tree = ryml::parse_in_place(yamlContent.data());

  response.description      = std::string(tree["description"].val().str, tree["description"].val().len);
  response.modelName        = std::string(tree["name"].val().str, tree["name"].val().len);
  std::string formatVersion = tree["format_version"].val().str;

  //
  // Data type
  //
  std::string weightsPytorch = std::string(tree["inputs"][0]["data_type"].val().str, tree["inputs"][0]["data_type"].val().len);

  auto input = tree["inputs"][0];
  if(input.has_child("axes")) {
    if(input["axes"].is_seq()) {
      for(auto axis : input["axes"]) {
        /*if(axis.has_child("type")) {
          std::cout << "  Type: " << axis["type"].val() << "\n";
        }*/
        if(axis.has_child("id")) {
          auto id = std::string(axis["id"].val().str, axis["id"].val().len);
          if(axis.has_child("size")) {
            if(id == "y") {
              response.inputHeight = std::stoi(std::string(axis["size"].val().str, axis["size"].val().len));
            } else if(id == "x") {
              response.inputWith = std::stoi(std::string(axis["size"].val().str, axis["size"].val().len));
            }
          }
        }
        /*if(axis.has_child("channel_names")) {
          std::cout << "  Channel Names:\n";
          for(auto name : axis["channel_names"]) {
            std::cout << "    - " << name.val() << "\n";
          }
        }*/
      }

    } else {
      // bcyx
      std::string axes = std::string(tree["inputs"][0]["axes"].val().str, tree["inputs"][0]["axes"].val().len);

      size_t xPos = axes.find('x');
      size_t yPos = axes.find('y');

      //
      // shape = min + k * step for k in {0, 1, ...}
      //
      if(tree["inputs"][0]["shape"].has_child("min")) {
        int32_t minWidth =
            std::stoi(std::string(tree["inputs"][0]["shape"]["min"][xPos].val().str, tree["inputs"][0]["shape"]["min"][xPos].val().len));
        int32_t minHeight =
            std::stoi(std::string(tree["inputs"][0]["shape"]["min"][yPos].val().str, tree["inputs"][0]["shape"]["min"][yPos].val().len));
        int32_t withStep =
            std::stoi(std::string(tree["inputs"][0]["shape"]["step"][xPos].val().str, tree["inputs"][0]["shape"]["step"][xPos].val().len));
        int32_t heightStep =
            std::stoi(std::string(tree["inputs"][0]["shape"]["step"][yPos].val().str, tree["inputs"][0]["shape"]["step"][yPos].val().len));

        int32_t k = ((256.0f - static_cast<float>(minWidth)) / static_cast<float>(withStep));
        if(k < 1) {
          k = 0;
        }
        response.inputWith   = minWidth + k * withStep;
        response.inputHeight = minHeight + k * heightStep;
      } else {
        response.inputWith   = std::stoi(std::string(tree["inputs"][0]["shape"][xPos].val().str, tree["inputs"][0]["shape"][xPos].val().len));
        response.inputHeight = std::stoi(std::string(tree["inputs"][0]["shape"][yPos].val().str, tree["inputs"][0]["shape"][yPos].val().len));
      }
    }
  }
  response.classes.emplace_back("Class 0");
  return response;

  //
  // Weights
  //
  // std::string torchscript        = tree["weights"]["torchscript"]["source"].val().str;
  // std::string pytorch_state_dict = tree["weights"]["pytorch_state_dict"]["source"].val().str;
  // std::string tensorflow_js      = tree["weights"]["tensorflow_js"]["source"].val().str;
  // std::string onnx               = tree["weights"]["onnx"]["source"].val().str;
  // std::string weightsTensorflow  = tree["weights"]["tensorflow_saved_model_bundle"]["source"].val().str;
  // std::string tensorflowVersion  = tree["weights"]["tensorflow_saved_model_bundle"]["tensorflow_version"].val().str;
}

///
/// \brief      ONNX model parser
/// \author
/// \param[in]
/// \param[out]
/// \return
///
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

}    // namespace joda::onnx
