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

auto preparePlateSurface(const joda::table::Table &table, int32_t rows, int32_t cols, std::shared_ptr<QtBackend> backend) -> std::map<Pos, int32_t>;

}    // namespace joda::ui::gui
