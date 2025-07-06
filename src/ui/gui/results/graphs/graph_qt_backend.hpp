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

#include <qpoint.h>
#include <qtmetamacros.h>
#include <qwidget.h>
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

class QtBackend : public QWidget
{
  Q_OBJECT

public:
  QtBackend(QWidget *parent);
  ~QtBackend() override;
  /////////////////////////////////////////////////////
  void updateGraph(const joda::table::Table &&data, bool isImageView);

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
