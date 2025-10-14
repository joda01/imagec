///
/// \file      ai_model_parser.cpp
/// \author    Joachim Danmayr
/// \date      2024-09-28
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "ai_model_parser.hpp"
#include <onnx/onnx_pb.h>    // The ONNX protobuf headers
#include <qdir.h>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include "backend/commands/classification/ai_classifier/ai_classifier_settings.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/rapidyaml/rapidyaml.hpp"
#include "backend/helper/system/directories.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ai {

///
/// \brief      Save template in users home directory
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto AiModelParser::getUsersAiModelDirectory() -> std::filesystem::path
{
#ifdef _WIN32
  auto homeDir = std::filesystem::path(QDir::toNativeSeparators(QDir::homePath()).toStdString()) /
                 std::filesystem::path(joda::fs::USER_SETTINGS_PATH) / std::filesystem::path("models");
#else
  auto homeDir = std::filesystem::path(QDir::toNativeSeparators(QDir::homePath()).toStdString()) /
                 std::filesystem::path("." + joda::fs::USER_SETTINGS_PATH) / std::filesystem::path("models");

#endif
  if(!fs::exists(homeDir) || !fs::is_directory(homeDir)) {
    try {
      fs::create_directories(homeDir);
    } catch(const fs::filesystem_error &e) {
      joda::log::logError("Cannot create users template directory!");
    }
  }
  return homeDir.string();
}

///
/// \brief      Save template in users home directory
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto AiModelParser::getGlobalAiModelDirectory() -> std::filesystem::path
{
  std::filesystem::path path = joda::system::getExecutablePath() / "models";
  return path;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto AiModelParser::findAiModelFiles() -> std::map<std::filesystem::path, Data>
{
  std::lock_guard<std::mutex> lock(lookForMutex);
  std::map<std::filesystem::path, Data> aiModelFiles;
  std::vector<std::filesystem::path> directories{getGlobalAiModelDirectory(), getUsersAiModelDirectory()};

  for(const auto &directory : directories) {
    if(fs::exists(directory) && fs::is_directory(directory)) {
      for(const auto &entry : fs::recursive_directory_iterator(directory)) {
        if(entry.is_regular_file()) {
          try {
            if(entry.path().string().ends_with("rdf.yaml") || entry.path().string().ends_with("rdf.yml")) {
              auto modelInfo = parseResourceDescriptionFile(entry.path());
              aiModelFiles.emplace(modelInfo.modelPath, modelInfo);
            }
          } catch(const nlohmann::json::parse_error &ex) {
            // std::cerr << "JSON Parse error: " << ex.what() << "\n"
            //           << "Error occurred at byte: " << ex.byte << "\n";
            //
            joda::log::logWarning(entry.path().string() + ": " + std::string(ex.what()));

          } catch(const nlohmann::json::type_error &ex) {
            joda::log::logWarning(entry.path().string() + ": " + std::string(ex.what()));

          } catch(const std::exception &ex) {
            joda::log::logWarning(entry.path().string() + ": " + ex.what());
          }
        }
      }
    }
  }
  mCache = aiModelFiles;
  return aiModelFiles;
}

auto AiModelParser::parseResourceDescriptionFile(std::filesystem::path rdfYaml) -> Data
{
  if(!rdfYaml.string().ends_with("rdf.yaml") && !rdfYaml.string().ends_with("rdf.yml")) {
    rdfYaml = rdfYaml.parent_path() / "rdf.yaml";
  }

  auto calcOptimalSize = [](int32_t min, int32_t step) {
    int32_t k = static_cast<int32_t>((640.0F - static_cast<float>(min)) / static_cast<float>(step));
    if(k < 1) {
      k = 0;
    }
    return min + k * step;
  };

  auto getPos = [](const std::string &axes, char c) -> std::optional<size_t> {
    auto p = axes.find(c);
    if(p == std::string::npos) {
      return std::nullopt;
    }
    return p;
  };

  auto dataTypeToEnum = [](const std::string &typeStr) -> joda::settings::AiClassifierSettings::NetInputDataType {
    if(typeStr == "float32") {
      return joda::settings::AiClassifierSettings::NetInputDataType::FLOAT32;
    }
    if(typeStr == "uint32") {
      return joda::settings::AiClassifierSettings::NetInputDataType::UINT32;
    }
    if(typeStr == "uint16") {
      return joda::settings::AiClassifierSettings::NetInputDataType::UINT16;
    }
    if(typeStr == "uint8") {
      return joda::settings::AiClassifierSettings::NetInputDataType::UINT8;
    }
    return joda::settings::AiClassifierSettings::NetInputDataType::FLOAT32;
  };

  // =======================================
  // Load rdf.yaml file
  // =======================================
  if(!std::filesystem::exists(rdfYaml)) {
    return {};
  }

  std::ifstream file(rdfYaml.string());
  if(!file.is_open()) {
    std::cerr << "Failed to open YAML file!" << std::endl;
    return {};
  }

  // =======================================
  // Convert to JSON and parse
  // =======================================
  std::string yamlContent;
  size_t size = std::filesystem::file_size(rdfYaml);
  yamlContent.resize(size);
  file.read(yamlContent.data(), static_cast<std::streamsize>(size));
  auto rdfParsed = joda::yaml::Yaml::convert(yamlContent);

  // =======================================
  // Extract data
  // =======================================
  Data response;
  std::string rdfFormatVersion = rdfParsed["format_version"];

  response.description = rdfParsed["description"];
  response.modelName   = rdfParsed["name"];

  auto authors = rdfParsed["authors"];
  for(const auto &author : authors) {
    std::string affiliation;
    if(author.contains("affiliation")) {
      affiliation = author["affiliation"];
    }
    response.authors.emplace_back(Data::Author{.affiliation = affiliation, .authorName = author["name"]});
  }

  auto tagsJson = rdfParsed["tags"];
  std::set<std::string> tags;
  for(const std::string tag : tagsJson) {
    tags.emplace(helper::toLower(tag));
  }

  if(rdfParsed.contains("version")) {
    if(rdfParsed["version"].is_string()) {
      response.version = rdfParsed["version"];
    } else {
      float modelVersion = rdfParsed["version"];
      std::ostringstream oss;
      oss << std::fixed << std::setprecision(1) << modelVersion;
      response.version = oss.str();
    }
  }

  // =======================================
  // Weights information data
  // =======================================
  auto weights = rdfParsed["weights"];
  std::string modelWeighsFileName;
  if(weights.contains("onnx")) {
    response.modelParameter.modelFormat = settings::AiClassifierSettings::ModelFormat::ONNX;
    modelWeighsFileName                 = weights["onnx"]["source"];
  } else if(weights.contains("torchscript")) {
    response.modelParameter.modelFormat = settings::AiClassifierSettings::ModelFormat::TORCHSCRIPT;
    modelWeighsFileName                 = weights["torchscript"]["source"];
  } else {
    throw std::invalid_argument("Not supported format!");
  }

  response.modelPath = rdfYaml.parent_path() / modelWeighsFileName;

  // =======================================
  // Input information
  // =======================================
  auto inputs = rdfParsed["inputs"];
  if(inputs.size() != 1) {
    throw std::invalid_argument("Only models with exact one input are supported!");
  }
  if(rdfFormatVersion == "0.4.10") {
    //
    //
    //
    for(const auto &input : inputs) {
      std::string inputName = input["name"];
      response.inputs.emplace(inputName, settings::AiClassifierSettings::NetInputParameters{});
      auto &elementToWork    = response.inputs.at(inputName);
      elementToWork.axes     = input["axes"];    //  "axes": "bcyx"
      elementToWork.dataType = dataTypeToEnum(input["data_type"]);
      // auto preprocessing    = input["preprocessing"];
      // for(const auto &command : preprocessing) {
      //   std::string name = command["name"];
      // }

      //

      std::optional<size_t> bPos = getPos(elementToWork.axes, 'b');
      std::optional<size_t> cPos = getPos(elementToWork.axes, 'c');
      std::optional<size_t> xPos = getPos(elementToWork.axes, 'x');
      std::optional<size_t> yPos = getPos(elementToWork.axes, 'y');

      auto shape = input["shape"];
      if(shape.is_array()) {
        elementToWork.batch = 1;
        if(bPos.has_value()) {
          elementToWork.batch = shape[bPos.value()];
        }
        if(cPos.has_value()) {
          elementToWork.channels = static_cast<settings::AiClassifierSettings::NetChannels>(shape[cPos.value()]);
        }
        if(xPos.has_value()) {
          elementToWork.spaceX = shape[xPos.value()];
        }
        if(yPos.has_value()) {
          elementToWork.spaceY = shape[yPos.value()];
        }

      } else if(shape.is_object()) {
        auto min  = shape["min"];
        auto step = shape["step"];

        int32_t min_c  = 1;
        int32_t step_c = 0;
        if(cPos.has_value()) {
          min_c  = min[cPos.value()];
          step_c = step[cPos.value()];
        }

        int32_t min_b  = 1;
        int32_t step_b = 0;
        if(bPos.has_value()) {
          min_b  = min[bPos.value()];
          step_b = step[bPos.value()];
        }

        int32_t min_y  = 640;
        int32_t step_y = 0;
        if(yPos.has_value()) {
          min_y  = min[yPos.value()];
          step_y = step[yPos.value()];
        }

        int32_t min_x  = 640;
        int32_t step_x = 0;
        if(xPos.has_value()) {
          min_x  = min[xPos.value()];
          step_x = step[xPos.value()];
        }

        elementToWork.channels = static_cast<settings::AiClassifierSettings::NetChannels>(calcOptimalSize(min_c, step_c));
        elementToWork.batch    = calcOptimalSize(min_b, step_b);
        elementToWork.spaceX   = calcOptimalSize(min_x, step_x);
        elementToWork.spaceY   = calcOptimalSize(min_y, step_y);
      }
    }
  } else if(rdfFormatVersion == "0.5.3") {
    //
    //
    //
    std::string axesString;
    for(const auto &input : inputs) {
      std::string inputName = input["id"];
      response.inputs.emplace(inputName, settings::AiClassifierSettings::NetInputParameters{});
      auto &elementToWork = response.inputs.at(inputName);

      auto axes = input["axes"];    //  "axes": "bcyx"
      for(const auto &axe : axes) {
        std::string type = axe["type"];
        if(type == "batch") {
          axesString += "b";
          elementToWork.batch = 1;
          if(axe.contains("size")) {
            elementToWork.batch = axe["size"];
          }
        }
        if(type == "channel") {
          axesString += "c";
          auto channelNames = axe["channel_names"];
          std::vector<std::string> channelNamesOut;
          for(const auto &channelName : channelNames) {
            channelNamesOut.emplace_back(channelName);
          }
          /// How to get correct channel size
          elementToWork.channels = static_cast<settings::AiClassifierSettings::NetChannels>(channelNamesOut.size());
        }
        if(type == "space") {
          std::string id = axe["id"];
          axesString += id;

          int32_t fixedSize = 0;
          auto sizeIn       = axe["size"];
          if(sizeIn.is_object()) {
            int32_t min  = sizeIn["min"];
            int32_t step = sizeIn["step"];
            fixedSize    = calcOptimalSize(min, step);
          } else {
            fixedSize = sizeIn;
          }

          if(id == "y") {
            elementToWork.spaceY = fixedSize;
          }
          if(id == "x") {
            elementToWork.spaceX = fixedSize;
          }
          if(id == "z") {
          }
        }
      }
      elementToWork.axes = axesString;

      if(input.contains("data")) {
        auto data              = input["data"];    //  "axes": "bcyx"
        elementToWork.dataType = dataTypeToEnum(data["type"]);
      } else {
        elementToWork.dataType = settings::AiClassifierSettings::NetInputDataType::FLOAT32;
      }

      // auto preprocessing = input["preprocessing"];
      // for(const auto &command : preprocessing) {
      //   std::string name = command["name"];
      // }
    }
  }

  // =======================================
  // Output information
  // =======================================
  auto outputs = rdfParsed["outputs"];
  if(rdfFormatVersion == "0.4.10") {
    //
    //
    //
    for(const auto &output : outputs) {
      std::string outputName = output["name"];
      std::string axesOrder  = output["axes"];    //  "axes": "bcyx"
      // auto netOutputType     = dataTypeToEnum(output["data_type"]);

      std::optional<size_t> bPos = getPos(axesOrder, 'b');
      std::optional<size_t> cPos = getPos(axesOrder, 'c');
      std::optional<size_t> xPos = getPos(axesOrder, 'x');
      std::optional<size_t> yPos = getPos(axesOrder, 'y');

      // auto halo     = output["halo"];
      // int32_t haloB = 0;
      // int32_t haloC = 0;
      // int32_t haloX = 0;
      // int32_t haloY = 0;
      // if(cPos.has_value()) {
      //   haloC = halo[cPos.value()];
      // }
      // if(bPos.has_value()) {
      //   haloB = halo[bPos.value()];
      // }
      // if(xPos.has_value()) {
      //   haloX = halo[xPos.value()];
      // }
      // if(yPos.has_value()) {
      //   haloY = halo[yPos.value()];
      // }

      int32_t channelSize  = 1;
      int32_t batchSize    = 1;
      int32_t outputWidth  = 1;
      int32_t outputHeight = 1;
      auto shape           = output["shape"];
      if(shape.is_array()) {
        if(cPos.has_value()) {
          channelSize = shape[cPos.value()];
        }
        if(bPos.has_value()) {
          batchSize = shape[bPos.value()];
        }
        if(xPos.has_value()) {
          outputWidth = shape[xPos.value()];
        }
        if(yPos.has_value()) {
          outputHeight = shape[yPos.value()];
        }

      } else if(shape.is_object()) {
        std::string tensorId = shape["reference_tensor"];
        auto offset          = shape["offset"];
        auto scale           = shape["scale"];

        float offsetB = 0;
        float offsetC = 0;
        float offsetX = 0;
        float offsetY = 0;

        float scaleB = 1;
        float scaleC = 1;
        float scaleX = 1;
        float scaleY = 1;

        if(cPos.has_value()) {
          offsetC = offset[cPos.value()];
          scaleC  = scale[cPos.value()];
        }
        if(bPos.has_value()) {
          offsetB = offset[bPos.value()];
          scaleB  = scale[bPos.value()];
        }
        if(xPos.has_value()) {
          offsetX = offset[xPos.value()];
          scaleX  = scale[xPos.value()];
        }
        if(yPos.has_value()) {
          offsetY = offset[yPos.value()];
          scaleY  = scale[yPos.value()];
        }

        // Calc output
        channelSize  = static_cast<int32_t>(static_cast<float>(response.inputs.at(tensorId).channels) * scaleC + offsetC);
        batchSize    = static_cast<int32_t>(static_cast<float>(response.inputs.at(tensorId).batch) * scaleB + offsetB);
        outputWidth  = static_cast<int32_t>(static_cast<float>(response.inputs.at(tensorId).spaceX) * scaleX + offsetX);
        outputHeight = static_cast<int32_t>(static_cast<float>(response.inputs.at(tensorId).spaceY) * scaleY + offsetY);
      }

      response.outputs.emplace(outputName,
                               settings::AiClassifierSettings::NetOutputParameters{
                                   .axes = axesOrder, .batch = batchSize, .channels = channelSize, .spaceX = outputWidth, .spaceY = outputHeight});
    }

  } else if(rdfFormatVersion == "0.5.3") {
    for(const auto &output : outputs) {
      if(output.contains("description")) {
        std::string description = output["description"];
      }
      std::string idOut = output["id"];

      response.outputs.emplace(idOut, settings::AiClassifierSettings::NetOutputParameters{});
      auto &objectToWorkOn = response.outputs.at(idOut);

      auto axes = output["axes"];    //  "axes": "bcyx"
      std::string axesString;
      for(const auto &axe : axes) {
        std::string type = axe["type"];
        if(type == "batch") {
          axesString += "b";
          objectToWorkOn.batch = 1;
          if(axe.contains("size")) {
            objectToWorkOn.batch = axe["size"];
          }
        }
        if(type == "channel") {
          axesString += "c";
          auto channelNames = axe["channel_names"];
          std::vector<std::string> channelNamesOut;
          for(const auto &channelName : channelNames) {
            channelNamesOut.emplace_back(channelName);
          }
          /// How to get correct channel size
          objectToWorkOn.channels = static_cast<int32_t>(channelNamesOut.size());
        }
        if(type == "space") {
          std::string id = axe["id"];
          axesString += id;
          auto sizeIn = axe["size"];
          if(sizeIn.is_object()) {
            // int32_t halo         = axe["halo"];
            float scale          = axe["scale"];
            std::string tensorId = sizeIn["tensor_id"];
            std::string axisId   = sizeIn["axis_id"];
            float offset         = 0;
            if(sizeIn.contains("offset")) {
              offset = sizeIn["offset"];
            }

            if(id == "y") {
              objectToWorkOn.spaceY = static_cast<int32_t>(static_cast<float>(response.inputs.at(tensorId).spaceY) * scale + offset);
            }
            if(id == "x") {
              objectToWorkOn.spaceX = static_cast<int32_t>(static_cast<float>(response.inputs.at(tensorId).spaceX) * scale + offset);
            }
            if(id == "z") {
            }
          } else {
            if(id == "y") {
              objectToWorkOn.spaceY = sizeIn;
            }
            if(id == "x") {
              objectToWorkOn.spaceX = sizeIn;
            }
            if(id == "z") {
            }
          }
        }
        if(type == "index") {
          std::string id = axe["id"];
          axesString += id;
          // float scale = axe["scale"];
          auto sizeIn = axe["size"];
          if(sizeIn.is_object()) {
            std::string tensorId = sizeIn["tensor_id"];
            std::string axisId   = sizeIn["axis_id"];
            // int32_t offset       = size["offset"];
          } else {
            int32_t idx          = sizeIn;
            objectToWorkOn.index = idx;
          }
        }
      }
      objectToWorkOn.axes = axesString;
    }
  }

  // =======================================
  // Try to detect model architecture
  // =======================================
  if(response.modelParameter.modelFormat == settings::AiClassifierSettings::ModelFormat::ONNX) {
    auto result = getONNXModelOutputClasses(response.modelPath);
    response.classes.clear();
    for(auto const &[idx, value] : result) {
      response.classes.push_back(value);
    }
  }

  auto description = helper::toLower(response.description);
  if(tags.contains("cyto3") || tags.contains("cellpose")) {
    response.modelParameter.modelArchitecture = settings::AiClassifierSettings::ModelArchitecture::CYTO3;
  } else if(tags.contains("stardist")) {
    response.modelParameter.modelArchitecture = settings::AiClassifierSettings::ModelArchitecture::STAR_DIST;
  } else if(tags.contains("unet")) {
    response.modelParameter.modelArchitecture = settings::AiClassifierSettings::ModelArchitecture::U_NET;
  } else if(tags.contains("yolov5")) {
    response.modelParameter.modelArchitecture = settings::AiClassifierSettings::ModelArchitecture::YOLO_V5;
  } else if(helper::stringContains(description, "cyto3") || helper::stringContains(description, "cellpose")) {
    response.modelParameter.modelArchitecture = settings::AiClassifierSettings::ModelArchitecture::CYTO3;
  }

  return response;
}

///
/// \brief      ONNX model parser
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::map<int, std::string> AiModelParser::getONNXModelOutputClasses(const std::filesystem::path &modelPath)
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

template <class T>
auto toInputOrder(const std::string &id, const T &in) -> std::string
{
  std::stringstream prefix;
  std::stringstream outTmp;
  prefix << joda::helper::shrinkString(id, 7) << ":\t[";
  outTmp << "[";
  for(size_t idx = 0; idx < in.axes.size(); idx++) {
    char ax = in.axes.at(idx);
    if(outTmp.str().size() > 1) {
      outTmp << " ";
      prefix << " ";
    }
    prefix << ax;
    if('b' == ax) {
      outTmp << std::to_string(in.batch);
    }
    if('c' == ax) {
      outTmp << std::to_string(static_cast<int32_t>(in.channels));
    }
    if('x' == ax) {
      outTmp << std::to_string(in.spaceX);
    }
    if('y' == ax) {
      outTmp << std::to_string(in.spaceY);
    }
  }
  prefix << "] ";
  outTmp << "]";

  std::string outStr = prefix.str() + outTmp.str();
  return outStr;
};

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::string AiModelParser::Data::toString() const
{
  std::stringstream out;

  out << modelName << " v" << version << "\n----\n";

  out << "IN:\n";
  for(const auto &[id, input] : inputs) {
    out << toInputOrder<settings::AiClassifierSettings::NetInputParameters>(id, input) << "\n";
  }
  out << "\n";
  out << "OUT:\n";
  for(const auto &[id, input] : outputs) {
    out << toInputOrder<settings::AiClassifierSettings::NetOutputParameters>(id, input) << "\n";
  }

  out << "\n----\n”";
  for(size_t n = 0; n < authors.size(); n++) {
    const auto &author = authors[n];
    if(!author.affiliation.empty()) {
      out << author.affiliation << "/" << author.authorName;
    } else {
      out << author.authorName;
    }
    if(n + 1 < authors.size()) {
      out << ", ";
    }
  }

  return out.str();
}

}    // namespace joda::ai
