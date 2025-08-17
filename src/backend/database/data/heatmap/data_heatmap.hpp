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

struct PlotPlateSettings
{
  int32_t densityMapSize = -1;
};

auto convertToHeatmap(const joda::table::Table *table, uint32_t rows, uint32_t cols, uint32_t colToDisplay, int32_t tStackIn,
                      const PlotPlateSettings &settings) -> joda::table::Table;

}    // namespace joda::db::data
