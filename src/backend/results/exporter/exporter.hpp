
#pragma once

#include <sys/types.h>
#include <xlsxwriter/workbook.h>
#include <cstdint>
#include "backend/results/analyzer/analyzer.hpp"
#include "backend/results/analyzer/plugins/heatmap_for_plate.hpp"
#include "backend/results/analyzer/plugins/stats_for_image.hpp"
#include "backend/results/db_column_ids.hpp"

namespace joda::results::exporter {

class BatchExporter
{
public:
  struct Settings
  {
    struct Channel
    {
      std::string name;
      std::map<joda::results::MeasureChannelId, Stats> measureChannels;
    };
    std::map<ChannelIndex, Channel> imageChannels;
    Analyzer &analyzer;
    uint8_t plateId;
    uint16_t plateRows;
    uint16_t plarteCols;
  };

  static void startExport(const Settings &settings, const std::string &outputFileName);

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
    u_int32_t row = 0;
    u_int32_t col = 0;
  };
  static WorkBook createWorkBook(std::string outputFileName);

  static void paintPlateBorder(lxw_worksheet *sheet, int64_t rows, int64_t cols, int32_t rowOffset, lxw_format *header,
                               lxw_format *numberFormat, lxw_format *mergeFormat, const std::string &title);

  static Pos paintHeatmap(const WorkBook &workbookSettings, lxw_worksheet *worksheet, const joda::results::Table &table,
                          uint32_t rowOffset);
};

}    // namespace joda::results::exporter
