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
#include <qdialog.h>
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
#include "backend/helper/database/exporter/exporter.hpp"
#include "backend/helper/database/plugins/control_image.hpp"
#include "backend/helper/database/plugins/stats_for_image.hpp"
#include "backend/helper/database/plugins/stats_for_plate.hpp"
#include "backend/helper/database/plugins/stats_for_well.hpp"
#include "ui/container/setting/setting_base.hpp"
#include "ui/helper/setting_generator.hpp"
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
DialogExportData::DialogExportData(std::unique_ptr<joda::db::Database> &analyzer, const db::QueryFilter &filter,
                                   QWidget *windowMain) :
    QDialog(windowMain),
    mAnalyzer(analyzer), mFilter(filter), mLayout(this, false, true, false)
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

  createCheckBoxes(Base::COUNT,
                   {{Stat::AVG, "Avg", ""},
                    {Stat::CNT, "Cnt", ""},
                    {Stat::SUM, "Sum", ""},
                    {Stat::MIN, "Min", ""},
                    {Stat::MAX, "Max", ""},
                    {Stat::MEDIAN, "Median", ""},
                    {Stat::STDDEV, "Stddev", ""}},
                   "", "Count");
  createCheckBoxes(Base::CONFIDENCE,
                   {{Stat::AVG, "Avg", ""},
                    {Stat::CNT, "Cnt", ""},
                    {Stat::SUM, "Sum", ""},
                    {Stat::MIN, "Min", ""},
                    {Stat::MAX, "Max", ""},
                    {Stat::MEDIAN, "Median", ""},
                    {Stat::STDDEV, "Stddev", ""}},
                   "", "Confidence");
  createCheckBoxes(Base::AREA_SIZE,
                   {{Stat::AVG, "Avg", ""},
                    {Stat::CNT, "Cnt", ""},
                    {Stat::SUM, "Sum", ""},
                    {Stat::MIN, "Min", ""},
                    {Stat::MAX, "Max", ""},
                    {Stat::MEDIAN, "Median", ""},
                    {Stat::STDDEV, "Stddev", ""}},
                   "", "Area size");
  createCheckBoxes(Base::PERIMETER,
                   {{Stat::AVG, "Avg", ""},
                    {Stat::CNT, "Cnt", ""},
                    {Stat::SUM, "Sum", ""},
                    {Stat::MIN, "Min", ""},
                    {Stat::MAX, "Max", ""},
                    {Stat::MEDIAN, "Median", ""},
                    {Stat::STDDEV, "Stddev", ""}},
                   "", "Perimeter");
  createCheckBoxes(Base::CIRCULARITY,
                   {{Stat::AVG, "Avg", ""},
                    {Stat::CNT, "Cnt", ""},
                    {Stat::SUM, "Sum", ""},
                    {Stat::MIN, "Min", ""},
                    {Stat::MAX, "Max", ""},
                    {Stat::MEDIAN, "Median", ""},
                    {Stat::STDDEV, "Stddev", ""}},
                   "icons8-polygon-50-2.png", "Circularity");
  createCheckBoxes(Base::INTERSECTING_CNT,
                   {{Stat::AVG, "Avg", ""},
                    {Stat::CNT, "Cnt", ""},
                    {Stat::SUM, "Sum", ""},
                    {Stat::MIN, "Min", ""},
                    {Stat::MAX, "Max", ""},
                    {Stat::MEDIAN, "Median", ""},
                    {Stat::STDDEV, "Stddev", ""}},
                   "", "Intersecting count");
  createCheckBoxes(Base::CENTER_OF_MASS_X,
                   {{Stat::AVG, "Avg", ""},
                    {Stat::CNT, "Cnt", ""},
                    {Stat::SUM, "Sum", ""},
                    {Stat::MIN, "Min", ""},
                    {Stat::MAX, "Max", ""},
                    {Stat::MEDIAN, "Median", ""},
                    {Stat::STDDEV, "Stddev", ""}},
                   "", "Center of mass X");
  createCheckBoxes(Base::CENTER_OF_MASS_Y,
                   {{Stat::AVG, "Avg", ""},
                    {Stat::CNT, "Cnt", ""},
                    {Stat::SUM, "Sum", ""},
                    {Stat::MIN, "Min", ""},
                    {Stat::MAX, "Max", ""},
                    {Stat::MEDIAN, "Median", ""},
                    {Stat::STDDEV, "Stddev", ""}},
                   "", "Center of mass Y");
  createCheckBoxes(Base::INTENSITY_AVG,
                   {{Stat::AVG, "Avg", ""},
                    {Stat::CNT, "Cnt", ""},
                    {Stat::SUM, "Sum", ""},
                    {Stat::MIN, "Min", ""},
                    {Stat::MAX, "Max", ""},
                    {Stat::MEDIAN, "Median", ""},
                    {Stat::STDDEV, "Stddev", ""}},
                   "icons8-light-50.png", "Intensity AVG");
  createCheckBoxes(Base::INTENSITY_MIN,
                   {{Stat::AVG, "Avg", ""},
                    {Stat::CNT, "Cnt", ""},
                    {Stat::SUM, "Sum", ""},
                    {Stat::MIN, "Min", ""},
                    {Stat::MAX, "Max", ""},
                    {Stat::MEDIAN, "Median", ""},
                    {Stat::STDDEV, "Stddev", ""}},
                   "icons8-light-50.png", "Intensity MIN");
  createCheckBoxes(Base::INTENSITY_MAX,
                   {{Stat::AVG, "Avg", ""},
                    {Stat::CNT, "Cnt", ""},
                    {Stat::SUM, "Sum", ""},
                    {Stat::MIN, "Min", ""},
                    {Stat::MAX, "Max", ""},
                    {Stat::MEDIAN, "Median", ""},
                    {Stat::STDDEV, "Stddev", ""}},
                   "icons8-light-50.png", "Intensity MAX");

  col1->addGroup("Measurements", settings);

  //
  // Details
  mReportingDetails =
      SettingBase::create<SettingComboBox<joda::db::BatchExporter::Settings::ExportDetail>>(windowMain, "", "Details");
  mReportingDetails->addOptions({{joda::db::BatchExporter::Settings::ExportDetail::PLATE, "Overview of all images", ""},
                                 {joda::db::BatchExporter::Settings::ExportDetail::WELL, "Selected well", ""},
                                 {joda::db::BatchExporter::Settings::ExportDetail::IMAGE, "Selected image", ""}});
  mReportingDetails->setDefaultValue(joda::db::BatchExporter::Settings::ExportDetail::PLATE);

  //
  // Type
  mReportingType =
      SettingBase::create<SettingComboBox<joda::db::BatchExporter::Settings::ExportType>>(windowMain, "", "Type");
  mReportingType->addOptions(
      {{joda::db::BatchExporter::Settings::ExportType::HEATMAP, "Heatmap", "icons8-heat-map-50.png"},
       {joda::db::BatchExporter::Settings::ExportType::LIST, "Table", "icons8-table-50.png"}});
  mReportingType->setDefaultValue(joda::db::BatchExporter::Settings::ExportType::HEATMAP);

  //
  // Clusters
  {
    mClustersToExport = SettingBase::create<SettingComboBoxMulti<enums::ClusterId>>(windowMain, "", "Clusters");
    std::vector<SettingComboBoxMulti<enums::ClusterId>::ComboEntry> clustersCombo;
    auto clusters = mAnalyzer->selectClusters();
    clustersCombo.reserve(clusters.size());
    for(const auto &[clusterId, cluster] : clusters) {
      clustersCombo.push_back(SettingComboBoxMulti<enums::ClusterId>::ComboEntry{
          .key = clusterId, .label = cluster.name.data(), .icon = ""});
    }
    mClustersToExport->addOptions(clustersCombo);
  }

  //
  // Classes
  {
    mClassesToExport = SettingBase::create<SettingComboBoxMulti<enums::ClassId>>(windowMain, "", "Classes");
    std::vector<SettingComboBoxMulti<enums::ClassId>::ComboEntry> classCombo;
    auto classes = mAnalyzer->selectClasses();
    classCombo.reserve(classes.size());
    for(const auto &[classId, cluster] : classes) {
      classCombo.push_back(
          SettingComboBoxMulti<enums::ClassId>::ComboEntry{.key = classId, .label = cluster.name.data(), .icon = ""});
    }
    mClassesToExport->addOptions(classCombo);
  }

  //
  // Image channels
  mImageChannels = joda::ui::generateCStackCombo<SettingComboBoxMulti<int32_t>>("Image channels", windowMain);

  auto *col2 = tab->addVerticalPanel();
  col2->addGroup("Exports", {mReportingType.get(), mReportingDetails.get(), mClustersToExport.get(),
                             mClassesToExport.get(), mImageChannels.get()});
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
  QString filePathOfSettingsFile = QFileDialog::getSaveFileName(this, "Save File", "", "Spreadsheet (*.xlsx)");

  if(filePathOfSettingsFile.isEmpty()) {
    return;
  }
  std::map<enums::ClusterId, joda::db::BatchExporter::BatchExporter::Settings::Channel> clustersToExport;
  std::map<enums::Measurement, std::set<enums::Stats>> measureChannels;

  for(const auto &[ch, stat] : mChannelsToExport) {
    measureChannels.emplace(ch, stat->getValue());
  }

  for(const auto &clusterId : mClustersToExport->getValue()) {
    clustersToExport.emplace(clusterId, joda::db::BatchExporter::BatchExporter::Settings::Channel{
                                            .name            = mClustersToExport->getName(clusterId).toStdString(),
                                            .classes         = mClassesToExport->getValueAndNames(),
                                            .imageChannelId  = mImageChannels->getValue(),
                                            .measureChannels = measureChannels});
  }

  joda::db::BatchExporter::Settings settings{.clustersToExport = clustersToExport,
                                             .analyzer         = *mAnalyzer,
                                             .plateId          = mFilter.plateId,
                                             .groupId          = mFilter.actGroupId,
                                             .imageId          = mFilter.actImageId,
                                             .plateRows        = mFilter.plateRows,
                                             .plateCols        = mFilter.plateCols,
                                             .heatmapAreaSize  = mFilter.densityMapAreaSize,
                                             .wellImageOrder   = mFilter.wellImageOrder,
                                             .exportType       = mReportingType->getValue(),
                                             .exportDetail     = mReportingDetails->getValue()};
  joda::db::BatchExporter::startExport(settings, filePathOfSettingsFile.toStdString());
}

void DialogExportData::onCancelClicked()
{
}

}    // namespace joda::ui
