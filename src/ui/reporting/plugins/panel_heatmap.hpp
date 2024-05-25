///
/// \file      panel_heatmap.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-21
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <qcolormap.h>
#include <qwidget.h>
#include "backend/results/table/table.hpp"

namespace joda::ui::qt::reporting::plugin {

///
/// \class      ChartHeatMap
/// \author     Joachim Danmayr
/// \brief      Chart plotting a heatmap
///
class ChartHeatMap : public QWidget
{
public:
  /////////////////////////////////////////////////////
  ChartHeatMap(QWidget *parent);
  void setData(const joda::results::Table &);

private:
  /////////////////////////////////////////////////////
  QString formatDoubleScientific(double value, int precision = 3);
  void paintEvent(QPaintEvent *ev) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  joda::results::Table mData;
  std::map<float, QColor> mColorMap{
      {0.1, QColor{32, 102, 168}},  {0.1, QColor{32, 102, 168}},  {0.2, QColor{142, 193, 218}},
      {0.3, QColor{205, 225, 236}}, {0.4, QColor{237, 237, 237}}, {0.5, QColor{246, 214, 194}},
      {0.6, QColor{246, 214, 194}}, {0.7, QColor{246, 214, 194}}, {0.7, QColor{212, 114, 100}},
      {0.8, QColor{174, 40, 44}},   {0.9, QColor{174, 40, 44}},   {1.1, QColor{174, 40, 44}}};

  /////////////////////////////////////////////////////
  static inline const uint32_t spacing       = 4;
  static inline const uint32_t Y_TOP_MARING  = 16;
  static inline const uint32_t X_LEFT_MARGIN = 10;

  uint32_t mRows        = 0;
  uint32_t mCols        = 0;
  int32_t mHoveredWell  = -1;
  int32_t mSelectedWell = -1;
};

///
/// \class      PanelHeatmap
/// \author     Joachim Danmayr
/// \brief      Heatmap panel
///
class PanelHeatmap : public QWidget
{
public:
  /////////////////////////////////////////////////////
  PanelHeatmap(QWidget *parent);
  void setData(const joda::results::Table &);

private:
  /////////////////////////////////////////////////////
  ChartHeatMap *mHeatmap01;
};

}    // namespace joda::ui::qt::reporting::plugin
