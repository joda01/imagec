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
#include "backend/helper/database/plugins/helper.hpp"
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

  QueryFilter queryFilter;
  ExportType exportType;
  ExportDetail exportDetail;
};
}    // namespace joda::db
