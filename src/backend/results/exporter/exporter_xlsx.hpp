///
/// \file      exporter_xlsx.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     XLSX exporter
///

#pragma once

#include <xlsxwriter/workbook.h>
#include "backend/results/table/table.hpp"

namespace joda::results::exporter {

class ExporterXlsx
{
public:
  /////////////////////////////////////////////////////
  static void exportAsList(const joda::results::Table &table, std::string outputFileName);
  static void exportAsHeatmap(const joda::results::Table &table, std::string outputFileName);

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t CELL_SIZE = 60;

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

  /////////////////////////////////////////////////////
  static WorkBook prepare(std::string outputFileName);
  static void paintPlateBorder(lxw_worksheet *sheet, int64_t rows, int64_t cols, int32_t rowOffset, lxw_format *header,
                               lxw_format *numberFormat);
};

}    // namespace joda::results::exporter
