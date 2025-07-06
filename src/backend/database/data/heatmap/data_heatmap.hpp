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

#include <qlabel.h>
#include <memory>
#include <string>
#include "backend/helper/table/table.hpp"

namespace joda::db::data {

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
  int32_t densityMapSize = -1;
};

auto convertToHeatmap(const std::shared_ptr<joda::table::Table> table, int32_t rows, int32_t cols, int32_t colToDisplay,
                      const PlotPlateSettings &settings) -> joda::table::Table;

}    // namespace joda::db::data
