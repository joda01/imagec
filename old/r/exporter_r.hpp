
#pragma once

#include <sys/types.h>
#include <xlsxwriter/workbook.h>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"

#include "backend/database/database.hpp"
#include "backend/database/query/filter.hpp"
#include "backend/database/query/query_for_well.hpp"
#include "backend/helper/table/table.hpp"
#include "backend/settings/analze_settings.hpp"

namespace joda::db {

class RExporter
{
public:
  static void startExport(const settings::ResultsSettings &filter, db::StatsPerGroup::Grouping grouping,
                          const settings::AnalyzeSettings &analyzeSettings, const std::string &jobName,
                          std::chrono::system_clock::time_point timeStarted, std::chrono::system_clock::time_point timeFinished,
                          std::string outputFileName);

private:
};

}    // namespace joda::db
