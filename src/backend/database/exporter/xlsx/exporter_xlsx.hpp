///
/// \file      exporter_xlsx.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-03
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

namespace joda::exporter::xlsx {

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
                          const std::string &outputFileName);

private:
  /////////////////////////////////////////////////////
  struct WorkBook
  {
    lxw_workbook *workbook;
    lxw_format *header;
    lxw_format *headerInvalid;
    lxw_format *imageHeaderHyperlinkFormat;
    lxw_format *imageHeaderHyperlinkFormatInvalid;
    lxw_format *merge_format;
    lxw_format *headerBold;
    lxw_format *idFormat;
    lxw_format *fontNormal;
    lxw_format *numberFormat;
    lxw_format *numberFormatInvalid;
    lxw_format *numberFormatScientific;
    lxw_format *numberFormatInvalidScientific;
  };

  /////////////////////////////////////////////////////
  static WorkBook createWorkBook(std::string outputFileName);
  static void createAnalyzeSettings(WorkBook &workbookSettings, const settings::AnalyzeSettings &settings, const std::string &jobName,
                                    std::chrono::system_clock::time_point timeStarted, std::chrono::system_clock::time_point timeFinished);
  static void writeWorkSheet(const Exporter::WorkBook &, const Exportable *, int32_t index);
};

}    // namespace joda::exporter::xlsx
