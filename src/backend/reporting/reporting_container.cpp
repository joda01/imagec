///
/// \file      reporting_container.cpp
/// \author    Joachim Danmayr
/// \date      2024-03-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "reporting_container.hpp"
#include "xlsxwriter.h"

namespace joda::reporting {

ReportingContainer::ReportingContainer()
{
}

void ReportingContainer::flushReportToFile(std::string_view fileName, Table::OutputFormat format)
{
  lxw_workbook *workbook   = workbook_new(fileName.data());
  lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);

  /* Add a format. */
  lxw_format *header = workbook_add_format(workbook);

  // Define the cell format for the merged cells.
  lxw_format *merge_format = workbook_add_format(workbook);
  format_set_align(merge_format, LXW_ALIGN_CENTER);
  format_set_align(merge_format, LXW_ALIGN_VERTICAL_CENTER);

  int colOffset = 0;
  for(const auto &[idx, table] : mColumns) {
    colOffset = table.flushReportToFileXlsx(colOffset, worksheet, header, merge_format);
    colOffset++;
  }

  workbook_close(workbook);
}

}    // namespace joda::reporting
