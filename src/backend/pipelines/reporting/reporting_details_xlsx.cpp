

#include <xlsxwriter/worksheet.h>
#include <string>
#include "backend/pipelines/reporting/reporting_defines.hpp"
#include "reporting_details.xlsx.hpp"

namespace joda::pipeline::reporting {

///
/// \brief      Writes the report to a CSV file
/// \author     Joachim Danmayr
/// \param[in]  fileName  Name of the output report file
///
std::tuple<int, int> DetailReport::writeReport(const joda::settings::ChannelReportingSettings &reportingSettings,
                                               const joda::results::Table &results, int colOffset, int /*rowOffset*/,
                                               lxw_worksheet *worksheet, lxw_format *header, lxw_format *merge_format,
                                               lxw_format *numberFormat)
{
  setlocale(LC_NUMERIC, "C");    // Needed for correct comma in libxlsx
  int ROW_OFFSET                       = 2;
  int COL_OFFSET                       = colOffset + 1;
  const int STATISTIC_START_WITH_INDEX = 2;    // Validity, invalidity and Sum are just for internal use

  //
  // Write header
  //
  uint32_t nrOfWrittenCols = 0;
  {
    uint32_t sheetColIdx = 0;
    for(int64_t colIdx = 0; colIdx < results.getNrOfColumns(); colIdx++) {
      auto colKey = getMeasureChannel(results.getColumnKeyAt(colIdx));
      if(reportingSettings.detail.measureChannels.contains(colKey)) {
        worksheet_write_string(worksheet, 1, sheetColIdx + COL_OFFSET, results.getColumnNameAt(colIdx).data(), header);
        worksheet_set_column(worksheet, sheetColIdx + COL_OFFSET, sheetColIdx + COL_OFFSET, 10, NULL);
        sheetColIdx++;
      }
    }
    nrOfWrittenCols = sheetColIdx;
  }

  worksheet_set_column(worksheet, 0, 0, 10, NULL);

  //
  // Write name
  //
  worksheet_merge_range(worksheet, 0, COL_OFFSET, 0, COL_OFFSET + nrOfWrittenCols - 1, "-", merge_format);
  if(!results.getTableName().empty()) {
    worksheet_write_string(worksheet, 0, COL_OFFSET, results.getTableName().data(), header);
  } else {
    worksheet_write_string(worksheet, 0, COL_OFFSET, std::to_string(COL_OFFSET).data(), header);
  }

  //
  // Write statistic data
  //
  int rowIdxStat = STATISTIC_START_WITH_INDEX;
  for(rowIdxStat = STATISTIC_START_WITH_INDEX; rowIdxStat < results::Statistics::NR_OF_VALUE; rowIdxStat++) {
    worksheet_write_string(worksheet, ROW_OFFSET + rowIdxStat - STATISTIC_START_WITH_INDEX, 0,
                           results::Statistics::getStatisticsTitle()[rowIdxStat].data(), header);

    uint32_t sheetColIdx = 0;
    for(int64_t colIdx = 0; colIdx < results.getNrOfColumns(); colIdx++) {
      auto colKey = getMeasureChannel(results.getColumnKeyAt(colIdx));
      if(reportingSettings.detail.measureChannels.contains(colKey)) {
        if(results.getStatistics().contains(colIdx)) {
          auto statistics = results.getStatistics().at(colIdx);

          worksheet_write_number(worksheet, ROW_OFFSET + rowIdxStat - STATISTIC_START_WITH_INDEX,
                                 sheetColIdx + COL_OFFSET, statistics.getStatistics()[rowIdxStat], numberFormat);

        } else {
          // No statistics for that
          worksheet_write_string(worksheet, ROW_OFFSET + rowIdxStat - STATISTIC_START_WITH_INDEX,
                                 sheetColIdx + COL_OFFSET, "-", numberFormat);
        }
        sheetColIdx++;
      }
    }
  }
  ROW_OFFSET = ROW_OFFSET += (rowIdxStat + 2 - STATISTIC_START_WITH_INDEX);

  //
  // Write table data
  //
  {
    int64_t rowIdx = 0;
    for(rowIdx = 0; rowIdx < results.getNrOfRows(); rowIdx++) {
      uint32_t sheetColIdx = 0;
      for(int64_t colIdx = 0; colIdx < results.getNrOfColumns(); colIdx++) {
        auto colKey = getMeasureChannel(results.getColumnKeyAt(colIdx));
        if(reportingSettings.detail.measureChannels.contains(colKey)) {
          if(results.getStatistics().contains(colIdx)) {
            //
            // Write row data
            //
            if(0 == colIdx) {
              //
              // Write row header
              //
              worksheet_write_string(worksheet, ROW_OFFSET + rowIdx, colIdx, results.getRowNameAt(rowIdx).data(),
                                     header);
            }

            if(results.getTable().contains(colIdx) && results.getTable().at(colIdx).contains(rowIdx)) {
              if(!results.getTable().at(colIdx).at(rowIdx).validity.has_value()) {
                worksheet_write_number(worksheet, ROW_OFFSET + rowIdx, sheetColIdx + COL_OFFSET,
                                       results.getTable().at(colIdx).at(rowIdx).value, numberFormat);
              } else {
                worksheet_write_string(
                    worksheet, ROW_OFFSET + rowIdx, colIdx + COL_OFFSET,
                    results::Table::validityToString(results.getTable().at(colIdx).at(rowIdx).validity.value()).data(),
                    NULL);
              }
            } else {
              // Empty table entry
              worksheet_write_string(worksheet, ROW_OFFSET + rowIdx, sheetColIdx + COL_OFFSET, "-", numberFormat);
            }
          }
          sheetColIdx++;
        }
      }
    }
  }

  return {nrOfWrittenCols, 2};
}
}    // namespace joda::pipeline::reporting
