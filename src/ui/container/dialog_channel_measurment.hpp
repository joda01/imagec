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
#include "backend/settings/channel/channel_reporting_settings.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::qt {

class DialogChannelMeasurement : public QDialog
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogChannelMeasurement(QWidget *windowMain, joda::settings::ChannelReportingSettings &reportingSettings);
  int exec() override;

private:
  std::map<joda::settings::ChannelReportingSettings::MeasureChannels, QCheckBox *> mMeasurementOverViewReport;
  std::map<joda::settings::ChannelReportingSettings::MeasureChannels, QCheckBox *> mMeasurementDetailsReport;
  std::map<joda::settings::ChannelReportingSettings::MeasureChannels, QCheckBox *> mMeasurementHeatmapReport;

  joda::settings::ChannelReportingSettings &mReportingSettings;

private slots:
  void onOkayClicked();
  void onCancelClicked();
};

}    // namespace joda::ui::qt
