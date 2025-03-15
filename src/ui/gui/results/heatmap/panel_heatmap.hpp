///
/// \file      panel_heatmap.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <qboxlayout.h>
#include <qcolormap.h>
#include <qmainwindow.h>
#include <qwidget.h>
#include "backend/helper/table/table.hpp"
#include "heatmap_color_generator.hpp"

namespace joda::ui::gui {

class PanelResults;

struct Point
{
  uint32_t x = 0;
  uint32_t y = 0;
};

///
/// \class      ChartHeatMap
/// \author     Joachim Danmayr
/// \brief      Chart plotting a heatmap
///
class ChartHeatMap : public QWidget
{
  Q_OBJECT

public:
  enum class MatrixForm
  {
    CIRCLE,
    RECTANGLE
  };

  enum class PaintControlImage
  {
    NO,
    YES
  };

  enum class HeatmapMinMax
  {
    AUTO,
    MANUAL
  };

  /////////////////////////////////////////////////////
  ChartHeatMap(PanelResults *parent);
  void setData(const joda::table::Table &, MatrixForm form, PaintControlImage paint, int32_t newHierarchy);
  void exportToSVG(const QString &filePath);
  void exportToPNG(const QString &filePath);

  [[nodiscard]] const table::Table &getData() const
  {
    return mData;
  }

signals:
  void onElementClick(int cellX, int cellY, table::TableCell value);
  void onDoubleClicked(int cellX, int cellY, table::TableCell value);

private:
  /////////////////////////////////////////////////////
  QString formatDoubleScientific(double value, int precision = 3);
  static void drawGaussianCurve(QPainter &painter, int startX, int startY, int height, int length);
  static double calcValueOnGaussianCurve(double x, double avg, double sttdev);
  void paintEvent(QPaintEvent *ev) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void drawChart(QPainter &painter, const QSize &size);

  // Function to find the nearest element in the map to a given value x
  static std::pair<float, QColor> findNearest(std::map<float, QColor> &myMap, double target);
  std::tuple<int32_t, Point> getWellUnderMouse(QMouseEvent *event);

  joda::table::Table mData;
  /*std::map<float, QColor> mColorMap{{0.1, QColor{32, 102, 168}},  {0.2, QColor{142, 193, 218}},
                                    {0.3, QColor{205, 225, 236}}, {0.4, QColor{237, 237, 237}},
                                    {0.5, QColor{246, 214, 194}}, {0.6, QColor{246, 214, 194}},
                                    {0.7, QColor{246, 214, 194}}, {0.8, QColor{212, 114, 100}},
                                    {0.9, QColor{174, 40, 44}},   {1, QColor{174, 40, 44}}};*/

  std::map<float, QColor> mColorMap{generateColorMap()};
  double showInputDialog(double defaultVal);

  /////////////////////////////////////////////////////
  static inline const float spacing                       = 4.0;
  float Y_TOP_MARING                                      = 16;
  float X_LEFT_MARGIN                                     = 10;
  static inline const float LEGEND_HEIGHT                 = 30;
  static inline const float LEGEND_COLOR_ROW_HEIGHT       = 15;
  static inline const float HEATMAP_FONT_SIZE             = 12;
  static inline const float HEATMAP_COLOR_ROW_TEXT_HEIGHT = 25;

  PanelResults *mParent;
  MatrixForm mForm = MatrixForm::CIRCLE;
  PaintControlImage mPaintCtrlImage;
  HeatmapMinMax mMinMaxMode = HeatmapMinMax::AUTO;
  struct HeatMapMinMax
  {
    QRect textMinPos;
    QRect textMaxPos;
    double min = 0;
    double max = 0;
  } mHeatMapMinMax;

  uint32_t mRows        = 0;
  uint32_t mCols        = 0;
  int32_t mHoveredWell  = -1;
  int32_t mActHierarchy = 0;

  struct Selection
  {
    int32_t mSelectedWell = -1;
    Point mSelectedPoint;
  };

  std::map<int32_t, Selection> mSelection;

  QString mActControlImagePath;
  QImage mActControlImage;
  bool mIsHovering = false;
};
}    // namespace joda::ui::gui
