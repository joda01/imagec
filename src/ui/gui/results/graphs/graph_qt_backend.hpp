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

#include <qcustomplot.h>
#include <qopenglfunctions_3_3_core.h>
#include <qopenglversionfunctions.h>
#include <qpoint.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QSvgRenderer>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>
#include "backend/helper/image/image.hpp"
#include "backend/helper/table/table.hpp"

namespace joda::plot {
class Heatmap;
}    // namespace joda::plot

namespace joda::ui::gui {

enum class ColormapName
{
  ACCENT,
  BLUES,
  BRBG,
  BUGN,
  BUPU,
  CHROMAJS,
  DARK2,
  GNBU,
  GNPU,
  GREENS,
  GREYS,
  INFERNO,
  JET,
  TURBO,
  MAGMA,
  ORANGES,
  ORRD,
  PAIRED,
  PARULA,
  PASTEL1,
  PASTEL2,
  PIYG,
  PLASMA,
  PRGN,
  PUBU,
  PUBUGN,
  PUOR,
  PURD,
  PURPLES,
  RDBU,
  BURD,
  RDGY,
  RDPU,
  RDYLBU,
  RDYLGN,
  REDS,
  SAND,
  SET1,
  SET2,
  SET3,
  SPECTRAL,
  VIRIDIS,
  WHYLRD,
  YLGN,
  YLGNBU,
  YLORBR,
  YLORRD,
  YLRD,
  HSV,
  HOT,
  COOL,
  SPRING,
  SUMMER,
  AUTUMN,
  WINTER,
  GRAY,
  BONE,
  COPPER,
  PINK,
  LINES,
  COLORCUBE,
  PRISM,
  FLAG,
  WHITE,
  DEFAULT_MAP,
  DEFAULT_COLORS_MAP
};

class QtBackend : public QWidget
{
  Q_OBJECT

public:
  QtBackend(QWidget *parent);
  ~QtBackend() override;
  /////////////////////////////////////////////////////
  void updateGraph(const joda::table::Table &&data);

signals:
  void onGraphClicked(joda::table::TableCell);
  void onGraphDoubleClicked(joda::table::TableCell);

private:
  /////////////////////////////////////////////////////
  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

  /////////////////////////////////////////////////////
  std::mutex mPaintMutex;
  int32_t mRows = 0;
  int32_t mCols = 0;
  std::unique_ptr<joda::plot::Heatmap> mHeatmap;
  QPoint mPixmapTopLeft;
  QSize mPixmapSize;
};
}    // namespace joda::ui::gui
