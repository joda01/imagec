///
/// \file      common.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <string>
#include "backend/results/db_column_ids.hpp"

namespace joda::results::analyze::plugins {

inline std::string getStatsString(Stats stats)
{
  std::string statsStr;
  switch(stats) {
    case Stats::AVG:
      statsStr = "  AVG(element_at(values, $1)[1]) as val_img ";
      break;
    case Stats::MEDIAN:
      statsStr = "  MEDIAN(element_at(values, $1)[1]) as val_img ";
      break;
    case Stats::SUM:
      statsStr = "  SUM(element_at(values, $1)[1]) as val_img ";
      break;
    case Stats::MIN:
      statsStr = "  MIN(element_at(values, $1)[1]) as val_img ";
      break;
    case Stats::MAX:
      statsStr = "  MAX(element_at(values, $1)[1]) as val_img ";
      break;
    case Stats::STDDEV:
      statsStr = "  STDDEV(element_at(values, $1)[1]) as val_img ";
      break;
  };
  return statsStr;
}

}    // namespace joda::results::analyze::plugins
