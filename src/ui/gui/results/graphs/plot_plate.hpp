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

#pragma once

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

struct PlotPlateSettings
{
  ColormapName colorMap = ColormapName::BLUES;
};

auto preparePlateSurface(const joda::table::Table &table, int32_t rows, int32_t cols, int32_t colToDisplay, const PlotPlateSettings &settings,
                         std::shared_ptr<QtBackend> backend) -> std::map<Pos, int32_t>;

}    // namespace joda::ui::gui
