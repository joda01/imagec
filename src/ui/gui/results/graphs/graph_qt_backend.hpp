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
#include <mutex>
#include <vector>

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

struct Pos
{
  uint32_t posX = 0;
  uint32_t posY = 0;

  bool operator<(const Pos &in) const
  {
    uint64_t tmp  = static_cast<uint64_t>(posX) << 32 | posY;
    uint64_t tmp2 = static_cast<uint64_t>(in.posX) << 32 | in.posY;
    return tmp < tmp2;
  };
};

class QtBackend : public QCustomPlot
{
  Q_OBJECT

public:
  QtBackend(QWidget *parent);
  ~QtBackend() override;

  /////////////////////////////////////////////////////
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void setNrOfRowsAndCols(int32_t rows, int32_t cols, const std::vector<std::vector<double>> &data);

signals:
  void onGraphClicked(int row, int col);
  void onGraphDoubleClicked(int row, int col);

private:
  /////////////////////////////////////////////////////
  QCPColorMap *mColorMap;
  QSvgRenderer *svgRenderer = nullptr;
  QRectF targetRect;
  std::mutex mPaintMutex;
  int32_t mRows = 0;
  int32_t mCols = 0;
  std::vector<std::pair<QRectF, QPoint>> mRects;
  int32_t mSelectedIndex = -1;
};
}    // namespace joda::ui::gui
