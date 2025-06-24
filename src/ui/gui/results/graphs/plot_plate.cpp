

#include "plot_plate.hpp"
#include <string>

namespace joda::ui::gui {

void set_colormap_from_enum(matplot::axes_handle ax, ColormapName cmap);

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

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto preparePlateSurface(const joda::table::Table &table, int32_t rows, int32_t cols, int32_t colToDisplay, const PlotPlateSettings &settings,
                         std::shared_ptr<QtBackend> backend) -> std::map<Pos, int32_t>
{
  if(rows > 50 && cols > 50) {
    return {};
  }

  std::vector<std::vector<double>> data(rows, std::vector<double>(cols, std::numeric_limits<double>::quiet_NaN()));
  std::vector<std::string> xLabels;
  std::vector<std::string> yLabels;
  std::map<Pos, int32_t> posToRowMap;

  for(int x = 0; x < cols; x++) {
    xLabels.emplace_back(std::to_string(x + 1));
  }

  for(int y = 0; y < rows; y++) {
    yLabels.emplace_back(numberToExcelColumn(y + 1));
  }

  double mMin = std::numeric_limits<double>::max();
  double mMax = std::numeric_limits<double>::min();

  struct Element
  {
    double val  = 0;
    int32_t cnt = 0;
    int32_t tblRow;
  };

  std::map<Pos, Element> densityMapVal;

  for(int32_t tblRow = 0; tblRow < table.getRows(); tblRow++) {
    double val       = table.data(tblRow, colToDisplay).getVal();
    uint32_t posX    = table.data(tblRow, colToDisplay).getPosX();
    uint32_t posY    = table.data(tblRow, colToDisplay).getPosY();
    uint32_t tStack  = table.data(tblRow, colToDisplay).getStackT();
    uint64_t groupId = table.data(tblRow, colToDisplay).getGroupId();
    if(tStack == 0 && val == val && table.data(tblRow, colToDisplay).isValid()) {
      posX--;    // The maps start counting at 1
      posY--;    // The maps start counting at 1
      if(settings.densityMapSize > 0) {
        posX = posX / settings.densityMapSize;
        posY = posY / settings.densityMapSize;
      }

      densityMapVal[{posX, posY}].val += val;
      densityMapVal[{posX, posY}].cnt++;
      densityMapVal[{posX, posY}].tblRow = tblRow;
    }
  }

  //
  // Calc mean and forward to vector
  //
  for(const auto &mapEntry : densityMapVal) {
    double val     = mapEntry.second.val / static_cast<double>(mapEntry.second.cnt);
    uint32_t posY  = mapEntry.first.posY;
    uint32_t posX  = mapEntry.first.posX;
    int32_t tblRow = mapEntry.second.tblRow;
    if(data.size() > posY && posY >= 0) {
      if(data[posY].size() > posX && posX >= 0) {
        data[posY][posX] = val;
        posToRowMap.emplace(Pos{posX, posY}, tblRow);

        if(val < mMin) {
          mMin = val;
        }
        if(val > mMax) {
          mMax = val;
        }
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

  std::cout << "min " << std::to_string(mMin) << " max " << std::to_string(mMax) << std::endl;
  if(mMax > mMin) {
    ax->color_box_range(mMin, mMax);
  }
  set_colormap_from_enum(ax, settings.colorMap);

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

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
std::vector<std::vector<double>> burd(size_t n = 8)
{
  static std::vector<std::vector<double>> map = {{0.129412, 0.4, 0.67451},       {0.262745, 0.576471, 0.764706}, {0.572549, 0.772549, 0.870588},
                                                 {0.819608, 0.898039, 0.941176}, {0.992157, 0.858824, 0.780392}, {0.956863, 0.647059, 0.509804},
                                                 {0.839216, 0.376471, 0.301961}, {0.698039, 0.0941176, 0.168627}};
  if(n == 8) {
    return map;
  }
  std::vector<std::vector<double>> result;
  for(size_t i = 0; i < n; ++i) {
    std::array<float, 4> ac = matplot::colormap_interpolation(static_cast<double>(i), 0., static_cast<double>(n - 1), map);
    std::vector<double> vc  = {ac[1], ac[2], ac[3]};
    result.emplace_back(vc);
  }
  return result;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void set_colormap_from_enum(matplot::axes_handle ax, ColormapName cmap)
{
  using namespace matplot::palette;

  switch(cmap) {
    case ColormapName::ACCENT:
      ax->colormap(accent());
      break;
    case ColormapName::BLUES:
      ax->colormap(blues());
      break;
    case ColormapName::BRBG:
      ax->colormap(brbg());
      break;
    case ColormapName::BUGN:
      ax->colormap(bugn());
      break;
    case ColormapName::BUPU:
      ax->colormap(bupu());
      break;
    case ColormapName::CHROMAJS:
      ax->colormap(chromajs());
      break;
    case ColormapName::DARK2:
      ax->colormap(dark2());
      break;
    case ColormapName::GNBU:
      ax->colormap(gnbu());
      break;
    case ColormapName::GNPU:
      ax->colormap(gnpu());
      break;
    case ColormapName::GREENS:
      ax->colormap(greens());
      break;
    case ColormapName::GREYS:
      ax->colormap(greys());
      break;
    case ColormapName::INFERNO:
      ax->colormap(inferno());
      break;
    case ColormapName::JET:
      ax->colormap(jet());
      break;
    case ColormapName::TURBO:
      ax->colormap(turbo());
      break;
    case ColormapName::MAGMA:
      ax->colormap(magma());
      break;
    case ColormapName::ORANGES:
      ax->colormap(oranges());
      break;
    case ColormapName::ORRD:
      ax->colormap(orrd());
      break;
    case ColormapName::PAIRED:
      ax->colormap(paired());
      break;
    case ColormapName::PARULA:
      ax->colormap(parula());
      break;
    case ColormapName::PASTEL1:
      ax->colormap(pastel1());
      break;
    case ColormapName::PASTEL2:
      ax->colormap(pastel2());
      break;
    case ColormapName::PIYG:
      ax->colormap(piyg());
      break;
    case ColormapName::PLASMA:
      ax->colormap(plasma());
      break;
    case ColormapName::PRGN:
      ax->colormap(prgn());
      break;
    case ColormapName::PUBU:
      ax->colormap(pubu());
      break;
    case ColormapName::PUBUGN:
      ax->colormap(pubugn());
      break;
    case ColormapName::PUOR:
      ax->colormap(puor());
      break;
    case ColormapName::PURD:
      ax->colormap(purd());
      break;
    case ColormapName::PURPLES:
      ax->colormap(purples());
      break;
    case ColormapName::RDBU:
      ax->colormap(rdbu());
      break;
    case ColormapName::BURD:
      ax->colormap(burd());
      break;
    case ColormapName::RDGY:
      ax->colormap(rdgy());
      break;
    case ColormapName::RDPU:
      ax->colormap(rdpu());
      break;
    case ColormapName::RDYLBU:
      ax->colormap(rdylbu());
      break;
    case ColormapName::RDYLGN:
      ax->colormap(rdylgn());
      break;
    case ColormapName::REDS:
      ax->colormap(reds());
      break;
    case ColormapName::SAND:
      ax->colormap(sand());
      break;
    case ColormapName::SET1:
      ax->colormap(set1());
      break;
    case ColormapName::SET2:
      ax->colormap(set2());
      break;
    case ColormapName::SET3:
      ax->colormap(set3());
      break;
    case ColormapName::SPECTRAL:
      ax->colormap(spectral());
      break;
    case ColormapName::VIRIDIS:
      ax->colormap(viridis());
      break;
    case ColormapName::WHYLRD:
      ax->colormap(whylrd());
      break;
    case ColormapName::YLGN:
      ax->colormap(ylgn());
      break;
    case ColormapName::YLGNBU:
      ax->colormap(ylgnbu());
      break;
    case ColormapName::YLORBR:
      ax->colormap(ylorbr());
      break;
    case ColormapName::YLORRD:
      ax->colormap(ylorrd());
      break;
    case ColormapName::YLRD:
      ax->colormap(ylrd());
      break;
    case ColormapName::HSV:
      ax->colormap(hsv());
      break;
    case ColormapName::HOT:
      ax->colormap(hot());
      break;
    case ColormapName::COOL:
      ax->colormap(cool());
      break;
    case ColormapName::SPRING:
      ax->colormap(spring());
      break;
    case ColormapName::SUMMER:
      ax->colormap(summer());
      break;
    case ColormapName::AUTUMN:
      ax->colormap(autumn());
      break;
    case ColormapName::WINTER:
      ax->colormap(winter());
      break;
    case ColormapName::GRAY:
      ax->colormap(gray());
      break;
    case ColormapName::BONE:
      ax->colormap(bone());
      break;
    case ColormapName::COPPER:
      ax->colormap(copper());
      break;
    case ColormapName::PINK:
      ax->colormap(pink());
      break;
    case ColormapName::LINES:
      ax->colormap(lines());
      break;
    case ColormapName::COLORCUBE:
      ax->colormap(colorcube());
      break;
    case ColormapName::PRISM:
      ax->colormap(prism());
      break;
    case ColormapName::FLAG:
      ax->colormap(flag());
      break;
    case ColormapName::WHITE:
      ax->colormap(white());
      break;
    case ColormapName::DEFAULT_MAP:
      ax->colormap(default_map());
      break;
    case ColormapName::DEFAULT_COLORS_MAP:
      ax->colormap(default_colors_map());
      break;
    default:
      ax->colormap(default_map());
      break;
  }
}

}    // namespace joda::ui::gui
