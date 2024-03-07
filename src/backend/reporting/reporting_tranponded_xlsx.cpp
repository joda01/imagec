

#include <xlsxwriter/worksheet.h>
#include <string>
#include "reporting.h"
#include "xlsxwriter.h"

namespace joda::reporting {

///
/// \brief      Writes the report to a CSV file
/// \author     Joachim Danmayr
/// \param[in]  fileName  Name of the output report file
///
std::tuple<int, int> Table::flushReportToFileXlsxTransponded(int colOffset, int rowOffset, lxw_worksheet *worksheet,
                                                             lxw_format *header, lxw_format *merge_format,
                                                             lxw_format *numberFormat) const
{
  bool WRITE_HEADER_FOR_EACH_CHANNEL = false;

  colOffset = 2;

  int64_t columns = std::max(getNrOfColumns(), static_cast<int64_t>(mColumnName.size()));

  //
  // Write statistics data
  //
  for(int colIdx = 0; colIdx < Statistics::NR_OF_VALUE; colIdx++) {
    if(rowOffset == 0 || WRITE_HEADER_FOR_EACH_CHANNEL) {
      worksheet_write_string(worksheet, rowOffset, colIdx + colOffset, Statistics::getStatisticsTitle()[colIdx].data(),
                             header);
    }

    for(int64_t rowIdx = 0; rowIdx < columns; rowIdx++) {
      if(mStatistics.contains(rowIdx)) {
        auto statistics = mStatistics.at(rowIdx);

        worksheet_write_number(worksheet, rowOffset + rowIdx + 1, colIdx + colOffset,
                               statistics.getStatistics()[colIdx], numberFormat);

      } else {
      }
    }
  }

  colOffset += Statistics::NR_OF_VALUE;

  //
  // Write image header
  //
  //
  int headerColumnRowOffset = 0;
  if(headerColumnRowOffset == 0 || WRITE_HEADER_FOR_EACH_CHANNEL) {
    for(int32_t colIdx = 0; colIdx < getNrOfRows(); colIdx++) {
      if(mRowNames.contains(colIdx)) {
        std::string filePath = "external:.\\" + mRowNames.at(colIdx) + "/detail.xlsx";
        worksheet_write_url(worksheet, headerColumnRowOffset, colIdx + colOffset, filePath.data(), NULL);
        worksheet_write_string(worksheet, headerColumnRowOffset, colIdx + colOffset, mRowNames.at(colIdx).data(),
                               header);
      } else {
        worksheet_write_string(worksheet, headerColumnRowOffset, colIdx + colOffset, std::to_string(colIdx).data(),
                               header);
      }
      worksheet_set_column(worksheet, colIdx + colOffset, colIdx + colOffset, 20, NULL);
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
  for(int64_t colIdx = 0; colIdx < getNrOfRows(); colIdx++) {
    for(int64_t rowIdx = 0; rowIdx < columns; rowIdx++) {
      if(mTable.contains(rowIdx) && mTable.at(rowIdx).contains(colIdx)) {
        if(!mTable.at(rowIdx).at(colIdx).validity.has_value()) {
          worksheet_write_number(worksheet, rowOffset + rowIdx, colIdx + colOffset, mTable.at(rowIdx).at(colIdx).value,
                                 numberFormat);
        } else {
          worksheet_write_string(worksheet, rowOffset + rowIdx, colIdx + colOffset,
                                 validityToString(mTable.at(rowIdx).at(colIdx).validity.value()).data(), NULL);
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
