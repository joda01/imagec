

#include <xlsxwriter/worksheet.h>
#include <string>
#include <unordered_map>
#include "reporting.h"
#include "xlsxwriter.h"

namespace joda::reporting {

///
/// \brief      Writes the report to a CSV file
/// \author     Joachim Danmayr
/// \param[in]  fileName  Name of the output report file
///
std::tuple<int, int> Table::flushReportToFileXlsxTransponded(const std::string &headerText, const std::string &jobName,
                                                             int colOffset, int rowOffset, int startRow,
                                                             lxw_worksheet *worksheet, lxw_format *header,
                                                             lxw_format *merge_format, lxw_format *numberFormat,
                                                             lxw_format *imageHeaderHyperlinkFormat) const
{
  setlocale(LC_NUMERIC, "C");    // Needed for correct comma in libxlsx
  const int STATISTIC_START_WITH_INDEX = 2;

  //
  // Sort rows
  //
  std::map<std::string, uint64_t> sortedRow;
  for(const auto &[idx, name] : mRowNames) {
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

  int64_t columns = std::max(getNrOfColumns(), static_cast<int64_t>(mColumnName.size()));

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

    for(int statColIdx = STATISTIC_START_WITH_INDEX; statColIdx < Statistics::NR_OF_VALUE; statColIdx++) {
      if(rowOffset == startRow || WRITE_HEADER_FOR_EACH_CHANNEL) {
        worksheet_write_string(worksheet, rowOffset, colIdx + colOffset,
                               Statistics::getStatisticsTitle()[statColIdx].data(), header);
        worksheet_set_column(worksheet, colIdx + colOffset, colIdx + colOffset, 15, NULL);
      }

      for(int64_t rowIdx = 0; rowIdx < columns; rowIdx++) {
        if(mStatistics.contains(rowIdx)) {
          auto statistics = mStatistics.at(rowIdx);

          worksheet_write_number(worksheet, rowOffset + rowIdx + 1, colIdx + colOffset,
                                 statistics.getStatistics()[statColIdx], numberFormat);

        } else {
        }
      }
      colIdx++;
    }
  }

  colOffset += (Statistics::NR_OF_VALUE - STATISTIC_START_WITH_INDEX + 1);

  //
  // Write image header
  //
  //
  int headerColumnRowOffset = rowOffset;
  if(headerColumnRowOffset == startRow || WRITE_HEADER_FOR_EACH_CHANNEL) {
    for(int32_t rowIndex = 0; rowIndex < getNrOfRows(); rowIndex++) {
      if(mRowNames.contains(getIndexOfSortedMap(rowIndex))) {
        std::string filePath =
            "external:.\\" + mRowNames.at(getIndexOfSortedMap(rowIndex)) + "/results_image_" + jobName + ".xlsx";

        worksheet_write_url(worksheet, headerColumnRowOffset, rowIndex + colOffset, filePath.data(),
                            imageHeaderHyperlinkFormat);
        worksheet_write_string(worksheet, headerColumnRowOffset, rowIndex + colOffset,
                               mRowNames.at(getIndexOfSortedMap(rowIndex)).data(), imageHeaderHyperlinkFormat);
      } else {
        worksheet_write_string(worksheet, headerColumnRowOffset, rowIndex + colOffset,
                               std::to_string(getIndexOfSortedMap(rowIndex)).data(), imageHeaderHyperlinkFormat);
      }
      worksheet_set_column(worksheet, rowIndex + colOffset, rowIndex + colOffset, 15, NULL);
    }
  }

  rowOffset++;

  //
  // Write Result header
  //
  for(int64_t rowIdx = 0; rowIdx < columns; rowIdx++) {
    if(mColumnName.contains(rowIdx)) {
      worksheet_write_string(worksheet, rowIdx + rowOffset, 1, mColumnName.at(rowIdx).data(), header);
    } else {
      worksheet_write_string(worksheet, rowIdx + rowOffset, 1, std::to_string(rowIdx).data(), header);
    }
    worksheet_set_column(worksheet, 1, 1, 20, NULL);
  }

  //
  // Write Table Name
  //
  //
  worksheet_merge_range(worksheet, rowOffset, 0, rowOffset + getNrOfColumns() - 1, 0, "-", merge_format);
  if(!mTableName.empty()) {
    worksheet_write_string(worksheet, rowOffset, 0, mTableName.data(), merge_format);
  } else {
    worksheet_write_string(worksheet, rowOffset, 0, std::to_string(rowOffset).data(), merge_format);
  }

  //
  // Write table data
  //
  for(int64_t rowIndex = 0; rowIndex < getNrOfRows(); rowIndex++) {
    for(int64_t colIndex = 0; colIndex < columns; colIndex++) {
      if(mTable.contains(colIndex) && mTable.at(colIndex).contains(getIndexOfSortedMap(rowIndex))) {
        if(!mTable.at(colIndex).at(getIndexOfSortedMap(rowIndex)).validity.has_value()) {
          worksheet_write_number(worksheet, rowOffset + colIndex, rowIndex + colOffset,
                                 mTable.at(colIndex).at(getIndexOfSortedMap(rowIndex)).value, numberFormat);
        } else {
          worksheet_write_string(
              worksheet, rowOffset + colIndex, rowIndex + colOffset,
              validityToString(mTable.at(colIndex).at(getIndexOfSortedMap(rowIndex)).validity.value()).data(), NULL);
        }
      } else {
        // Empty table entry
      }
    }
  }

  rowOffset = rowOffset + columns;

  return {colOffset, rowOffset};
}
}    // namespace joda::reporting
