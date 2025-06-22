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
#include <matplot/util/colors.h>
#include <qlabel.h>
#include <memory>
#include <string>
#include "backend/helper/table/table.hpp"
#include "graph_qt_backend.hpp"

namespace joda::ui::gui {

auto preparePlateSurface(const joda::table::Table &table, int32_t rows, int32_t cols, std::shared_ptr<QtBackend> backend)
{
  std::vector<std::vector<double>> data(cols, std::vector<double>(rows, 0.0));
  std::vector<std::string> xLabels;
  std::vector<std::string> yLabels;

  xLabels.reserve(cols);
  for(int x = 0; x < cols; x++) {
    xLabels.emplace_back(std::to_string(x + 1));
  }

  yLabels.reserve(rows);
  for(int y = 0; y < rows; y++) {
    char toPrint = y + 'A';
    yLabels.emplace_back(std::string(1, toPrint));
  }

  for(int32_t tblRow = 0; tblRow < table.getRows(); tblRow++) {
    double val      = table.data(tblRow, 0).getVal();
    uint32_t posX   = table.data(tblRow, 0).getPosX();
    uint32_t posY   = table.data(tblRow, 0).getPosY();
    uint32_t tStack = table.data(tblRow, 0).getStackT();
    if(tStack == 0) {
      data[posX - 1][posY - 1] = val;
    }
  }

  // Generate a plot with matplot++
  auto fig = matplot::figure_no_backend(true);    // create figure but don't show a window

  fig->backend(backend);
  auto ax = fig->current_axes();

  ax->x_axis().ticklabels(xLabels).label_font_size(8);
  ax->y_axis().ticklabels(yLabels).label_font_size(8);
  ax->font_size(9);

  auto h = ax->heatmap(data);
  // h->normalization(matplot::matrix::color_normalization::columns);
  ax->colormap(matplot::palette::default_map());

  h->always_hide_labels(true);
  for(size_t i = 0; i < data.size(); ++i) {
    for(size_t j = 0; j < data[i].size(); ++j) {
      // Format number with fixed 2 decimals
      std::ostringstream oss;
      oss << std::fixed << std::setprecision(2) << data[i][j];
      // Add text at cell center with formatted number
      ax->text(j + 1, i + 1, oss.str())->font_size(5).alignment(matplot::labels::alignment::center).tag("myTag1" + std::to_string(i));
    }
  }

  std::filesystem::path tempDir = std::filesystem::temp_directory_path();
  tempDir                       = tempDir / "imagec_temp_graph.svg";
  fig->save(tempDir.string());
}

}    // namespace joda::ui::gui
