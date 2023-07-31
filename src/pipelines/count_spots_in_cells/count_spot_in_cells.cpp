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

#include "count_spot_in_cells.hpp"
#include <stdexcept>
#include "image_processing/functions/detection/voronoi_grid/voronoi_grid.hpp"
#include "image_processing/functions/roi/roi.hpp"

namespace joda::pipeline {

void CountSpotInCells::execute(const settings::json::AnalyzeSettings &analyseSettings,
                               const std::map<int, joda::func::DetectionResponse> &detectionResults,
                               const std::string &detailoutputPath)
{
  auto nucleusChannels = analyseSettings.getChannels(joda::settings::json::ChannelInfo::Type::NUCLEUS);
  if(nucleusChannels.empty()) {
    throw std::runtime_error("At least one nucleus channel must be selected!");
  }

  auto spotChannels = analyseSettings.getChannels(joda::settings::json::ChannelInfo::Type::SPOT);
  if(spotChannels.empty()) {
    throw std::runtime_error("At least one spot channel must be selected!");
  }

  auto nucleusChannelIndex = nucleusChannels.at(0).getChannelInfo().getChannelIndex();
  auto spotChannelIndex    = spotChannels.at(0).getChannelInfo().getChannelIndex();

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
  joda::reporting::Table detailReport;
  generateReportHeader(detailReport, spotChannels.at(0), true);
  generateReportHeader(cellReport, spotChannels.at(0), false);

  for(auto const &roiCell : voronoiResult.result) {
    appendToReport(cellReport, roiCell, roiCell.getIndex(), false);

    for(auto const &roiSpot : detectionResults.at(spotChannelIndex).result) {
      if(roiCell.doesIntersect(roiSpot)) {
        // Intersect
        appendToReport(detailReport, roiSpot, roiCell.getIndex(), true);
      }
    }
  }

  detailReport.flushReportToFile(detailoutputPath + std::filesystem::path::preferred_separator + "spots_in_cells.csv");
  cellReport.flushReportToFile(detailoutputPath + std::filesystem::path::preferred_separator +
                               "approximated_cells.csv");

  cv::imwrite(detailoutputPath + std::filesystem::path::preferred_separator + "control_approximated_cells" + ".jpg",
              voronoiResult.controlImage);
}

void CountSpotInCells::appendToReport(joda::reporting::Table &report, const func::ROI &spot, int cellIndex,
                                      bool intersect)
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

  if(intersect) {
    report.appendValueToColumnAtRow(colIdx + static_cast<int>(ColumnIndexDetailedReport::CELL_INTERSECTING_WITH),
                                    spot.getIndex(), cellIndex, func::ParticleValidity::VALID);
  }
}

void CountSpotInCells::generateReportHeader(joda::reporting::Table &report,
                                            const settings::json::ChannelSettings &spotChannelSettings, bool intersect)
{
  int colIdx = 0;

  if(intersect) {
    report.setColumnNames({{colIdx + static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE),
                            spotChannelSettings.getChannelInfo().getName() + "#confidence"},
                           {colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY),
                            spotChannelSettings.getChannelInfo().getName() + "#intensity"},
                           {colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN),
                            spotChannelSettings.getChannelInfo().getName() + "#Min"},
                           {colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX),
                            spotChannelSettings.getChannelInfo().getName() + "#Max"},
                           {colIdx + static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE),
                            spotChannelSettings.getChannelInfo().getName() + "#areaSize"},
                           {colIdx + static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY),
                            spotChannelSettings.getChannelInfo().getName() + "#circularity"},
                           {colIdx + static_cast<int>(ColumnIndexDetailedReport::VALIDITY),
                            spotChannelSettings.getChannelInfo().getName() + "#validity"},
                           {colIdx + static_cast<int>(ColumnIndexDetailedReport::CELL_INTERSECTING_WITH),
                            spotChannelSettings.getChannelInfo().getName() + "#cellIndex"}});
  } else {
    report.setColumnNames({{colIdx + static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE),
                            spotChannelSettings.getChannelInfo().getName() + "#confidence"},
                           {colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY),
                            spotChannelSettings.getChannelInfo().getName() + "#intensity"},
                           {colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN),
                            spotChannelSettings.getChannelInfo().getName() + "#Min"},
                           {colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX),
                            spotChannelSettings.getChannelInfo().getName() + "#Max"},
                           {colIdx + static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE),
                            spotChannelSettings.getChannelInfo().getName() + "#areaSize"},
                           {colIdx + static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY),
                            spotChannelSettings.getChannelInfo().getName() + "#circularity"},
                           {colIdx + static_cast<int>(ColumnIndexDetailedReport::VALIDITY),
                            spotChannelSettings.getChannelInfo().getName() + "#validity"}});
  }
}

}    // namespace joda::pipeline
