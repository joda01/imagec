

#include "reporting_overview_xlsx.hpp"
#include <xlsxwriter/worksheet.h>
#include <string>
#include <unordered_map>
#include "xlsxwriter.h"

namespace joda::pipeline::reporting {

///
/// \brief      Writes the report to a CSV file
/// \author     Joachim Danmayr
/// \param[in]  fileName  Name of the output report file
///
std::tuple<int, int> OverviewReport::writeReport(const joda::results::Table &results, const std::string &headerText,
                                                 const std::string &jobName, int colOffset, int rowOffset, int startRow,
                                                 lxw_worksheet *worksheet, lxw_format *header, lxw_format *merge_format,
                                                 lxw_format *numberFormat, lxw_format *imageHeaderHyperlinkFormat)
{
  setlocale(LC_NUMERIC, "C");    // Needed for correct comma in libxlsx
  const int STATISTIC_START_WITH_INDEX = 2;

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

  int64_t columns = results.getNrOfColumns();

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
    int colIdx = 0;

    for(int statColIdx = STATISTIC_START_WITH_INDEX; statColIdx < results::Statistics::NR_OF_VALUE; statColIdx++) {
      if(rowOffset == startRow || WRITE_HEADER_FOR_EACH_CHANNEL) {
        worksheet_write_string(worksheet, rowOffset, colIdx + colOffset,
                               results::Statistics::getStatisticsTitle()[statColIdx].data(), header);
        worksheet_set_column(worksheet, colIdx + colOffset, colIdx + colOffset, 15, NULL);
      }

      for(int64_t rowIdx = 0; rowIdx < columns; rowIdx++) {
        if(results.getStatistics().contains(rowIdx)) {
          auto statistics = results.getStatistics().at(rowIdx);

          worksheet_write_number(worksheet, rowOffset + rowIdx + 1, colIdx + colOffset,
                                 statistics.getStatistics()[statColIdx], numberFormat);

        } else {
        }
      }
      colIdx++;
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

      worksheet_write_url(worksheet, headerColumnRowOffset, rowIndex + colOffset, filePath.data(),
                          imageHeaderHyperlinkFormat);
      worksheet_write_string(worksheet, headerColumnRowOffset, rowIndex + colOffset, rowName.data(),
                             imageHeaderHyperlinkFormat);

      worksheet_set_column(worksheet, rowIndex + colOffset, rowIndex + colOffset, 15, NULL);
    }
  }

  rowOffset++;

  //
  // Write Result header
  //
  for(int64_t rowIdx = 0; rowIdx < columns; rowIdx++) {
    worksheet_write_string(worksheet, rowIdx + rowOffset, 1, results.getColumnNameAt(rowIdx).data(), header);
    worksheet_set_column(worksheet, 1, 1, 20, NULL);
  }

  //
  // Write Table Name
  //
  //
  worksheet_merge_range(worksheet, rowOffset, 0, rowOffset + results.getNrOfColumns() - 1, 0, "-", merge_format);
  if(!results.getTableName().empty()) {
    worksheet_write_string(worksheet, rowOffset, 0, results.getTableName().data(), merge_format);
  } else {
    worksheet_write_string(worksheet, rowOffset, 0, std::to_string(rowOffset).data(), merge_format);
  }

  //
  // Write table data
  //
  for(int64_t rowIndex = 0; rowIndex < results.getNrOfRows(); rowIndex++) {
    for(int64_t colIndex = 0; colIndex < columns; colIndex++) {
      if(results.getTable().contains(colIndex) &&
         results.getTable().at(colIndex).contains(getIndexOfSortedMap(rowIndex))) {
        if(!results.getTable().at(colIndex).at(getIndexOfSortedMap(rowIndex)).validity.has_value()) {
          worksheet_write_number(worksheet, rowOffset + colIndex, rowIndex + colOffset,
                                 results.getTable().at(colIndex).at(getIndexOfSortedMap(rowIndex)).value, numberFormat);
        } else {
          worksheet_write_string(worksheet, rowOffset + colIndex, rowIndex + colOffset,
                                 results::Table::validityToString(
                                     results.getTable().at(colIndex).at(getIndexOfSortedMap(rowIndex)).validity.value())
                                     .data(),
                                 NULL);
        }
      } else {
        // Empty table entry
      }
    }
  }

  rowOffset = rowOffset + columns;

  return {colOffset, rowOffset};
}
}    // namespace joda::pipeline::reporting
