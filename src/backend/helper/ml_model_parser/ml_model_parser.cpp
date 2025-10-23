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

#include "ml_model_parser.hpp"
#include <qdir.h>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include "backend/commands/classification/pixel_classifier/pixel_classifier_store_model.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/rapidyaml/rapidyaml.hpp"
#include "backend/helper/system/directories.hpp"
#include <nlohmann/json_fwd.hpp>
#include <pugixml.hpp>

namespace joda::ml {

///
/// \brief      Save template in users home directory
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto MlModelParser::getUsersMlModelDirectory(const std::filesystem::path &workingDirectory) -> std::filesystem::path
{
  auto homeDir = workingDirectory / joda::fs::WORKING_DIRECTORY_MODELS_PATH;

  if(!fs::exists(homeDir) || !fs::is_directory(homeDir)) {
    try {
      fs::create_directories(homeDir);
    } catch(const fs::filesystem_error &e) {
      joda::log::logError("Cannot create models directory!");
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
auto MlModelParser::getGlobalMlModelDirectory() -> std::filesystem::path
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
auto MlModelParser::findMlModelFiles(const std::filesystem::path &workingDirectory) -> std::map<std::filesystem::path, Data>
{
  std::lock_guard<std::mutex> lock(lookForMutex);
  std::map<std::filesystem::path, Data> aiModelFiles;
  std::vector<std::filesystem::path> directories{getGlobalMlModelDirectory(), getUsersMlModelDirectory(workingDirectory)};

  for(const auto &directory : directories) {
    if(fs::exists(directory) && fs::is_directory(directory)) {
      for(const auto &entry : fs::recursive_directory_iterator(directory)) {
        if(entry.is_regular_file()) {
          try {
            if(entry.path().string().ends_with(joda::fs::MASCHINE_LEARNING_OPCEN_CV_XML_MODEL)) {
              const auto relativePath = std::filesystem::relative(entry.path(), workingDirectory);
              std::cout << "Rel path " << relativePath.string() << std::endl;
              auto modelInfo = parseOpenCVModelXMLDescriptionFile(relativePath, workingDirectory);

              aiModelFiles.emplace(relativePath, modelInfo);
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

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto MlModelParser::parseOpenCVModelXMLDescriptionFile(const std::filesystem::path &modelFile, const std::filesystem::path &workingDirectory) -> Data
{
  // Open file stream
  std::ifstream file(std::filesystem::weakly_canonical(workingDirectory / modelFile).string());
  if(!file.is_open()) {
    return {};
  }
  joda::ml::PixelClassifierModel parsedModel = nlohmann::json::parse(file);
  file.close();
  Data info;
  info.modelPath = modelFile;
  info.modelName = modelFile.filename().string();
  if(!parsedModel.meta.organization.value_or("").empty() || !parsedModel.meta.author.value_or("").empty()) {
    info.authors.emplace_back(
        Data::Author{.affiliation = parsedModel.meta.organization.value_or(""), .authorName = parsedModel.meta.author.value_or("")});
  }
  for(const auto &item : parsedModel.classLabels) {
    info.classes.emplace_back("CL" + std::to_string(item.classId));
  }
  return info;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::string MlModelParser::Data::toString() const
{
  std::stringstream out;

  out << modelName << " " << version << "\n----\n";

  if(!authors.empty()) {
    out << "\n----\n";
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
  }

  out << "\nPixel classes:\n";
  for(size_t n = 0; n < classes.size(); n++) {
    const auto &classs = classes[n];
    out << classs;
    if(n + 1 < classes.size()) {
      out << ", ";
    }
  }

  return out.str();
}

}    // namespace joda::ml
