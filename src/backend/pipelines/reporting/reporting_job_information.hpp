///
/// \file      reporting_job_information.xlsx.hpp
/// \author    Joachim Danmayr
/// \date      2024-04-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Write job information
///

#pragma once

#include <chrono>
#include <map>
#include <string>
#include <tuple>
#include "backend/results/results.hpp"
#include "backend/results/results_defines.hpp"
#include "xlsxwriter.h"

namespace joda::results {
class TableWorkbook;
}

namespace joda::settings {
class AnalyzeSettings;
}

namespace joda::pipeline::reporting {

///
/// \class      OverviewReport
/// \author     Joachim Danmayr
/// \brief      Generates an ocerview report
///
class JobInformation
{
public:
  static void writeReport(const joda::settings::AnalyzeSettings &analyzeSettings,
                          const joda::results::WorkSheet &results, const joda::results::WorkSheet::Meta &meta,
                          lxw_worksheet *worksheet, lxw_format *header, lxw_format *fontNormal);
};

}    // namespace joda::pipeline::reporting
