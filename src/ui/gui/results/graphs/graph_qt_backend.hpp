///
/// \file      graph_qt_backend.hpp
/// \author    Joachim Danmayr
/// \date      2025-06-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <matplot/backend/backend_interface.h>
#include <matplot/backend/gnuplot.h>
#include <qopenglfunctions_3_3_core.h>
#include <qopenglversionfunctions.h>
#include <qpoint.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QSvgRenderer>
#include <iostream>
#include <mutex>
#include <vector>

namespace joda::ui::gui {

class QtBackend : public QWidget, public matplot::backend::gnuplot
{
  Q_OBJECT

public:
  QtBackend(const std::string &terminal, QWidget *parent);
  ~QtBackend() override;

  /////////////////////////////////////////////////////
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void setNrOfRowsAndCols(int32_t rows, int32_t cols)
  {
    mRows = rows;
    mCols = cols;
  }
signals:
  void onGraphClicked(int row, int col);
  void onGraphDoubleClicked(int row, int col);

private:
  bool render_data() override;

  /////////////////////////////////////////////////////
  QSvgRenderer *svgRenderer = nullptr;
  QRectF targetRect;
  std::mutex mPaintMutex;
  int32_t mRows = 0;
  int32_t mCols = 0;
  std::vector<std::pair<QRectF, QPoint>> mRects;
  int32_t mSelectedIndex = -1;
};
}    // namespace joda::ui::gui
