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

#include "dialog_channel_measurment.hpp"
#include <qboxlayout.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <QMessageBox>
#include <exception>
#include <string>
#include <vector>
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::qt {

struct Temp
{
  std::vector<std::vector<int32_t>> order;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Temp, order);
};

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogChannelMeasurement::DialogChannelMeasurement(QWidget *windowMain,
                                                   joda::settings::ChannelReportingSettings &reportingSettings) :
    QDialog(windowMain),
    mReportingSettings(reportingSettings)
{
  setWindowTitle("Settings");
  setBaseSize(500, 200);

  auto *mainLayout = new QVBoxLayout(this);
  auto *groupBox   = new QGroupBox("Measurements", this);
  auto *gridLayout = new QGridLayout(groupBox);

  gridLayout->addWidget(new QLabel("Detail"), 0, 1, Qt::AlignCenter);
  gridLayout->addWidget(new QLabel("Overview"), 0, 2, Qt::AlignCenter);
  gridLayout->addWidget(new QLabel("Heatmap"), 0, 3, Qt::AlignCenter);

  int row               = 1;
  auto createCheckBoxes = [this, &reportingSettings, &gridLayout, &groupBox,
                           &row](joda::settings::ChannelReportingSettings::MeasureChannels type,
                                 const std::string &description) {
    gridLayout->addWidget(new QLabel(description.data()), row, 0);

    QCheckBox *onOffDetail = new QCheckBox(groupBox);
    gridLayout->addWidget(onOffDetail, row, 1, Qt::AlignCenter);
    mMeasurementDetailsReport.emplace(type, onOffDetail);

    QCheckBox *onOffOverview = new QCheckBox(groupBox);
    gridLayout->addWidget(onOffOverview, row, 2, Qt::AlignCenter);
    mMeasurementOverViewReport.emplace(type, onOffOverview);

    QCheckBox *onOffHeatmap = new QCheckBox(groupBox);
    gridLayout->addWidget(onOffHeatmap, row, 3, Qt::AlignCenter);
    mMeasurementHeatmapReport.emplace(type, onOffHeatmap);

    if(reportingSettings.detail.measureChannels.contains(type)) {
      onOffDetail->setChecked(true);
    } else {
      onOffDetail->setChecked(false);
    }

    if(reportingSettings.overview.measureChannels.contains(type)) {
      onOffOverview->setChecked(true);
    } else {
      onOffOverview->setChecked(false);
    }

    if(reportingSettings.heatmap.measureChannels.contains(type)) {
      onOffHeatmap->setChecked(true);
    } else {
      onOffHeatmap->setChecked(false);
    }

    row++;
  };

  createCheckBoxes(joda::settings::ChannelReportingSettings::MeasureChannels::CONFIDENCE, "Confidence");
  createCheckBoxes(joda::settings::ChannelReportingSettings::MeasureChannels::AREA_SIZE, "Area size");
  createCheckBoxes(joda::settings::ChannelReportingSettings::MeasureChannels::PERIMETER, "Perimeter");
  createCheckBoxes(joda::settings::ChannelReportingSettings::MeasureChannels::CIRCULARITY, "Circularity");
  createCheckBoxes(joda::settings::ChannelReportingSettings::MeasureChannels::VALIDITY, "Validity");
  createCheckBoxes(joda::settings::ChannelReportingSettings::MeasureChannels::INVALIDITY, "Invalidity");
  createCheckBoxes(joda::settings::ChannelReportingSettings::MeasureChannels::CENTER_OF_MASS_X, "Center of mass X");
  createCheckBoxes(joda::settings::ChannelReportingSettings::MeasureChannels::CENTER_OF_MASS_Y, "Center of mass Y");
  createCheckBoxes(joda::settings::ChannelReportingSettings::MeasureChannels::INTENSITY_AVG, "Intensity AVG");
  createCheckBoxes(joda::settings::ChannelReportingSettings::MeasureChannels::INTENSITY_MIN, "Intensity MIN");
  createCheckBoxes(joda::settings::ChannelReportingSettings::MeasureChannels::INTENSITY_MAX, "Intensity MAX");
  createCheckBoxes(joda::settings::ChannelReportingSettings::MeasureChannels::INTENSITY_AVG_CROSS_CHANNEL,
                   "Cross ch. intensity avg");
  createCheckBoxes(joda::settings::ChannelReportingSettings::MeasureChannels::INTENSITY_MIN_CROSS_CHANNEL,
                   "Cross ch. intensity min");
  createCheckBoxes(joda::settings::ChannelReportingSettings::MeasureChannels::INTENSITY_MAX_CROSS_CHANNEL,
                   "Cross ch. intensity max");
  createCheckBoxes(joda::settings::ChannelReportingSettings::MeasureChannels::INTERSECTION_CROSS_CHANNEL,
                   "Cross ch. count");

  mainLayout->addWidget(groupBox);
}

int DialogChannelMeasurement::exec()
{
  int ret = QDialog::exec();

  std::set<joda::settings::ChannelReportingSettings::MeasureChannels> details;
  std::set<joda::settings::ChannelReportingSettings::MeasureChannels> overview;
  std::set<joda::settings::ChannelReportingSettings::MeasureChannels> heatmap;

  for(auto const &[key, val] : mMeasurementDetailsReport) {
    if(val->isChecked()) {
      details.emplace(key);
    }
  }

  for(auto const &[key, val] : mMeasurementOverViewReport) {
    if(val->isChecked()) {
      overview.emplace(key);
    }
  }

  for(auto const &[key, val] : mMeasurementHeatmapReport) {
    if(val->isChecked()) {
      heatmap.emplace(key);
    }
  }

  mReportingSettings.detail.measureChannels   = details;
  mReportingSettings.overview.measureChannels = overview;
  mReportingSettings.heatmap.measureChannels  = heatmap;

  return ret;
}

void DialogChannelMeasurement::onOkayClicked()
{
}
void DialogChannelMeasurement::onCancelClicked()
{
}

}    // namespace joda::ui::qt
