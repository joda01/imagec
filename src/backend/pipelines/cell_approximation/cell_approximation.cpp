///
/// \file      count_spot_in_cells.cpp
/// \author    Joachim Danmayr
/// \date      2023-07-31
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "cell_approximation.hpp"
#include <stdexcept>
#include "../../image_processing/functions/detection/voronoi_grid/voronoi_grid.hpp"
#include "../../image_processing/functions/roi/roi.hpp"

namespace joda::pipeline {

auto CellApproximation::execute(const settings::json::AnalyzeSettings &settings,
                                const std::map<int, joda::func::DetectionResponse> &detectionResults,
                                const std::string &detailoutputPath) const -> joda::func::DetectionResponse
{
  auto nucleusChannelIndex = mNucleusChannelIndex;

  //
  // Calculate a limited voronoi grid based on the center of nucleus
  //
  joda::func::img::VoronoiGrid grid(detectionResults.at(nucleusChannelIndex).result);
  auto voronoiResult = grid.forward(detectionResults.at(nucleusChannelIndex).controlImage,
                                    detectionResults.at(nucleusChannelIndex).originalImage);
  //
  // Calculate the intersection
  //
  joda::reporting::Table cellReport;
  generateReportHeader(cellReport, settings.getChannelByChannelIndex(nucleusChannelIndex));

  for(auto const &roiCell : voronoiResult.result) {
    appendToReport(cellReport, roiCell, roiCell.getIndex());
  }

  std::string separator(1, std::filesystem::path::preferred_separator);

  cellReport.flushReportToFile(detailoutputPath + separator + "approximated_cells.csv");
  cv::imwrite(detailoutputPath + separator + "control_approximated_cells" + ".jpg", voronoiResult.controlImage);
  return voronoiResult;
}

void CellApproximation::appendToReport(joda::reporting::Table &report, const func::ROI &spot, int cellIndex)
{
  int colIdx = 0;

  report.appendValueToColumnAtRow(colIdx + static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE), spot.getIndex(),
                                  spot.getConfidence(), spot.getValidity());
  report.appendValueToColumnAtRow(colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY), spot.getIndex(),
                                  spot.getIntensity(), spot.getValidity());
  report.appendValueToColumnAtRow(colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN), spot.getIndex(),
                                  spot.getIntensityMin(), spot.getValidity());
  report.appendValueToColumnAtRow(colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX), spot.getIndex(),
                                  spot.getIntensityMax(), spot.getValidity());
  report.appendValueToColumnAtRow(colIdx + static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE), spot.getIndex(),
                                  spot.getAreaSize(), spot.getValidity());
  report.appendValueToColumnAtRow(colIdx + static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY), spot.getIndex(),
                                  spot.getCircularity(), spot.getValidity());
  report.appendValueToColumnAtRow(colIdx + static_cast<int>(ColumnIndexDetailedReport::VALIDITY), spot.getIndex(),
                                  spot.getValidity());
}

void CellApproximation::generateReportHeader(joda::reporting::Table &report,
                                             const settings::json::ChannelSettings &channelSettings)
{
  int colIdx = 0;

  report.setColumnNames({{colIdx + static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE),
                          channelSettings.getChannelInfo().getName() + "#confidence"},
                         {colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY),
                          channelSettings.getChannelInfo().getName() + "#intensity"},
                         {colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN),
                          channelSettings.getChannelInfo().getName() + "#Min"},
                         {colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX),
                          channelSettings.getChannelInfo().getName() + "#Max"},
                         {colIdx + static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE),
                          channelSettings.getChannelInfo().getName() + "#areaSize"},
                         {colIdx + static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY),
                          channelSettings.getChannelInfo().getName() + "#circularity"},
                         {colIdx + static_cast<int>(ColumnIndexDetailedReport::VALIDITY),
                          channelSettings.getChannelInfo().getName() + "#validity"}});
}

}    // namespace joda::pipeline
