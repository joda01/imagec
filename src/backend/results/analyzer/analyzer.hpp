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
  Analyzer(const std::filesystem::path &databasePath);
  auto getImagesForJob(const std::string &jobId) -> std::vector<db::ImageMeta>;
  auto getChannelsForImage(const std::string &jobId, uint32_t imageId) -> std::vector<db::ChannelMeta>;

private:
  joda::results::db::Database mDatabase;
};

}    // namespace joda::results
