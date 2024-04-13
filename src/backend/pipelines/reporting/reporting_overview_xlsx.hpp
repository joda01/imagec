///
/// \file      reporting_overview.xlsx.hpp
/// \author    Joachim Danmayr
/// \date      2024-04-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Generates a overview view report
///

#pragma once

#include <tuple>
#include "backend/results/results.h"
#include "backend/settings/channel/channel_reporting_settings.hpp"
#include "xlsxwriter.h"

namespace joda::pipeline::reporting {

///
/// \class      OverviewReport
/// \author     Joachim Danmayr
/// \brief      Generates an ocerview report
///
class OverviewReport
{
public:
  static std::tuple<int, int> writeReport(const joda::settings::ChannelReportingSettings &reportingSettings,
                                          const joda::results::Table &results, const std::string &headerText,
                                          const std::string &jobName, int colOffset, int rowOffset, int startRow,
                                          lxw_worksheet *worksheet, lxw_format *header, lxw_format *merge_format,
                                          lxw_format *numberFormat, lxw_format *imageHeaderHyperlinkFormat);
};

}    // namespace joda::pipeline::reporting
