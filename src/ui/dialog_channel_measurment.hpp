///
/// \file      dialog_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-29
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qtmetamacros.h>
#include <qwindow.h>
#include <memory>
#include <thread>
#include "backend/pipelines/reporting/reporting_helper.hpp"
#include "backend/settings/analze_settings_parser.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::qt {

class DialogChannelMeasurement : public QDialog
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogChannelMeasurement(QWidget *windowMain);
  void fromJson(const settings::json::AnalyzeSettingsReporting &settings);
  nlohmann::json toJson();

private:
  std::map<joda::pipeline::reporting::Helper::ColumnIndexDetailedReport, QCheckBox *> mMeasurementOverViewReport;
  std::map<joda::pipeline::reporting::Helper::ColumnIndexDetailedReport, QCheckBox *> mMeasurementDetailsReport;
  std::map<joda::pipeline::reporting::Helper::ColumnIndexDetailedReport, QCheckBox *> mMeasurementHeatmapReport;

private slots:
  void onOkayClicked();
  void onCancelClicked();
};

}    // namespace joda::ui::qt
