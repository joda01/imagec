///
/// \file      heatmap_widget.hpp
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
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>
#include "backend/helper/image/image.hpp"
#include "backend/helper/table/table.hpp"
#include "backend/plot/plot_base.hpp"

namespace joda::plot {
class Heatmap;
}    // namespace joda::plot

namespace joda::ui::gui {

class HeatmapWidget : public QWidget
{
  Q_OBJECT

public:
  HeatmapWidget(QWidget *parent);
  ~HeatmapWidget() override;
  /////////////////////////////////////////////////////
  void updateGraph(const joda::table::Table &data, joda::plot::ColormapName colorMap, joda::plot::ColorMappingMode mode,
                   const joda::plot::ColorMappingRange &range, bool isPlateView, bool isImageView);
  void exportToPNG(const std::filesystem::path &) const;
  void copyToClipboard() const;
  auto getColorMapRange() const -> joda::plot::ColorMappingRange;

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
