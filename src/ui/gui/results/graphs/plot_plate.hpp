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
#include "graph_qt_backend.hpp"

namespace joda::ui::gui {

struct PlotPlateSettings
{
  int32_t densityMapSize = -1;
};

auto preparePlateSurface(const joda::table::Table &table, int32_t rows, int32_t cols, int32_t colToDisplay, const PlotPlateSettings &settings,
                         std::shared_ptr<QtBackend> backend) -> std::vector<std::vector<double>>;

}    // namespace joda::ui::gui
