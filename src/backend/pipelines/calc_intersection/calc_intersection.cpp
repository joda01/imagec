///
/// \file      calc_intersection.cpp
/// \author    Joachim Danmayr
/// \date      2023-09-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "calc_intersection.hpp"
#include "../../reporting/reporting.h"

namespace joda::pipeline {

CalcIntersection::CalcIntersection(const std::set<int32_t> &indexesToIntersect) :
    mIndexesToIntersect(indexesToIntersect)
{
}

auto CalcIntersection::execute(const settings::json::AnalyzeSettings &settings,
                               const std::map<int, joda::func::DetectionResponse> &detectionResultsIn,
                               const std::string &detailoutputPath) const -> joda::func::DetectionResponse
{
  //
  // Calculate the intersection
  //
  joda::reporting::Table cellReport;
  joda::reporting::Table detailReport;
  // generateReportHeader(detailReport, spotChannels.at(0), true);
  //
  // for(auto const &roiCell : voronoiResult.result) {
  //  for(auto const &roiSpot : detectionResults.at(spotChannelIndex).result) {
  //    if(roiCell.doesIntersect(roiSpot)) {
  //      // Intersect
  //      appendToReport(detailReport, roiSpot, roiCell.getIndex(), true);
  //    }
  //  }
  //}
}

void CalcIntersection::appendToReport(joda::reporting::Table &report, const func::ROI &spot, int cellIndex,
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

void CalcIntersection::generateReportHeader(joda::reporting::Table &report,
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
