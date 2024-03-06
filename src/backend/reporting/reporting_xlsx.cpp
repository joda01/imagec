

#include "reporting.h"
#include "xlsxwriter.h"

namespace joda::reporting {

///
/// \brief      Writes the report to a CSV file
/// \author     Joachim Danmayr
/// \param[in]  fileName  Name of the output report file
///
void Table::flushReportToFileXlsx(std::string_view fileName) const
{
  /* Create a new workbook and add a worksheet. */
  lxw_workbook *workbook   = workbook_new(fileName.data());
  lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);

  /* Add a format. */
  lxw_format *header = workbook_add_format(workbook);

  /* Set the bold property for the format */
  format_set_bold(header);
  format_set_pattern(header, LXW_PATTERN_SOLID);
  format_set_bg_color(header, LXW_COLOR_YELLOW);
  format_set_border(header, LXW_BORDER_THIN);

  int ROW_OFFSET = 1;
  int COL_OFFSET = 1;

  //
  // Write header
  //
  int64_t columns = std::max(getNrOfColumns(), static_cast<int64_t>(mColumnName.size()));
  for(int64_t colIdx = 0; colIdx < columns; colIdx++) {
    if(mColumnName.contains(colIdx)) {
      worksheet_write_string(worksheet, 0, colIdx + COL_OFFSET, mColumnName.at(colIdx).data(), header);
    } else {
      worksheet_write_string(worksheet, 0, colIdx + COL_OFFSET, std::to_string(colIdx).data(), header);
    }
    worksheet_set_column(worksheet, colIdx + COL_OFFSET, colIdx + COL_OFFSET, 20, NULL);
  }

  worksheet_set_column(worksheet, 0, 0, 10, NULL);

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
                                 mTable.at(colIdx).at(rowIdx).value, NULL);
        } else {
          worksheet_write_string(worksheet, ROW_OFFSET + rowIdx, colIdx + COL_OFFSET,
                                 validityToString(mTable.at(colIdx).at(rowIdx).validity.value()).data(), NULL);
        }
      } else {
        // Empty table entry
      }
    }
  }

  ROW_OFFSET = rowIdx + 2;

  //
  // Write measurement data
  //
  for(int rowIdx = 0; rowIdx < Statistics::NR_OF_VALUE; rowIdx++) {
    worksheet_write_string(worksheet, ROW_OFFSET + rowIdx, 0, Statistics::getStatisticsTitle()[rowIdx].data(), header);

    for(int64_t colIdx = 0; colIdx < columns; colIdx++) {
      if(mStatistics.contains(colIdx)) {
        auto statistics = mStatistics.at(colIdx);

        worksheet_write_number(worksheet, ROW_OFFSET + rowIdx, colIdx + COL_OFFSET, statistics.getStatistics()[rowIdx],
                               NULL);

      } else {
      }
    }
  }

  /* Write some simple text. */
  /* worksheet_write_string(worksheet, 0, 0, "Hello", NULL);

   worksheet_write_string(worksheet, 1, 0, "World", format);

   worksheet_write_number(worksheet, 2, 0, 123, NULL);
   worksheet_write_number(worksheet, 3, 0, 123.456, NULL);
   worksheet_insert_image(worksheet, 1, 2, "logo.png");*/

  workbook_close(workbook);
}
}    // namespace joda::reporting
