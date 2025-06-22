///
/// \file      plot_plate.hpp
/// \author    Joachim Danmayr
/// \date      2025-06-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include <matplot/core/figure_registry.h>
#include <matplot/freestanding/plot.h>
#include <matplot/matplot.h>
#include <qlabel.h>
#include <memory>
#include "backend/helper/table/table.hpp"
#include "graph_qt_backend.hpp"

namespace joda::ui::gui {

auto preparePlateSurface(const joda::table::Table &table, int32_t rows, int32_t cols, std::shared_ptr<QtBackend> backend)
{
  for(int32_t tblRow = 0; tblRow < table.getRows(); tblRow++) {
    double val      = table.data(tblRow, 0).getVal();
    uint32_t posX   = table.data(tblRow, 0).getPosX();
    uint32_t posY   = table.data(tblRow, 0).getPosY();
    uint32_t tStack = table.data(tblRow, 0).getStackT();
  }

  // Generate a plot with matplot++
  auto fig = matplot::figure_no_backend(true);    // create figure but don't show a window

  fig->backend(backend);
  auto ax = fig->current_axes();
  ax->plot({1, 2, 3, 4}, {1, 4, 9, 16});
  ax->title("Simple Plot");
  ax->xlabel("X");
  ax->ylabel("Y");

  fig->draw();

  // Convert to QImage
  // QImage qimg(img.data(), fig->width(), fig->height(), QImage::Format_RGBA8888);
  // graphContainer->setPixmap(QPixmap::fromImage(qimg));
  // graphContainer->show();
}

}    // namespace joda::ui::gui
