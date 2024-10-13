///
/// \file      export_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <sys/types.h>
#include <xlsxwriter/workbook.h>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/table/table.hpp"
#include "backend/settings/analze_settings.hpp"

namespace joda::db {

struct ExportSettings
{
  enum class ExportType
  {
    HEATMAP,
    TABLE,
    TABLE_DETAIL
  };

  enum class ExportDetail
  {
    PLATE,
    WELL,
    IMAGE
  };

  struct Channel
  {
    std::string clusterName;
    std::string className;
    std::map<enums::Measurement, std::set<enums::Stats>> measureChannels;
    std::map<int32_t, std::string> crossChannelStacksC;
    std::map<settings::ClassificatorSettingOut, std::pair<std::string, std::string>> crossChannelCount;
  };
  std::map<settings::ClassificatorSettingOut, Channel> clustersToExport;
  db::Database &analyzer;
  uint8_t plateId;
  uint16_t groupId;
  uint64_t imageId;
  uint16_t plateRows;
  uint16_t plateCols;
  uint32_t heatmapAreaSize;
  std::vector<std::vector<int32_t>> wellImageOrder;
  ExportType exportType;
  ExportDetail exportDetail;
};
}    // namespace joda::db
