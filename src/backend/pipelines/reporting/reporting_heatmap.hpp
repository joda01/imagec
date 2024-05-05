///
/// \file      results.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <xlsxwriter/worksheet.h>
#include "backend/image_reader/image_reader.hpp"
#include "backend/results/results.hpp"
#include "backend/settings/analze_settings.hpp"

namespace joda::pipeline::reporting {

///
/// \class      Reporting generation
/// \author     Joachim Danmayr
/// \brief
///
class Heatmap
{
public:
  /////////////////////////////////////////////////////
  static void createHeatMapForImage(const joda::results::WorkSheet &containers, const std::string &fileName);
  static void createAllOverHeatMap(const joda::settings::ChannelReportingSettings &reportingSettings,
                                   joda::results::WorkSheet &allOverReport, const std::string &outputFolder);

private:
  /////////////////////////////////////////////////////
  struct HeatMapPoint
  {
    int32_t x = -1;
    int32_t y = -1;
  };

  /////////////////////////////////////////////////////

  static constexpr int32_t CELL_SIZE = 60;

  static void paintPlateBorder(lxw_worksheet *sheet, int64_t rows, int64_t cols, int32_t rowOffset, lxw_format *header,
                               lxw_format *numberFormat);

  static void createHeatmapOfWellsForGroup(const std::string &outputFolder, const std::string &groupName,
                                           const std::map<int32_t, HeatMapPoint> &wellOrder, int32_t sizeX,
                                           int32_t sizeY, const joda::results::Group &groupReports);

  static auto transformMatrix(const std::vector<std::vector<int32_t>> &imageWellOrderMatrix, int32_t &sizeX,
                              int32_t &sizeY) -> std::map<int32_t, HeatMapPoint>;
};

}    // namespace joda::pipeline::reporting
