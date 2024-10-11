
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

class BatchExporter
{
public:
  struct Settings
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

  static void startExport(const Settings &settings, const settings::AnalyzeSettings &analyzeSettings, const std::string &jobName,
                          std::chrono::system_clock::time_point timeStarted, std::chrono::system_clock::time_point timeFinished,
                          const std::string &outputFileName);

private:
  /////////////////////////////////////////////////////

  struct WorkBook
  {
    lxw_workbook *workbook;
    lxw_format *header;
    lxw_format *headerInvalid;
    lxw_format *imageHeaderHyperlinkFormat;
    lxw_format *imageHeaderHyperlinkFormatInvalid;
    lxw_format *merge_format;
    lxw_format *headerBold;
    lxw_format *fontNormal;
    lxw_format *numberFormat;
    lxw_format *numberFormatInvalid;
    lxw_format *numberFormatScientific;
    lxw_format *numberFormatInvalidScientific;
  };

  struct Pos
  {
    uint32_t row = 0;
    uint32_t col = 0;
  };
  static WorkBook createWorkBook(std::string outputFileName);
  static void createAnalyzeSettings(WorkBook &, const settings::AnalyzeSettings &settings, const std::string &jobName,
                                    std::chrono::system_clock::time_point timeStarted, std::chrono::system_clock::time_point timeFinished);
  static void createHeatmapSummary(WorkBook &, const Settings &settings);
  static void createListSummary(WorkBook &workbookSettings, const Settings &settings);
  static void paintPlateBorder(lxw_worksheet *sheet, int64_t rows, int64_t cols, int32_t rowOffset, lxw_format *header, lxw_format *numberFormat,
                               lxw_format *mergeFormat, const std::string &title);
  static Pos paintHeatmap(const WorkBook &workbookSettings, lxw_worksheet *worksheet, const joda::table::Table &table, uint32_t rowOffset);
};

}    // namespace joda::db
