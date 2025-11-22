///
/// \file      image_meta.hpp
/// \author    Joachim Danmayr
/// \date      2025-11-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/fnv1a.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

namespace joda::image_settings {

struct ImageMeta
{
  struct HistogramSettings
  {
    //
    // Image channel for which the histogram settings were taken
    //
    int32_t channel = 0;

    //
    //
    //
    uint16_t lowerLevelContrast = 0;

    //
    //
    //
    uint16_t upperLevelContrast = 0;

    //
    //
    //
    uint16_t lowerRange = 0;

    //
    //
    //
    uint16_t upperRange = 0;

    //
    //
    //
    bool usePseudocolors = true;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(HistogramSettings, channel, lowerLevelContrast, upperLevelContrast, usePseudocolors, lowerRange,
                                                upperRange)
  };

  //
  //
  //
  std::string filePathRelative;

  //
  //
  //
  std::string filePathAbsolute;

  //
  //
  //
  std::vector<HistogramSettings> histogramSettings;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ImageMeta, filePathRelative, filePathAbsolute, histogramSettings);

  //
  //
  //
  std::vector<HistogramSettings> histogramSettingsEditedImage;

  void save(const std::filesystem::path &imageFile, const std::filesystem::path &projectPath)
  {
    if(projectPath.empty()) {
      return;
    }
    filePathRelative  = std::filesystem::relative(imageFile, projectPath).generic_string();
    filePathAbsolute  = imageFile.generic_string();
    auto metaFileName = joda::helper::generateImageMetaDataStoragePathFromImagePath(imageFile, projectPath, joda::fs::FILE_NAME_image_meta + ".json");

    nlohmann::json tmp = *this;
    std::ofstream file(metaFileName.string());
    if(!file) {
      joda::log::logWarning("Could not write image meta file >" + metaFileName.generic_string() + "<!");
    }
    file << tmp.dump(2);
    file.close();
  }

  void open(const std::filesystem::path &imageFile, const std::filesystem::path &projectPath)
  {
    if(projectPath.empty()) {
      return;
    }
    auto metaFileName = joda::helper::generateImageMetaDataStoragePathFromImagePath(imageFile, projectPath, joda::fs::FILE_NAME_image_meta + ".json");

    std::ifstream ifs(metaFileName);
    if(!ifs) {
      return;
    }
    std::string wholeFile;
    size_t size = std::filesystem::file_size(metaFileName);
    wholeFile.resize(size);
    ifs.read(wholeFile.data(), static_cast<std::streamsize>(size));
    ifs.close();

    if(wholeFile.empty()) {
      return;
    }
    try {
      *this = nlohmann::json::parse(wholeFile);
    } catch(...) {
    }
  }

  auto getHistogramSettingsForImageChannel(int32_t channel, bool editedImage) const -> HistogramSettings
  {
    const std::vector<HistogramSettings> *tmp = &histogramSettings;
    if(editedImage) {
      tmp = &histogramSettingsEditedImage;
    }

    for(const auto &hist : *tmp) {
      if(hist.channel == channel) {
        return hist;
      }
    }

    return {};
  }

  void setHistogramSetingsForChannel(const HistogramSettings &histIn, bool editedImage)
  {
    std::vector<HistogramSettings> *tmp = &histogramSettings;
    if(editedImage) {
      tmp = &histogramSettingsEditedImage;
    }

    for(auto &hist : *tmp) {
      if(hist.channel == histIn.channel) {
        hist = histIn;
        return;
      }
    }
    tmp->emplace_back(histIn);
  }
};

}    // namespace joda::image_settings
