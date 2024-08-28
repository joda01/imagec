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
#include <qaction.h>
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
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/database/plugins/control_image.hpp"
#include "backend/helper/database/plugins/heatmap_for_image.hpp"
#include "backend/helper/database/plugins/heatmap_for_plate.hpp"
#include "backend/helper/database/plugins/heatmap_for_well.hpp"
#include "ui/container/setting/setting_base.hpp"
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json_fwd.hpp>
#include "panel_results.hpp"

namespace joda::ui {

struct Temp
{
  std::vector<std::vector<int32_t>> order;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Temp, order);
};

using Base = enums::Measurement;
using Stat = enums::Stats;

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogExportData::DialogExportData(std::unique_ptr<joda::db::Database> &analyzer, const SelectedFilter &filter,
                                   QWidget *windowMain) :
    mAnalyzer(analyzer),
    mFilter(filter), mLayout(this, false, true, false)
{
  setWindowTitle("Export data");
  setMinimumHeight(700);

  auto *exportToExcel = mLayout.addActionButton("Excel export", "icons8-export-excel-50.png");
  connect(exportToExcel, &QAction::triggered, [this] { onExportClicked(); });

  auto *tab  = mLayout.addTab("Measurement", [] {});
  auto *col1 = tab->addVerticalPanel();

  std::vector<SettingBase *> settings;
  auto createCheckBoxes = [this, &windowMain, &settings](
                              Base measurement, std::vector<SettingComboBoxMulti<enums::Stats>::ComboEntry> stats,
                              const std::string &icon, const std::string &description) {
    auto element = SettingBase::create<SettingComboBoxMulti<enums::Stats>>(windowMain, icon.data(), description.data());
    element->addOptions(stats);
    element->setDefaultValue(Stat::AVG);
    settings.emplace_back(element.get());
    mChannelsToExport.emplace(measurement, std::move(element));
  };

  createCheckBoxes(Base::COUNT, {{Stat::AVG, "Avg", ""}}, "", "Count");
  createCheckBoxes(Base::CONFIDENCE, {{Stat::AVG, "Avg", ""}}, "", "Confidence");
  createCheckBoxes(Base::AREA_SIZE, {{Stat::AVG, "Avg", ""}}, "", "Area size");
  createCheckBoxes(Base::PERIMETER, {{Stat::AVG, "Avg", ""}}, "", "Perimeter");
  createCheckBoxes(Base::CIRCULARITY, {{Stat::AVG, "Avg", ""}}, "icons8-polygon-50-2.png", "Circularity");
  createCheckBoxes(Base::CENTER_OF_MASS_X, {{Stat::AVG, "Avg", ""}}, "", "Center of mass X");
  createCheckBoxes(Base::CENTER_OF_MASS_Y, {{Stat::AVG, "Avg", ""}}, "", "Center of mass Y");
  createCheckBoxes(Base::INTENSITY_AVG, {{Stat::AVG, "Avg", ""}}, "icons8-light-50.png", "Intensity AVG");
  createCheckBoxes(Base::INTENSITY_MIN, {{Stat::AVG, "Avg", ""}}, "icons8-light-50.png", "Intensity MIN");
  createCheckBoxes(Base::INTENSITY_MAX, {{Stat::AVG, "Avg", ""}}, "icons8-light-50.png", "Intensity MAX");

  col1->addGroup("Measurements", settings);

  mReportingDetails =
      SettingBase::create<SettingComboBox<joda::db::BatchExporter::Settings::ExportDetail>>(windowMain, "", "Details");
  mReportingDetails->addOptions({{joda::db::BatchExporter::Settings::ExportDetail::PLATE, "Plate", ""},
                                 {joda::db::BatchExporter::Settings::ExportDetail::WELL, "Well", ""},
                                 {joda::db::BatchExporter::Settings::ExportDetail::IMAGE, "Image", ""}});
  mReportingDetails->setDefaultValue(joda::db::BatchExporter::Settings::ExportDetail::PLATE);

  mReportingType =
      SettingBase::create<SettingComboBox<joda::db::BatchExporter::Settings::ExportType>>(windowMain, "", "Type");
  mReportingType->addOptions(
      {{joda::db::BatchExporter::Settings::ExportType::LIST, "List", ""},
       {joda::db::BatchExporter::Settings::ExportType::HEATMAP, "Heatmap", "icons8-heat-map-50.png"}});
  mReportingType->setDefaultValue(joda::db::BatchExporter::Settings::ExportType::LIST);

  auto *col2 = tab->addVerticalPanel();
  col2->addGroup("Exports", {mReportingDetails.get(), mReportingType.get()});
}

///
/// \brief      Export data
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogExportData::onExportClicked()
{
}

///
/// \brief      Export data
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogExportData::exportHeatmap()
{
  auto table = joda::db::HeatmapPerPlate::getData(*mAnalyzer, mFilter.plateId, mFilter.plateRows, mFilter.plateCols,
                                                  mFilter.clusterId, mFilter.classId, mFilter.measurementChannel,
                                                  mFilter.imageChannel, mFilter.stats);
}

void DialogExportData::onCancelClicked()
{
}

}    // namespace joda::ui
