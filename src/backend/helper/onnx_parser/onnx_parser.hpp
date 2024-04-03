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

class OnnxParser
{
public:
  struct Data
  {
    std::string title;
    std::string description;
    std::string modelPath;
    std::vector<std::string> classes;
  };

  static auto findOnnxFiles(const std::string &directory = "models") -> std::map<std::string, Data>
  {
    std::map<std::string, Data> onnxFiles;
    if(fs::exists(directory) && fs::is_directory(directory)) {
      for(const auto &entry : fs::recursive_directory_iterator(directory)) {
        if(entry.is_regular_file() && entry.path().extension().string() == ".onnx") {
          // auto data = readMetaJson(entry.path().string());

          Data data;

          if(!mCache.contains(entry.path().string())) {
            auto result = readClassLabels(entry.path().string());
            for(auto const &[idx, value] : result) {
              data.classes.push_back(value);
            }
          } else {
            // Read form cache
            data.classes = mCache.at(entry.path().string()).classes;
          }

          data.title     = entry.path().filename().string();
          data.modelPath = entry.path().string();
          onnxFiles.emplace(entry.path().string(), data);
        }
      }
    }
    mCache = onnxFiles;
    return onnxFiles;
  };

private:
  /* static void readClassNames(const ::onnx::ModelProto &model)
   {
     // Access the graph from the ONNX model
     const ::onnx::GraphProto &graph = model.graph();

     // Iterate through the nodes in the graph
     for(const ::onnx::NodeProto &node : graph.node()) {
       // Check if the node is an initializer (contains class names)
       if(node.op_type() == "Constant" && node.attribute_size() > 0) {
         const ::onnx::AttributeProto &attribute = node.attribute(0);

         // Check if the attribute is a tensor
         if(attribute.type() == ::onnx::AttributeProto_AttributeType::AttributeProto_AttributeType_TENSOR) {
           const ::onnx::TensorProto &tensor = attribute.t();

           // Check if the tensor has string data
           if(tensor.data_type() == ::onnx::TensorProto_DataType::TensorProto_DataType_STRING) {
             // Iterate through the strings in the tensor (class names)
             for(const std::string &className : tensor.string_data()) {
               std::cout << "Class Name: " << className << std::endl;
             }
           }
         }
       }
     }
   }*/

  /*static auto readMetaJson(std::string onnxFile) -> Data
  {
    std::string modelPath = onnxFile;
    std::string title     = onnxFile;
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

    return {.title = title, .description = description, .modelPath = onnxFile, .classes = readClassLabels(onnxFile)};
  }*/

  static auto readClassLabels(const std::string &onnxFile) -> std::map<int32_t, std::string>
  {
    // Read the content of the file
    std::ifstream file(onnxFile, std::ios::binary);
    if(!file.is_open()) {
      std::cerr << "Error opening file: " << onnxFile << std::endl;
      return {};
    }

    // Read the last 60 chars, we expect the model classes there
    // Move to the end of the file
    int readL = 60;
    file.seekg(0, std::ios::end);
    int fileSize = file.tellg();

    // Calculate the position from where to start reading
    std::streampos startPos = (fileSize - readL > 0) ? fileSize - readL : 0;
    file.seekg(startPos);

    //
    // We get the classes in following format: {0: 'cell', 1: 'cell_cut'}
    // The next lines converts this in a valid JSON
    //
    char ch[readL];
    int size        = 0;
    bool braketOpen = false;
    for(int n = 0; n < readL; n++) {
      char tmp;
      file.read(&tmp, 1);
      if(tmp == '{' || size > 0) {
        if(tmp == '\'') {
          tmp        = '\"';
          braketOpen = false;
        }
        if(tmp == ':') {
          ch[size]   = '\"';
          braketOpen = false;
          size++;
        }
        if(braketOpen && tmp == ' ') {
          // We do not want spaces in the number key
        } else {
          ch[size] = tmp;
        }
        if(tmp == '{') {
          size++;
          ch[size]   = '\"';
          braketOpen = true;
        }
        if(tmp == ',') {
          size++;
          ch[size]   = '\"';
          braketOpen = true;
        }
        if(braketOpen && tmp == ' ') {
        } else {
          size++;
        }
      }
    }
    ch[size] = 0;
    std::string jsonString(ch, size);
    file.close();

    std::map<int32_t, std::string> classes;
    try {
      nlohmann::json parsed = nlohmann::json::parse(jsonString);
      for(auto &[key, value] : parsed.items()) {
        std::cout << "Key: " << key << ", Value: " << value << std::endl;
        classes.emplace(std::stoi(key), value);
      }
    } catch(const std::exception &ex) {
      std::cout << "Could not parse: " << ex.what() << std::endl;
    }

    return classes;
  }

  static inline std::map<std::string, Data> mCache;
};

}    // namespace joda::onnx
