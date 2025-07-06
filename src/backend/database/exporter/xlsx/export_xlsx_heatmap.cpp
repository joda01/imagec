///
/// \file      exporter_xlsx.cpp
/// \author    Joachim Danmayr
/// \date      2025-07-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include <xlsxwriter/worksheet.h>
#include <string>
#include <variant>
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "controller/controller.hpp"
#include "exporter_xlsx.hpp"

namespace joda::exporter::xlsx {

void Exporter::startHeatmapExport(const std::vector<const Exportable *> &data, const settings::AnalyzeSettings &analyzeSettings,
                                  const std::string &jobName, std::chrono::system_clock::time_point timeStarted,
                                  std::chrono::system_clock::time_point timeFinished, const std::string &outputFileName,
                                  const settings::ResultsSettings &filterSettings, ExportSettings::ExportView view, int32_t imageHeight,
                                  int32_t imageWidth)
{
  int32_t cols           = filterSettings.getPlateSetup().cols;
  int32_t rows           = filterSettings.getPlateSetup().rows;
  int32_t densityMapSize = -1;
  switch(view) {
    case ExportSettings::ExportView::PLATE:
      break;
    case ExportSettings::ExportView::WELL:
      rows = filterSettings.getPlateSetup().getRowsAndColsOfWell().first;
      cols = filterSettings.getPlateSetup().getRowsAndColsOfWell().second;
      break;
    case ExportSettings::ExportView::IMAGE:
      densityMapSize = filterSettings.getDensityMapSettings().densityMapAreaSize;
      rows           = static_cast<int32_t>(std::ceil((float) imageHeight / (float) densityMapSize));
      cols           = static_cast<int32_t>(std::ceil((float) imageWidth / (float) densityMapSize));

      break;
  }

  for(const auto &tbl : data) {
    auto colNr = filterSettings.getColumns().size();
    for(int n = 0; n < colNr; n++) {
      auto dataHeatmap =
          joda::db::data::convertToHeatmap(&tbl->getTable(), rows, cols, n, joda::db::data::PlotPlateSettings{.densityMapSize = densityMapSize});

      // No write the heatmap to XLSX
    }
  }
}

}    // namespace joda::exporter::xlsx
