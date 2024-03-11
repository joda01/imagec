

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
std::tuple<int, int> Table::flushReportToFileXlsx(int colOffset, int /*rowOffset*/, lxw_worksheet *worksheet,
                                                  lxw_format *header, lxw_format *merge_format,
                                                  lxw_format *numberFormat) const
{
  int ROW_OFFSET = 2;
  int COL_OFFSET = colOffset + 1;

  //
  // Write name
  //
  worksheet_merge_range(worksheet, 0, COL_OFFSET, 0, COL_OFFSET + getNrOfColumns() - 1, "-", merge_format);
  if(!mTableName.empty()) {
    worksheet_write_string(worksheet, 0, COL_OFFSET, mTableName.data(), header);
  } else {
    worksheet_write_string(worksheet, 0, COL_OFFSET, std::to_string(COL_OFFSET).data(), header);
  }

  //
  // Write header
  //
  int64_t columns = std::max(getNrOfColumns(), static_cast<int64_t>(mColumnName.size()));
  for(int64_t colIdx = 0; colIdx < columns; colIdx++) {
    if(mColumnName.contains(colIdx)) {
      worksheet_write_string(worksheet, 1, colIdx + COL_OFFSET, mColumnName.at(colIdx).data(), header);
    } else {
      worksheet_write_string(worksheet, 1, colIdx + COL_OFFSET, std::to_string(colIdx).data(), header);
    }
    worksheet_set_column(worksheet, colIdx + COL_OFFSET, colIdx + COL_OFFSET, 10, NULL);
  }

  worksheet_set_column(worksheet, 0, 0, 10, NULL);

  //
  // Write statistic data
  //
  int rowIdxStat = 0;
  for(rowIdxStat = 0; rowIdxStat < Statistics::NR_OF_VALUE; rowIdxStat++) {
    worksheet_write_string(worksheet, ROW_OFFSET + rowIdxStat, 0, Statistics::getStatisticsTitle()[rowIdxStat].data(),
                           header);

    for(int64_t colIdx = 0; colIdx < columns; colIdx++) {
      if(mStatistics.contains(colIdx)) {
        auto statistics = mStatistics.at(colIdx);

        worksheet_write_number(worksheet, ROW_OFFSET + rowIdxStat, colIdx + COL_OFFSET,
                               statistics.getStatistics()[rowIdxStat], numberFormat);

      } else {
      }
    }
  }
  ROW_OFFSET = ROW_OFFSET += (rowIdxStat + 2);

  //
  // Write table data
  //
  int64_t rowIdx = 0;
  for(rowIdx = 0; rowIdx < getNrOfRows(); rowIdx++) {
    for(int64_t colIdx = 0; colIdx < columns; colIdx++) {
      //
      // Write row data
      //
      if(0 == colIdx) {
        //
        // Write row header
        //
        if(mRowNames.contains(rowIdx)) {
          worksheet_write_string(worksheet, ROW_OFFSET + rowIdx, colIdx, mRowNames.at(rowIdx).data(), header);
        } else {
          worksheet_write_string(worksheet, ROW_OFFSET + rowIdx, colIdx, std::to_string(rowIdx).data(), header);
        }
      }

      if(mTable.contains(colIdx) && mTable.at(colIdx).contains(rowIdx)) {
        if(!mTable.at(colIdx).at(rowIdx).validity.has_value()) {
          worksheet_write_number(worksheet, ROW_OFFSET + rowIdx, colIdx + COL_OFFSET,
                                 mTable.at(colIdx).at(rowIdx).value, numberFormat);
        } else {
          worksheet_write_string(worksheet, ROW_OFFSET + rowIdx, colIdx + COL_OFFSET,
                                 validityToString(mTable.at(colIdx).at(rowIdx).validity.value()).data(), NULL);
        }
      } else {
        // Empty table entry
      }
    }
  }

  return {columns, 2};
}
}    // namespace joda::reporting
