///
/// \file      reporting_generator.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <map>
#include <mutex>
#include <stdexcept>
#include <string>
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/results/results_container.hpp"
#include "backend/results/results_defines.hpp"
#include "backend/settings/analze_settings.hpp"

namespace joda::pipeline::reporting {

class ReportGenerator
{
public:
  enum class OutputFormat
  {
    VERTICAL,
    HORIZONTAL
  };

  static void flushReportToFile(const joda::settings::AnalyzeSettings &analyzeSettings,
                                const joda::results::TableWorkBook &resultsWorkbook, const std::string &fileName,
                                const joda::results::JobMeta &meta, OutputFormat format, bool writeRunMeta);
};

}    // namespace joda::pipeline::reporting
