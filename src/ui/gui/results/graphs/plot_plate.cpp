

#include "plot_plate.hpp"
#include <string>

namespace joda::ui::gui {

std::string numberToExcelColumn(int number)
{
  std::string result;

  while(number > 0) {
    number--;    // Excel columns are 1-based, but internally it's 0-based
    char ch = 'A' + (number % 26);
    result  = ch + result;
    number /= 26;
  }

  return result;
}

auto preparePlateSurface(const joda::table::Table &table, int32_t rows, int32_t cols, std::shared_ptr<QtBackend> backend) -> std::map<Pos, int32_t>
{
  std::vector<std::vector<double>> data(rows, std::vector<double>(cols, 0.0));
  std::vector<std::string> xLabels;
  std::vector<std::string> yLabels;
  std::map<Pos, int32_t> posToRowMap;

  for(int x = 0; x < cols; x++) {
    xLabels.emplace_back(std::to_string(x + 1));
  }

  for(int y = 0; y < rows; y++) {
    yLabels.emplace_back(numberToExcelColumn(y + 1));
  }

  for(int32_t tblRow = 0; tblRow < table.getRows(); tblRow++) {
    double val       = table.data(tblRow, 0).getVal();
    uint32_t posX    = table.data(tblRow, 0).getPosX();
    uint32_t posY    = table.data(tblRow, 0).getPosY();
    uint32_t tStack  = table.data(tblRow, 0).getStackT();
    uint64_t groupId = table.data(tblRow, 0).getGroupId();

    if(tStack == 0 && data.size() >= posY && posY > 0) {
      if(data[posY - 1].size() >= posX && posX > 0) {
        data[posY - 1][posX - 1] = val;
        posToRowMap.emplace(Pos{posX - 1, posY - 1}, tblRow);
      }
    }
  }

  // Generate a plot with matplot++
  auto fig = matplot::figure_no_backend(true);    // create figure but don't show a window
  fig->backend(backend);
  backend->setNrOfRowsAndCols(rows, cols);
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
      oss << std::fixed << std::setprecision(0) << data[i][j];
      // Add text at cell center with formatted number
      auto tx = ax->text(j + 1, i + 1, oss.str());
      tx->font_size(5);
      tx->alignment(matplot::labels::alignment::center);
    }
  }

  std::filesystem::path tempDir = std::filesystem::temp_directory_path();
  tempDir                       = tempDir / "imagec_temp_graph.svg";
  fig->save(tempDir.string());

  return posToRowMap;
}
}    // namespace joda::ui::gui
