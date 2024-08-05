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
#include <cstdint>
#include <filesystem>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>
#include "backend/helper/ome_parser/ome_info.hpp"
#include "backend/image_processing/detection/detection_response.hpp"
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
  std::string imageFileNameRegex;
  joda::settings::ExperimentSettings::GroupBy groupBy;
  uint8_t plateIdx    = 0;
  uint16_t plateRowNr = 0;
  uint16_t plateColNr = 0;
};

struct WellPosition
{
  uint16_t wellPosX = 0;
  uint16_t wellPosY = 0;
  uint32_t imageIdx = 0;
  std::string toString() const
  {
    char al = (wellPosY - 1 + 'A');
    return std::string(1, al) + "x" + std::to_string(wellPosX);
  }
};

struct GroupInformation
{
  std::string groupName;
  WellPosition well;
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
  struct DetailReportAdder
  {
    std::shared_ptr<duckdb::Connection> connection;
    std::shared_ptr<duckdb::Appender> objects;
    std::shared_ptr<duckdb::Appender> imageStats;
  };

  Results(const std::filesystem::path &pathToRawData, const ExperimentSetting &settings,
          const joda::settings::AnalyzeSettings &analyzeSettings);

  void appendChannelsToDetailReport(const joda::settings::AnalyzeSettings &);
  void appendImageToDetailReport(const joda::ome::OmeInfo &imgProps, const std::filesystem::path &imagePath);

  auto prepareDetailReportAdding() -> DetailReportAdder;
  void appendToDetailReport(const DetailReportAdder &, const joda::image::detect::DetectionResponse &result,
                            const joda::settings::ChannelSettingsMeta &channelSettings,
                            const joda::ome::TileToLoad &tileIdx, const joda::ome::OmeInfo &imgProps,
                            const std::filesystem::path &imagePath);
  void writePredatedData(const DetailReportAdder &);

  /////////////////////////////////////////////////////
  static GroupInformation applyRegex(const std::string &regex, const std::filesystem::path &imagePath);
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
                                           const joda::ome::OmeInfo &imgProps, const std::filesystem::path &imagePath);
  void prepareOutputFolders(const std::filesystem::path &resultsFolder);

  /////////////////////////////////////////////////////
  ExperimentSetting mExperimentSettings;
  std::filesystem::path mPathToRawData;
  std::filesystem::path mOutputFolder;
  std::filesystem::path mDatabaseFileName;
  std::filesystem::path mOutputFolderImages;
  std::shared_ptr<joda::results::db::Database> mDatabase;
  std::string mAnalyzeId;

  class WellPosGenerator
  {
  public:
    struct Pos
    {
      std::string groupName;
      uint16_t groupId = 0;
      uint32_t imgIdx  = 0;
      uint16_t x       = UINT16_MAX;
      uint16_t y       = UINT16_MAX;
    };

    Pos getGroupId(const GroupInformation &groupInfo)
    {
      // This group still exists
      if(mGroups.contains(groupInfo.groupName)) {
        auto newPos = mGroups.at(groupInfo.groupName);
        if(groupInfo.well.imageIdx == UINT32_MAX) {
          newPos.imgIdx = nextFreeImgIdx();
        } else {
          newPos.imgIdx = groupInfo.well.imageIdx;
        }
        return newPos;
      }
      Pos newPos;
      // This group does not yet exist
      if(groupInfo.well.wellPosX == UINT16_MAX || groupInfo.well.wellPosY == UINT16_MAX) {
        newPos = Pos{.groupName = groupInfo.groupName,
                     .groupId   = actGroupId,
                     .x         = static_cast<uint8_t>((actWellPos % MAX_COLS) + 1),
                     .y         = static_cast<uint8_t>((actWellPos / MAX_COLS) + 1)};
      } else {
        newPos = Pos{.groupName = groupInfo.groupName,
                     .groupId   = actGroupId,
                     .x         = groupInfo.well.wellPosX,
                     .y         = groupInfo.well.wellPosY};
      }

      if(groupInfo.well.imageIdx == UINT32_MAX) {
        newPos.imgIdx = nextFreeImgIdx();
      } else {
        newPos.imgIdx = groupInfo.well.imageIdx;
      }

      actWellPos++;
      actGroupId++;

      mGroups.emplace(groupInfo.groupName, newPos);

      return newPos;
    }

    uint32_t nextFreeImgIdx()
    {
      uint32_t toReturn = imgIdx;
      imgIdx++;
      return toReturn;
    }

  private:
    /////////////////////////////////////////////////////
    static const inline int32_t MAX_COLS = 24;
    static const inline int32_t MAX_ROWS = 16;

    std::map<std::string, Pos> mGroups;
    uint32_t actWellPos = 0;
    uint32_t imgIdx     = 1;
    uint16_t actGroupId = 0;
  };
  WellPosGenerator mWellPosGenerator;
  std::mutex mWellGeneratorLock;
  std::mutex mAppenderMutex;
};

}    // namespace joda::results
