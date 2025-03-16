///
/// \file      export_heatmap.hpp
/// \author    Joachim Danmayr
/// \date      2025-03-16
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <sys/types.h>
#include <xlsxwriter/workbook.h>
#include <QPainter>
#include <cstdint>
#include <map>
#include <string>
#include <tuple>
#include <vector>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/database/plugins/filter.hpp"
#include "backend/helper/database/plugins/stats_for_well.hpp"
#include "backend/helper/table/table.hpp"
#include "backend/settings/analze_settings.hpp"
#include "export_heatmap_settings.hpp"
#include "heatmap_color_generator.hpp"

namespace joda::db {

class HeatmapExporter
{
public:
  HeatmapExporter(joda::settings::DensityMapSettings &settings) : mSettings(settings)
  {
  }
  ~HeatmapExporter()
  {
  }
  void setData(const joda::table::Table &data);
  void setMinMaxMode(joda::settings::DensityMapSettings::HeatMapRangeMode mode);
  void setSelectedIndex(int32_t idx);
  void paint(QPainter &painter, const QSize &size, bool updatePosition = true) const;
  void exportToSVG(const QString &filePath) const;
  void exportToPNG(const QString &filePath) const;
  [[nodiscard]] const table::Table &getData() const
  {
    return mData;
  }
  std::tuple<int32_t, QPoint> getWellAtPosition(const QPoint &pos, const QSize &size) const;
  bool isLegendMinSectionCLicked(const QPoint &pos) const;
  bool isLegendMaxSectionCLicked(const QPoint &pos) const;

  void setHeatMapMin(float min);
  void setHeatMapMax(float max);
  float getHeatMapMin() const;
  float getHeatMapMax() const;

private:
  /////////////////////////////////////////////////////
  struct HeatMapMinMax
  {
    double min = 0;
    double max = 0;
  };

  /////////////////////////////////////////////////////
  std::pair<float, QColor> findNearest(const std::map<float, QColor> &myMap, double target) const;
  QString formatDoubleScientific(double value, int precision = 3) const;
  void drawGaussianCurve(QPainter &painter, int startX, int startY, int height, int length) const;
  void drawLegend(QPainter &painter, float rectWidth, float xOffset, float X_LEFT_MARGIN, float Y_TOP_MARING, bool updatePosition,
                  const QRect &headerMetrics) const;
  double calcValueOnGaussianCurve(double x, double avg, double sttdev) const;
  auto calcMargins(const QSize &size) const -> std::tuple<float, float, QRect, QFont>;
  HeatMapMinMax getHeatmapMinMax() const;

  /////////////////////////////////////////////////////
  const float spacing                 = 4.0;
  const float LEGEND_COLOR_ROW_HEIGHT = 15;

  struct LegendPosition
  {
    QRect textMinPos;
    QRect textMaxPos;
  };

  HeatMapMinMax mHeatMapMinMaxAuto;
  mutable LegendPosition mLegendPosition;

  // DATA TO PAINT//////////////////////
  uint32_t mRows = 0;
  uint32_t mCols = 0;

  joda::table::Table mData;
  int32_t mSelectedIndex = -1;

  // DATA TO PAINT//////////////////////
  joda::settings::DensityMapSettings &mSettings;
  std::map<float, QColor> mColorMap{generateColorMap()};
};

}    // namespace joda::db
