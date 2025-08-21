///
/// \file      exporter_r.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-04
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <vector>
#include "../exportable.hpp"
#include "backend/settings/analze_settings.hpp"

namespace joda::exporter::r {

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class Exporter
{
public:
  /////////////////////////////////////////////////////
  static void startExport(const std::vector<const Exportable *> &data, const settings::AnalyzeSettings &analyzeSettings, const std::string &jobName,
                          std::chrono::system_clock::time_point timeStarted, std::chrono::system_clock::time_point timeFinished,
                          const std::string &unit, const std::string &outputFileName);
};
}    // namespace joda::exporter::r
