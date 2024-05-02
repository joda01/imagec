

#include "reporting_overview_xlsx.hpp"
#include <xlsxwriter/worksheet.h>
#include <string>
#include <unordered_map>
#include "backend/image_processing/roi/roi.hpp"
#include "backend/results/results_defines.hpp"
#include "xlsxwriter.h"

namespace joda::pipeline::reporting {

///
/// \brief      Writes the report to a CSV file
/// \author     Joachim Danmayr
/// \param[in]  fileName  Name of the output report file
///
std::tuple<int, int> OverviewReport::writeReport(const joda::settings::ChannelReportingSettings &reportingSettings,
                                                 const joda::results::Table &results, const std::string &headerText,
                                                 const std::string &jobName, int colOffset, int rowOffset, int startRow,
                                                 lxw_worksheet *worksheet, lxw_format *header,
                                                 lxw_format *headerInvalid, lxw_format *merge_format,
                                                 lxw_format *numberFormat, lxw_format *numberFormatInvalid,
                                                 lxw_format *imageHeaderHyperlinkFormat,
                                                 lxw_format *imageHeaderHyperlinkFormatInvalid)
{
  setlocale(LC_NUMERIC, "C");                  // Needed for correct comma in libxlsx
  const int STATISTIC_START_WITH_INDEX = 3;    // Validity, invalidity and Sum are just for internal use

  //
  // Sort rows
  //
  std::map<std::string, uint64_t> sortedRow;
  for(const auto &[idx, name] : results.getRowNames()) {
    sortedRow.emplace(name, idx);
  }

  std::vector<uint64_t> indexMapping(sortedRow.size());
  uint64_t cnt = 0;
  for(const auto &[_, idx] : sortedRow) {
    indexMapping[cnt] = idx;
    cnt++;
  }

  auto getIndexOfSortedMap = [&indexMapping](int idx) { return indexMapping[idx]; };

  //
  //
  //
  bool WRITE_HEADER_FOR_EACH_CHANNEL = false;

  colOffset = 2;

  int64_t nrOfColumns = results.getNrOfColumns();

  //
  // Write header text
  //
  if(rowOffset == startRow) {
    worksheet_merge_range(worksheet, rowOffset, 0, rowOffset, 1, "-", merge_format);
    worksheet_write_string(worksheet, rowOffset, 0, headerText.data(), header);
  }

  //
  // Write statistics data
  //
  {
    int sheetColumnIdx = 0;
    int sheetRowIdx    = 0;
    for(int statColIdx = STATISTIC_START_WITH_INDEX; statColIdx < results::Statistics::NR_OF_VALUE; statColIdx++) {
      sheetRowIdx = 0;
      if(rowOffset == startRow || WRITE_HEADER_FOR_EACH_CHANNEL) {
        worksheet_write_string(worksheet, rowOffset, sheetColumnIdx + colOffset,
                               results::Statistics::getStatisticsTitle()[statColIdx].data(), header);
        worksheet_set_column(worksheet, sheetColumnIdx + colOffset, sheetColumnIdx + colOffset, 15, NULL);
      }

      for(int64_t colIdx = 0; colIdx < nrOfColumns; colIdx++) {
        auto colKey = joda::results::getMeasureChannelWithStats(results.getColumnKeyAt(colIdx));
        if(reportingSettings.overview.measureChannels.contains(colKey)) {
          if(results.getStatistics().contains(colIdx)) {
            auto statistics = results.getStatistics().at(colIdx);

            worksheet_write_number(worksheet, rowOffset + sheetRowIdx + 1, sheetColumnIdx + colOffset,
                                   statistics.getStatistics()[statColIdx], numberFormat);

          } else {
            // No statistics for that
            worksheet_write_number(worksheet, rowOffset + sheetRowIdx + 1, sheetColumnIdx + colOffset, 0, numberFormat);
          }
          sheetRowIdx++;
        }
      }
      sheetColumnIdx++;
    }
  }

  colOffset += (results::Statistics::NR_OF_VALUE - STATISTIC_START_WITH_INDEX + 1);

  //
  // Write image header
  //
  //
  int headerColumnRowOffset = rowOffset;
  if(headerColumnRowOffset == startRow || WRITE_HEADER_FOR_EACH_CHANNEL) {
    for(int32_t rowIndex = 0; rowIndex < results.getNrOfRows(); rowIndex++) {
      auto rowName         = results.getRowNameAt(getIndexOfSortedMap(rowIndex));
      std::string filePath = "external:.\\images/" + rowName + "/results_image_" + jobName + ".xlsx";
      lxw_format *format   = imageHeaderHyperlinkFormat;
      worksheet_write_url(worksheet, headerColumnRowOffset, rowIndex + colOffset, filePath.data(), format);
      worksheet_write_string(worksheet, headerColumnRowOffset, rowIndex + colOffset, rowName.data(), format);

      worksheet_set_column(worksheet, rowIndex + colOffset, rowIndex + colOffset, 15, NULL);
    }
  }

  rowOffset++;

  //
  // Write Result header
  //
  int nrOfRowsWritten = 0;    // reportingSettings.getOverviewReportSettings().getMeasurementChannels().size();

  {
    int sheetRowIdx = 0;
    for(int64_t colIndex = 0; colIndex < nrOfColumns; colIndex++) {
      auto colKey = joda::results::getMeasureChannelWithStats(results.getColumnKeyAt(colIndex));
      if(reportingSettings.overview.measureChannels.contains(colKey)) {
        worksheet_write_string(worksheet, sheetRowIdx + rowOffset, 1, results.getColumnNameAt(colIndex).data(), header);
        worksheet_set_column(worksheet, 1, 1, 20, NULL);
        sheetRowIdx++;
      }
    }
    nrOfRowsWritten = sheetRowIdx;
  }

  //
  // Write Table Name
  //
  //
  worksheet_merge_range(worksheet, rowOffset, 0, rowOffset + nrOfRowsWritten - 1, 0, "-", merge_format);
  if(!results.getTableName().empty()) {
    worksheet_write_string(worksheet, rowOffset, 0, results.getTableName().data(), merge_format);
  } else {
    worksheet_write_string(worksheet, rowOffset, 0, std::to_string(rowOffset).data(), merge_format);
  }

  //
  // Write table data
  //
  {
    int sheetRowIdx = 0;
    for(int64_t rowIndex = 0; rowIndex < results.getNrOfRows(); rowIndex++) {
      sheetRowIdx = 0;
      for(int64_t colIndex = 0; colIndex < nrOfColumns; colIndex++) {
        auto colKey = joda::results::getMeasureChannelWithStats(results.getColumnKeyAt(colIndex));
        if(reportingSettings.overview.measureChannels.contains(colKey)) {
          if(results.getTable().contains(colIndex) &&
             results.getTable().at(colIndex).contains(getIndexOfSortedMap(rowIndex))) {
            auto dataToWrite = results.getTable().at(colIndex).at(getIndexOfSortedMap(rowIndex));
            auto *format     = numberFormat;
            if(!dataToWrite.isValid) {
              format = numberFormatInvalid;
            }
            if(std::holds_alternative<double>(dataToWrite.val)) {
              worksheet_write_number(worksheet, rowOffset + sheetRowIdx, rowIndex + colOffset,
                                     std::get<double>(dataToWrite.val), format);

            } else if(std::holds_alternative<joda::func::ParticleValidity>(dataToWrite.val)) {
              worksheet_write_string(
                  worksheet, rowOffset + sheetRowIdx, rowIndex + colOffset,
                  results::Table::validityToString(std::get<joda::func::ParticleValidity>(dataToWrite.val)).data(),
                  NULL);
            }
            sheetRowIdx++;
          } else {
            // Empty table entry
          }
        }
      }
    }
  }

  rowOffset = rowOffset + nrOfRowsWritten;

  return {colOffset, rowOffset};
}
}    // namespace joda::pipeline::reporting
