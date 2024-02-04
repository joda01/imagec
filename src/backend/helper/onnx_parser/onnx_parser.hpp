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
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <opencv2/gapi/infer/onnx.hpp>
#include <opencv2/opencv.hpp>

namespace joda::onnx {

namespace fs = std::filesystem;
// using namespace ::onnx;

class Onnx
{
public:
  struct Data
  {
    std::string title;
    std::string description;
    std::string fileName;
    std::map<int32_t, std::string> classes;
  };

  static auto findOnnxFiles(const std::string &directory = "models") -> std::map<std::string, Data>
  {
    std::map<std::string, Data> onnxFiles;
    for(const auto &entry : fs::recursive_directory_iterator(directory)) {
      if(entry.is_regular_file() && entry.path().extension() == ".onnx") {
        auto data = readMetaJson(entry.path().string());

        std::ifstream inputFile(entry.path().string(), std::ios::binary);
        // ModelProto model;
        // model.ParseFromIstream(&inputFile);

        // Read and print class names from the ONNX model
        // readClassNames(model);

        if(!data.classes.empty()) {
          onnxFiles.emplace(entry.path().string(), data);
        }
      }
    }
    return onnxFiles;
  };

private:
  /*  static void readClassNames(const ::onnx::ModelProto &model)
    {
      // Access the graph from the ONNX model
      const GraphProto &graph = model.graph();

      // Iterate through the nodes in the graph
      for(const NodeProto &node : graph.node()) {
        // Check if the node is an initializer (contains class names)
        if(node.op_type() == "Constant" && node.attribute_size() > 0) {
          const AttributeProto &attribute = node.attribute(0);

          // Check if the attribute is a tensor
          if(attribute.type() == AttributeProto_AttributeType::AttributeProto_AttributeType_TENSOR) {
            const TensorProto &tensor = attribute.t();

            // Check if the tensor has string data
            if(tensor.data_type() == TensorProto_DataType::TensorProto_DataType_STRING) {
              // Iterate through the strings in the tensor (class names)
              for(const std::string &className : tensor.string_data()) {
                std::cout << "Class Name: " << className << std::endl;
              }
            }
          }
        }
      }
    }*/

  static auto readMetaJson(std::string onnxFile) -> Data
  {
    std::string filename = onnxFile;
    std::string title    = onnxFile;
    std::string description;
    int32_t version = -1;

    try {
      size_t pos = onnxFile.find("onnx");
      while(pos != std::string::npos) {
        onnxFile.replace(pos, 4, "json");        // 4 is the length of "onnx"
        pos = onnxFile.find("onnx", pos + 4);    // Start searching after the replaced substring
      }

      std::ifstream input(onnxFile);
      nlohmann::json parsedData = nlohmann::json::parse(input);
      input.close();

      title       = parsedData["title"];
      description = parsedData["description"];
      version     = parsedData["version"];
    } catch(const std::exception &) {
    }

    return {.title = title, .description = description, .fileName = onnxFile, .classes = readClassLabels(onnxFile)};
  }

  static auto readClassLabels(const std::string &onnxFile) -> std::map<int32_t, std::string>
  {
    auto parseNames = [](const std::string &content) -> std::map<int32_t, std::string> {
      std::map<int, std::string> result;

      // Define a regular expression to match the key-value pairs
      std::regex pattern(R"(\{(\d+): '([^']+)', (\d+): '([^']+)'\})");

      // Create an iterator to iterate over matches
      auto begin = std::sregex_iterator(content.begin(), content.end(), pattern);
      auto end   = std::sregex_iterator();

      // Iterate over matches and store in the map
      std::sregex_iterator i = begin;
      while(i != end) {
        std::smatch match = *i;
        int idx           = 1;
        while(idx < match.size()) {
          int key = std::stoi(match[idx].str());
          idx++;
          std::string value = match[idx].str();
          result[key]       = value;
          idx++;
        }
        i++;
      }

      return result;
    };

    // Read the content of the file
    std::ifstream file(onnxFile, std::ios::binary);
    if(!file.is_open()) {
      std::cerr << "Error opening file: " << onnxFile << std::endl;
      return {};
    }

    // Move to the end of the file
    file.seekg(0, std::ios::end);

    //
    // Go to the last line
    //
    char ch      = ' ';    // Init ch not equal to '\n'
    int maxCount = -1;
    while(ch != '{' && maxCount > -1024) {
      file.seekg(maxCount, std::ios::end);    // Two steps back, this means we
                                              // will NOT check the last character
      if((int) file.tellg() <= 0) {           // If passed the start of the file,
        file.seekg(0);                        // this is the start of the line
        break;
      }
      // file.get(ch);    // Check the next character
      file.read(&ch, 1);
      maxCount--;
    }

    //
    // Read last line
    //
    std::string fileContent;
    std::getline(file, fileContent);
    file.close();

    // Look for the keyword "names" and parse key-value pairs
    size_t pos = fileContent.find("names");
    std::map<int, std::string> resultMap;
    if(pos != std::string::npos) {
      std::string namesContent = fileContent.substr(pos);
      namesContent             = removeEverythingBeforeBracket(namesContent);
      // Call the parseNames function
      resultMap = parseNames(namesContent);
    } else {
      std::cout << "Keyword 'names' not found in the file." << std::endl;
    }

    return resultMap;
  }

  static std::string removeEverythingBeforeBracket(const std::string &input)
  {
    size_t pos = input.find('{');

    if(pos != std::string::npos) {
      return input.substr(pos);
    }    // Handle the case when the delimiter is not found
    return input;
  }
};

}    // namespace joda::onnx
