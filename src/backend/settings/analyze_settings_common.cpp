
#include "analyze_settings_common.hpp"

namespace joda::settings::json {

void AnalyzeSettingsReportingHeatmap::postParsing()
{
  if(group_by == "FOLDER") {
    group_by_enum = GroupBy::FOLDER;
  } else if(group_by == "FILENAME") {
    group_by_enum = GroupBy::FILENAME;
  } else {
    group_by_enum = GroupBy::OFF;
  }
}

}    // namespace joda::settings::json
