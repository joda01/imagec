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
#include <qmainwindow.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "backend/results/analyzer/analyzer.hpp"
#include "backend/results/db_column_ids.hpp"
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
  enum class MatrixForm
  {
    CIRCLE,
    RECTANGLE
  };
  /////////////////////////////////////////////////////
  ChartHeatMap(QWidget *parent);
  void setData(const joda::results::Table &, MatrixForm form);
  [[nodiscard]] results::WellId getSelectedWell() const
  {
    return mSelectedWellId;
  }

private:
  /////////////////////////////////////////////////////
  QString formatDoubleScientific(double value, int precision = 3);
  void paintEvent(QPaintEvent *ev) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  joda::results::Table mData;
  std::map<float, QColor> mColorMap{{0.1, QColor{32, 102, 168}},  {0.2, QColor{142, 193, 218}},
                                    {0.3, QColor{205, 225, 236}}, {0.4, QColor{237, 237, 237}},
                                    {0.5, QColor{246, 214, 194}}, {0.6, QColor{246, 214, 194}},
                                    {0.7, QColor{246, 214, 194}}, {0.8, QColor{212, 114, 100}},
                                    {0.9, QColor{174, 40, 44}},   {1.01, QColor{174, 40, 44}}};

  /////////////////////////////////////////////////////
  static inline const uint32_t spacing                 = 4;
  static inline const uint32_t Y_TOP_MARING            = 16;
  static inline const uint32_t X_LEFT_MARGIN           = 10;
  static inline const uint32_t LEGEND_HEIGHT           = 30;
  static inline const uint32_t LEGEND_COLOR_ROW_HEIGHT = 15;
  static inline const uint32_t HEATMAP_FONT_SIZE       = 12;

  MatrixForm mForm      = MatrixForm::CIRCLE;
  uint32_t mRows        = 0;
  uint32_t mCols        = 0;
  int32_t mHoveredWell  = -1;
  int32_t mSelectedWell = -1;
  results::WellId mSelectedWellId;
};

///
/// \class      PanelHeatmap
/// \author     Joachim Danmayr
/// \brief      Heatmap panel
///
class PanelHeatmap : public QWidget
{
  Q_OBJECT

public:
  struct SelectedFilter
  {
    uint32_t plateRows = 0;
    uint32_t plateCols = 0;
    uint32_t plateId   = 1;
    joda::results::MeasureChannelId measureChannel;
    joda::results::Stats stats;
  };

  /////////////////////////////////////////////////////
  PanelHeatmap(QMainWindow *win, QWidget *parent);
  void setData(std::shared_ptr<joda::results::Analyzer> analyzer, const SelectedFilter &);

private:
  /////////////////////////////////////////////////////
  enum class Navigation
  {
    PLATE = 0,
    WELL  = 1,
    IMAGE = 2
  };

  /////////////////////////////////////////////////////
  ChartHeatMap *mHeatmap01;
  std::shared_ptr<joda::results::Analyzer> mAnalyzer;
  SelectedFilter mFilter;

  Navigation mNavigation = Navigation::PLATE;

private slots:
  void onOpenWellClicked();
};

}    // namespace joda::ui::qt::reporting::plugin
