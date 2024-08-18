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

///

#include "dialog_export_data.hpp"
#include <qboxlayout.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qtableview.h>
#include <qtablewidget.h>
#include <qwidget.h>
#include <QMessageBox>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "backend/results/db_column_ids.hpp"
#include "backend/results/exporter/exporter.hpp"
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::qt {

struct Temp
{
  std::vector<std::vector<int32_t>> order;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Temp, order);
};

using Base = results::MeasureChannel;
using Stat = results::Stats;

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogExportData::DialogExportData(QWidget *windowMain) : DialogShadow(windowMain)
{
  setWindowTitle("Settings");
  setBaseSize(500, 400);
  setMinimumWidth(250);

  auto *mainLayout = new QVBoxLayout(this);
  auto *groupBox   = new QWidget(this);
  auto *gridLayout = new QGridLayout(groupBox);

  std::map<Base, Stat> measureChannelsOverview;

  int row               = 4;
  auto createCheckBoxes = [this, &measureChannelsOverview, &gridLayout, &groupBox,
                           &row](Base detail, Stat stat, const std::string &description) {
    gridLayout->addWidget(new QLabel(description.data()), row, 0);

    QCheckBox *onOffOverview = new QCheckBox(groupBox);
    gridLayout->addWidget(onOffOverview, row, 1, Qt::AlignCenter);
    mChannelsToExport.emplace(std::tuple<results::MeasureChannel, results::Stats>{detail, stat}, onOffOverview);

    row++;
  };

  createCheckBoxes(Base::CONFIDENCE, Stat::AVG, "Confidence");
  createCheckBoxes(Base::AREA_SIZE, Stat::AVG, "Area size");
  createCheckBoxes(Base::PERIMETER, Stat::AVG, "Perimeter");
  createCheckBoxes(Base::CIRCULARITY, Stat::AVG, "Circularity");
  createCheckBoxes(Base::VALID, Stat::SUM, "Valid");
  createCheckBoxes(Base::INVALID, Stat::SUM, "Invalid");
  createCheckBoxes(Base::CENTER_OF_MASS_X, Stat::AVG, "Center of mass X");
  createCheckBoxes(Base::CENTER_OF_MASS_Y, Stat::AVG, "Center of mass Y");
  createCheckBoxes(Base::INTENSITY_AVG, Stat::AVG, "Intensity AVG");
  createCheckBoxes(Base::INTENSITY_MIN, Stat::AVG, "Intensity MIN");
  createCheckBoxes(Base::INTENSITY_MAX, Stat::AVG, "Intensity MAX");
  createCheckBoxes(Base::CROSS_CHANNEL_INTENSITY_AVG, Stat::AVG, "Cross ch. intensity avg");
  createCheckBoxes(Base::CROSS_CHANNEL_INTENSITY_MIN, Stat::AVG, "Cross ch. intensity min");
  createCheckBoxes(Base::CROSS_CHANNEL_INTENSITY_MAX, Stat::AVG, "Cross ch. intensity max");
  createCheckBoxes(Base::CROSS_CHANNEL_COUNT, Stat::SUM, "Cross ch. count avg");

  mainLayout->addWidget(groupBox);

  //
  // Footer buttons
  //
  QWidget *buttons = new QWidget();
  //   buttons->setContentsMargins(0, 0, 0, 0);
  QHBoxLayout *hBox = new QHBoxLayout(buttons);

  // Close button
  QPushButton *close = new QPushButton("Close", buttons);
  close->setCursor(Qt::PointingHandCursor);
  close->setObjectName("DialogButton");
  connect(close, &QPushButton::clicked, this, &DialogExportData::onCancelClicked);

  // Export button
  QPushButton *exportHeatmap = new QPushButton("Export as heatmap", buttons);
  exportHeatmap->setCursor(Qt::PointingHandCursor);
  exportHeatmap->setObjectName("DialogButton");
  connect(exportHeatmap, &QPushButton::clicked, this, &DialogExportData::onExportHeatmapClicked);

  // Export button
  QPushButton *exportList = new QPushButton("Export as list", buttons);
  exportList->setCursor(Qt::PointingHandCursor);
  exportList->setObjectName("DialogButton");
  connect(exportList, &QPushButton::clicked, this, &DialogExportData::onExportListClicked);

  hBox->addStretch();
  hBox->addWidget(exportList);
  hBox->addWidget(exportHeatmap);
  hBox->addWidget(close);
  QLayout *mainL = layout();
  mainL->addWidget(buttons);
}

DialogExportData::Ret DialogExportData::execute()
{
  int ret = QDialog::exec();
  if(retVal != 0) {
    return {retVal, {}};
  }

  std::map<results::MeasureChannel, results::Stats> overview;
  for(auto const &[key, val] : mChannelsToExport) {
    if(val->isChecked()) {
      auto [measureChannel, stat] = key;
      overview.emplace(std::get<0>(key), static_cast<results::Stats>(std::get<1>(key)));
    }
  }

  return {.ret = 0, .channelsToExport = overview, .exportHeatmap = mExportHeatmap, .exportList = mExportList};
}

void DialogExportData::onExportListClicked()
{
  retVal      = 0;
  mExportList = true;
  close();
}

void DialogExportData::onExportHeatmapClicked()
{
  retVal         = 0;
  mExportHeatmap = true;
  close();
}

void DialogExportData::onCancelClicked()
{
  retVal = -1;
  close();
}

}    // namespace joda::ui::qt
