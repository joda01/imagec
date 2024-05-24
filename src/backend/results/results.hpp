///
/// \file      results.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <regex.h>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/image_processing/reader/image_reader.hpp"
#include "backend/results/database/database.hpp"
#include "backend/results/db_column_ids.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/channel/channel_index.hpp"
#include "backend/settings/channel/channel_settings.hpp"
#include "backend/settings/channel/channel_settings_meta.hpp"

namespace joda::results {

struct ExperimentSetting
{
  std::string runId;
  std::string analyzeName;
  std::string scientistName;
  std::string imageFileNameRegex;
  uint8_t plateIdx    = 0;
  uint16_t plateRowNr = 0;
  uint16_t plateColNr = 0;
};

///
/// \class      Results
/// \author     Joachim Danmayr
/// \brief      This is a wrapper class used to manage the results of one job
///             Create one instance of this class per job
///
class Results
{
public:
  Results(const std::filesystem::path &pathToRawData, const ExperimentSetting &settings);

  void appendChannelsToDetailReport(const joda::settings::AnalyzeSettings &);
  void appendImageToDetailReport(const image::ImageProperties &imgProps, const std::filesystem::path &imagePath);
  void appendToDetailReport(const joda::image::detect::DetectionResponse &result,
                            const joda::settings::ChannelSettingsMeta &channelSettings, uint16_t tileIdx,
                            const image::ImageProperties &imgProps, const std::filesystem::path &imagePath);

  /////////////////////////////////////////////////////
  static WellId applyRegex(const std::string &regex, const std::filesystem::path &imagePath);
  static uint64_t calcImagePathHash(const std::string &runId, const std::filesystem::path &pathToOriginalImage);

  const std::filesystem::path &getOutputFolder() const
  {
    return mOutputFolder;
  }

private:
  /////////////////////////////////////////////////////
  static inline const std::string DB_FILENAME{"results.duckdb"};
  static inline const std::filesystem::path CONTROL_IMAGE_PATH{"images"};
  static inline const std::string CONTROL_IMAGES_FILE_EXTENSION{".png"};

  /////////////////////////////////////////////////////

  /////////////////////////////////////////////////////
  std::filesystem::path createControlImage(const joda::image::detect::DetectionResponse &result,
                                           const joda::settings::ChannelSettingsMeta &channelSettings, uint16_t tileIdx,
                                           const image::ImageProperties &imgProps,
                                           const std::filesystem::path &imagePath);
  void prepareOutputFolders(const std::filesystem::path &resultsFolder);

  /////////////////////////////////////////////////////
  ExperimentSetting mExperimentSettings;
  std::filesystem::path mPathToRawData;
  std::filesystem::path mOutputFolder;
  std::filesystem::path mDatabaseFileName;
  std::filesystem::path mOutputFolderImages;
  std::shared_ptr<joda::results::db::Database> mDatabase;
  std::string mAnalyzeId;
};

}    // namespace joda::results
