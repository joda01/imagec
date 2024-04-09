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
#include "backend/pipelines/reporting/reporting_defines.hpp"
#include "backend/pipelines/reporting/reporting_helper.hpp"
#include "backend/settings/analze_settings_parser.hpp"
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
DialogChannelMeasurement::DialogChannelMeasurement(settings::json::ReportingSettings *reportingSettings,
                                                   QWidget *windowMain) :
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
                           &row](joda::pipeline::reporting::MeasurementChannels type, const std::string &description) {
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

    if(reportingSettings->getDetailReportSettings().getMeasurementChannels().contains((int32_t) type)) {
      onOffDetail->setChecked(true);
    } else {
      onOffDetail->setChecked(false);
    }

    if(reportingSettings->getOverviewReportSettings().getMeasurementChannels().contains((int32_t) type)) {
      onOffOverview->setChecked(true);
    } else {
      onOffOverview->setChecked(false);
    }

    if(reportingSettings->getHeatmapSettings().getMeasurementChannels().contains((int32_t) type)) {
      onOffHeatmap->setChecked(true);
    } else {
      onOffHeatmap->setChecked(false);
    }

    row++;
  };

  createCheckBoxes(joda::pipeline::reporting::MeasurementChannels::CONFIDENCE, "Confidence");
  createCheckBoxes(joda::pipeline::reporting::MeasurementChannels::AREA_SIZE, "Area size");
  createCheckBoxes(joda::pipeline::reporting::MeasurementChannels::PERIMETER, "Perimeter");
  createCheckBoxes(joda::pipeline::reporting::MeasurementChannels::CIRCULARITY, "Circularity");
  createCheckBoxes(joda::pipeline::reporting::MeasurementChannels::VALIDITY, "Validity");
  createCheckBoxes(joda::pipeline::reporting::MeasurementChannels::INVALIDITY, "Invalidity");
  createCheckBoxes(joda::pipeline::reporting::MeasurementChannels::CENTER_OF_MASS_X, "Center of mass X");
  createCheckBoxes(joda::pipeline::reporting::MeasurementChannels::CENTER_OF_MASS_Y, "Center of mass Y");
  createCheckBoxes(joda::pipeline::reporting::MeasurementChannels::INTENSITY_AVG, "Intensity AVG");
  createCheckBoxes(joda::pipeline::reporting::MeasurementChannels::INTENSITY_MIN, "Intensity MIN");
  createCheckBoxes(joda::pipeline::reporting::MeasurementChannels::INTENSITY_MAX, "Intensity MAX");
  createCheckBoxes(joda::pipeline::reporting::MeasurementChannels::INTENSITY_AVG_CROSS_CHANNEL,
                   "Cross ch. intensity avg");
  createCheckBoxes(joda::pipeline::reporting::MeasurementChannels::INTENSITY_MIN_CROSS_CHANNEL,
                   "Cross ch. intensity min");
  createCheckBoxes(joda::pipeline::reporting::MeasurementChannels::INTENSITY_MAX_CROSS_CHANNEL,
                   "Cross ch. intensity max");
  createCheckBoxes(joda::pipeline::reporting::MeasurementChannels::INTERSECTION_CROSS_CHANNEL, "Cross ch. count");

  mainLayout->addWidget(groupBox);
}

int DialogChannelMeasurement::exec()
{
  int ret = QDialog::exec();

  std::set<uint32_t> details;
  std::set<uint32_t> overview;
  std::set<uint32_t> heatmap;

  for(auto const &[key, val] : mMeasurementDetailsReport) {
    if(val->isChecked()) {
      details.emplace((uint32_t) key);
    }
  }

  for(auto const &[key, val] : mMeasurementOverViewReport) {
    if(val->isChecked()) {
      overview.emplace((uint32_t) key);
    }
  }

  for(auto const &[key, val] : mMeasurementHeatmapReport) {
    if(val->isChecked()) {
      heatmap.emplace((uint32_t) key);
    }
  }

  mReportingSettings->setReportingSettings(details, overview, heatmap);

  return ret;
}

void DialogChannelMeasurement::onOkayClicked()
{
}
void DialogChannelMeasurement::onCancelClicked()
{
}

}    // namespace joda::ui::qt
