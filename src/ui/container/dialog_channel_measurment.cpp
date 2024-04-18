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
#include <optional>
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

using Base  = joda::settings::ChannelReportingSettings::MeasureChannels;
using Combi = joda::settings::ChannelReportingSettings::MeasureChannelsCombi;

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

                           &row](std::optional<Base> detail, Combi type, const std::string &description) {
    gridLayout->addWidget(new QLabel(description.data()), row, 0);

    QCheckBox *onOffOverview = new QCheckBox(groupBox);
    gridLayout->addWidget(onOffOverview, row, 2, Qt::AlignCenter);
    mMeasurementOverViewReport.emplace(type, onOffOverview);

    QCheckBox *onOffHeatmap = new QCheckBox(groupBox);
    gridLayout->addWidget(onOffHeatmap, row, 3, Qt::AlignCenter);
    mMeasurementHeatmapReport.emplace(type, onOffHeatmap);

    if(detail.has_value()) {
      QCheckBox *onOffDetail = new QCheckBox(groupBox);
      if(reportingSettings.detail.measureChannels.contains(detail.value())) {
        onOffDetail->setChecked(true);
      } else {
        onOffDetail->setChecked(false);
      }
      mMeasurementDetailsReport.emplace(detail.value(), onOffDetail);
      gridLayout->addWidget(onOffDetail, row, 1, Qt::AlignCenter);
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

  createCheckBoxes(Base::CONFIDENCE, Combi::CONFIDENCE_AVG, "Confidence");
  createCheckBoxes(Base::AREA_SIZE, Combi::AREA_SIZE_AVG, "Area size");
  createCheckBoxes(Base::PERIMETER, Combi::PERIMETER_AVG, "Perimeter");
  createCheckBoxes(Base::CIRCULARITY, Combi::CIRCULARITY_AVG, "Circularity");
  createCheckBoxes(Base::VALIDITY, Combi::VALIDITY_AVG, "Validity");
  createCheckBoxes(Base::INVALIDITY, Combi::INVALIDITY_AVG, "Invalidity");
  createCheckBoxes(Base::CENTER_OF_MASS_X, Combi::CENTER_OF_MASS_X_AVG, "Center of mass X");
  createCheckBoxes(Base::CENTER_OF_MASS_Y, Combi::CENTER_OF_MASS_Y_AVG, "Center of mass Y");
  createCheckBoxes(Base::INTENSITY_AVG, Combi::INTENSITY_AVG_AVG, "Intensity AVG");
  createCheckBoxes(Base::INTENSITY_MIN, Combi::INTENSITY_MIN_AVG, "Intensity MIN");
  createCheckBoxes(Base::INTENSITY_MAX, Combi::INTENSITY_MAX_AVG, "Intensity MAX");
  createCheckBoxes(Base::INTENSITY_AVG_CROSS_CHANNEL, Combi::INTENSITY_AVG_CROSS_CHANNEL_AVG,
                   "Cross ch. intensity avg");
  createCheckBoxes(Base::INTENSITY_MIN_CROSS_CHANNEL, Combi::INTENSITY_MIN_CROSS_CHANNEL_AVG,
                   "Cross ch. intensity min");
  createCheckBoxes(Base::INTENSITY_MAX_CROSS_CHANNEL, Combi::INTENSITY_MAX_CROSS_CHANNEL_AVG,
                   "Cross ch. intensity max");
  createCheckBoxes(Base::COUNT_CROSS_CHANNEL, Combi::COUNT_CROSS_CHANNEL_AVG, "Cross ch. count avg");
  createCheckBoxes(std::nullopt, Combi::COUNT_CROSS_CHANNEL_SUM, "Cross ch. count sum");

  mainLayout->addWidget(groupBox);
}

int DialogChannelMeasurement::exec()
{
  int ret = QDialog::exec();

  std::set<Base> details;
  std::set<Combi> overview;
  std::set<Combi> heatmap;

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
