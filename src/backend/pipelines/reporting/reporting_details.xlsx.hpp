///
/// \file      reporting_details.xlsx.hpp
/// \author    Joachim Danmayr
/// \date      2024-04-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Generates a detail view report
///

#pragma once

#include <tuple>
#include "backend/results/results.h"
#include "xlsxwriter.h"

namespace joda::pipeline::reporting {

///
/// \class      DetailReport
/// \author     Joachim Danmayr
/// \brief      Generates a detail report
///
class DetailReport
{
public:
  static std::tuple<int, int> writeReport(const joda::results::Table &results, int colOffset, int /*rowOffset*/,
                                          lxw_worksheet *worksheet, lxw_format *header, lxw_format *merge_format,
                                          lxw_format *numberFormat);
};

}    // namespace joda::pipeline::reporting