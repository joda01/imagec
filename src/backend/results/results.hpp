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
#include <mutex>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/image_processing/reader/image_reader.hpp"
#include "backend/results/database/database.hpp"
#include "backend/results/db_column_ids.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/channel/channel_index.hpp"
#include "backend/settings/channel/channel_settings.hpp"
#include "backend/settings/channel/channel_settings_meta.hpp"
#include "backend/settings/experiment_settings.hpp"
#include <duckdb/main/appender.hpp>

namespace joda::results {

struct ExperimentSetting
{
  std::string runId;
  std::string analyzeName;
  std::string scientistName;
  std::string imageFileNameRegex;
  joda::settings::ExperimentSettings::GroupBy groupBy;
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

  auto prepareDetailReportAdding()
      -> std::tuple<std::shared_ptr<duckdb::Appender>, std::shared_ptr<duckdb::Connection>>;
  void appendToDetailReport(std::shared_ptr<duckdb::Appender>, const joda::image::detect::DetectionResponse &result,
                            const joda::settings::ChannelSettingsMeta &channelSettings, uint16_t tileIdx,
                            const image::ImageProperties &imgProps, const std::filesystem::path &imagePath);
  void writePredatedData(std::shared_ptr<duckdb::Appender>, std::shared_ptr<duckdb::Connection>);

  /////////////////////////////////////////////////////
  static std::tuple<WellId, std::string> applyRegex(const std::string &regex, const std::filesystem::path &imagePath);
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

  void createWellPositionBasesOnGroupName(WellId &wellInOut, const std::string &groupName);

  /////////////////////////////////////////////////////
  ExperimentSetting mExperimentSettings;
  std::filesystem::path mPathToRawData;
  std::filesystem::path mOutputFolder;
  std::filesystem::path mDatabaseFileName;
  std::filesystem::path mOutputFolderImages;
  std::shared_ptr<joda::results::db::Database> mDatabase;
  std::string mAnalyzeId;

  struct WellPosGenerator
  {
    static const inline int32_t MAX_COLS = 24;
    static const inline int32_t MAX_ROWS = 16;
    struct Pos
    {
      uint8_t x = UINT8_MAX;
      uint8_t y = UINT8_MAX;
    };
    std::map<std::string, Pos> mNameToWellId;
    uint32_t actWellPos = 0;
    uint32_t imgIdx     = 1;

    Pos nextFreeWell()
    {
      auto newPos = Pos{.x = static_cast<uint8_t>((actWellPos % MAX_COLS) + 1),
                        .y = static_cast<uint8_t>((actWellPos / MAX_COLS) + 1)};
      if(newPos.x * newPos.y >= MAX_COLS * MAX_ROWS) {
        throw std::runtime_error("Too many groups!");
      }
      actWellPos++;
      return newPos;
    }

    uint32_t nextFreeImgIdx()
    {
      uint32_t toReturn = imgIdx;
      imgIdx++;
      return toReturn;
    }
  };
  WellPosGenerator mWellPosGenerator;
  std::mutex mWellGeneratorLock;
  std::mutex mAppenderMutex;
};

}    // namespace joda::results
