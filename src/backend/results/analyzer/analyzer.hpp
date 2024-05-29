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
#include "backend/results/database/database_interface.hpp"
#include "backend/results/db_column_ids.hpp"
#include "backend/results/table/table.hpp"
#include "backend/settings/channel/channel_index.hpp"
#include "backend/settings/channel/channel_settings.hpp"
#include "backend/settings/channel/channel_settings_meta.hpp"

namespace joda::results {

///
/// \class      Results
/// \author     Joachim Danmayr
/// \brief      This is a wrapper class used to manage the results of one job
///             Create one instance of this class per job
///
class Analyzer
{
public:
  explicit Analyzer(const std::filesystem::path &databasePath);
  auto getAbsolutePathToControlImage(const std::string &relativePath) const -> std::filesystem::path;
  auto getAnalyzes() -> std::vector<db::AnalyzeMeta>;
  auto getImagesForAnalyses(const std::string &analyzeId) -> std::vector<db::ImageMeta>;
  auto getImageInformation(const std::string &analyzeId, uint8_t plateId, ChannelIndex channel, uint64_t imageId)
      -> std::tuple<db::ImageMeta, db::ChannelMeta, db::ImageChannelMeta>;
  auto getChannelsForAnalyses(const std::string &analyzeId) -> std::vector<db::ChannelMeta>;
  auto getPlatesForAnalyses(const std::string &analyzeId) -> std::vector<db::PlateMeta>;
  auto getWellsForPlate(const std::string &analyzeId, uint8_t plateId) -> std::vector<db::WellMeta>;
  auto getWellInformation(const std::string &analyzeId, uint8_t plateId, ChannelIndex channel, WellId wellId)
      -> std::tuple<db::WellMeta, db::ChannelMeta>;

  void markImageChannelAsManualInvalid(const std::string &analyzeId, uint8_t plateId, ChannelIndex channel,
                                       uint64_t imageId);
  void unMarkImageChannelAsManualInvalid(const std::string &analyzeId, uint8_t plateId, ChannelIndex channel,
                                         uint64_t imageId);

  auto getDatabase() -> joda::results::db::Database &
  {
    return mDatabase;
  }

private:
  static constexpr uint32_t BITSET_OFFSET = 31;
  joda::results::db::Database mDatabase;
  std::filesystem::path mParentPathToDb;
};

}    // namespace joda::results
