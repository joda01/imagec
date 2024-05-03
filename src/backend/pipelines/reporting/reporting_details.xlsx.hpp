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
#include "backend/results/results.hpp"
#include "backend/settings/channel/channel_reporting_settings.hpp"
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
  static std::tuple<int, int> writeReport(const joda::settings::ChannelReportingSettings &reportingSettings,
                                          const joda::results::Channel &results, int colOffset, int /*rowOffset*/,
                                          lxw_worksheet *worksheet, lxw_format *header, lxw_format *headerInvalid,
                                          lxw_format *merge_format, lxw_format *numberFormat,
                                          lxw_format *numberFormatInvalid);
};

}    // namespace joda::pipeline::reporting
