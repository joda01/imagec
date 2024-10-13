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
#include "backend/commands/classification/classifier_filter.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/database/exporter/r/exporter_r.hpp"
#include "backend/helper/database/exporter/xlsx/exporter.hpp"
#include "backend/helper/database/plugins/control_image.hpp"
#include "backend/helper/database/plugins/stats_for_image.hpp"
#include "backend/helper/database/plugins/stats_for_plate.hpp"
#include "backend/helper/database/plugins/stats_for_well.hpp"
#include "backend/settings/analze_settings.hpp"
#include "ui/container/setting/setting_base.hpp"
#include "ui/container/setting/setting_combobox_classification_unmanaged.hpp"
#include "ui/container/setting/setting_combobox_multi_classification_unmanaged.hpp"
#include "ui/helper/icon_generator.hpp"
#include "ui/helper/setting_generator.hpp"
#include "ui/helper/template_parser/template_parser.hpp"
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json_fwd.hpp>
#include "dialog_export_settings.hpp"
#include "panel_results.hpp"

namespace joda::ui {

struct Temp
{
  std::vector<std::vector<int32_t>> order;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Temp, order);
};

using Base = enums::Measurement;
using Stat = enums::Stats;

/////////////////////////////////////////////////////
ExportColumn::ExportColumn(std::unique_ptr<joda::db::Database> &analyzer,
                           const std::map<settings::ClassificatorSettingOut, QString> &clustersAndClasses, QWidget *windowMain) :
    QWidget(windowMain),
    mAnalyzer(analyzer), mClustersAndClassesVector(clustersAndClasses)
{
  auto *layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);

  ///
  ///
  ///
  mClustersAndClasses = SettingBase::create<SettingComboBoxClassificationUnmanaged>(windowMain, generateIcon("circle"), "Cluster/Class");
  mClustersAndClasses->addOptions(clustersAndClasses);
  layout->addWidget(mClustersAndClasses->getEditableWidget());

  //
  // Measurement
  //
  mMeasurement = SettingBase::create<SettingComboBoxMulti<enums::Measurement>>(windowMain, generateIcon("length"), "Measurement");
  mMeasurement->addOptions({{joda::enums::Measurement::COUNT, "Count", {}},
                            {joda::enums::Measurement::CONFIDENCE, "Confidence", {}},
                            {joda::enums::Measurement::AREA_SIZE, "Area size", {}},
                            {joda::enums::Measurement::PERIMETER, "Perimeter", {}},
                            {joda::enums::Measurement::CIRCULARITY, "Circularity", {}},
                            {joda::enums::Measurement::ORIGIN_OBJECT_ID, "Origin object ID", {}},
                            {joda::enums::Measurement::INTENSITY_SUM, "Intensity sum.", {}},
                            {joda::enums::Measurement::INTENSITY_AVG, "Intensity avg.", {}},
                            {joda::enums::Measurement::INTENSITY_MIN, "Intensity min.", {}},
                            {joda::enums::Measurement::INTENSITY_MAX, "Intensity max.", {}}

  });
  layout->addWidget(mMeasurement->getEditableWidget());

  //
  // Stats
  //
  mStats = SettingBase::create<SettingComboBoxMulti<enums::Stats>>(windowMain, generateIcon("sigma"), "Stats");
  mStats->addOptions({{Stat::AVG, "Avg", {}},
                      {Stat::CNT, "Cnt", {}},
                      {Stat::SUM, "Sum", {}},
                      {Stat::MIN, "Min", {}},
                      {Stat::MAX, "Max", {}},
                      {Stat::MEDIAN, "Median", {}},
                      {Stat::STDDEV, "Stddev", {}}});
  layout->addWidget(mStats->getEditableWidget());

  //
  // Cross channel
  //
  layout->addWidget(new QLabel("-->"), 0, Qt::AlignTop);
  mCrossChannelImageChannel = SettingBase::create<SettingComboBoxMulti<int32_t>>(windowMain, generateIcon("light"), "Intensity");
  layout->addWidget(mCrossChannelImageChannel->getEditableWidget());
  this->setLayout(layout);
  this->setContentsMargins(0, 0, 0, 0);

  //
  // connect signals
  //
  mClustersAndClasses->setValue({enums::ClusterId::UNDEFINED, enums::ClassId::UNDEFINED});
  connect(mClustersAndClasses.get(), &SettingBase::valueChanged, [this]() {
    getImageChannels();
    setEnabledDisabled(isEnabled());
  });
  getImageChannels();
  setEnabledDisabled(false);
}

bool ExportColumn::isEnabled()
{
  return mClustersAndClasses->getValue().clusterId != enums::ClusterId::UNDEFINED;
}

void ExportColumn::setEnabledDisabled(bool enabled)
{
  mCrossChannelImageChannel->getEditableWidget()->setEnabled(enabled);
  mMeasurement->getEditableWidget()->setEnabled(enabled);
  mStats->getEditableWidget()->setEnabled(enabled);
}

void ExportColumn::getImageChannels()
{
  auto imageChannels  = mAnalyzer->selectImageChannels();
  auto currentChannel = mCrossChannelImageChannel->getValue();
  auto channels = mAnalyzer->selectMeasurementChannelsForClusterAndClass(static_cast<enums::ClusterId>(mClustersAndClasses->getValue().clusterId),
                                                                         mClustersAndClasses->getValue().classId);
  mCrossChannelImageChannel->blockSignals(true);
  std::vector<SettingComboBoxMulti<int32_t>::ComboEntry> entry;

  for(const auto channelId : channels) {
    entry.emplace_back(SettingComboBoxMulti<int32_t>::ComboEntry{
        .key   = channelId,
        .label = "CH" + QString::number(channelId) + " (" + QString(imageChannels.at(channelId).name.data()) + ")",
    });
  }
  mCrossChannelImageChannel->addOptions(entry);
  mCrossChannelImageChannel->setValue(currentChannel);
  mCrossChannelImageChannel->blockSignals(false);
  if(channels.empty()) {
    mCrossChannelImageChannel->setEnabled(false);
  } else {
    mCrossChannelImageChannel->setEnabled(true);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::pair<settings::ClassificatorSettingOut, std::pair<std::string, std::string>> ExportColumn::getClusterClassesToExport()
{
  return mClustersAndClasses->getValueAndNames();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::map<int32_t, std::string> ExportColumn::getCrossChannelIntensityToExport()
{
  return mCrossChannelImageChannel->getValueAndNames();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::map<enums::Measurement, std::set<enums::Stats>> ExportColumn::getMeasurementAndStatsToExport()
{
  std::map<enums::Measurement, std::set<enums::Stats>> ret;
  for(const auto measure : mMeasurement->getValue()) {
    for(const auto stat : mStats->getValue()) {
      ret[measure].emplace(stat);
    }
  }
  return ret;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogExportData::DialogExportData(std::unique_ptr<joda::db::Database> &analyzer, const db::QueryFilter &filter,
                                   const std::map<settings::ClassificatorSettingOut, QString> &clustersAndClasses, db::AnalyzeMeta *analyzeMeta,
                                   QWidget *windowMain) :
    QDialog(windowMain),
    mWindowMain(windowMain), mAnalyzer(analyzer), mAnalyzeMeta(analyzeMeta), mFilter(filter), mLayout(this, false, true, false, true)
{
  setWindowTitle("Export data");
  setMinimumHeight(700);
  setMinimumWidth(1100);

  mExportButtonXLSX = mLayout.addActionButton("Excel export", generateIcon("export-excel"));
  connect(mExportButtonXLSX, &QAction::triggered, [this] { onExportClicked(ExportFormat::XLSX); });

  mExportButtonR = mLayout.addActionButton("R export", generateIcon("export-excel"));
  connect(mExportButtonR, &QAction::triggered, [this] { onExportClicked(ExportFormat::R); });

  mLayout.addSeparatorToTopToolbar();

  mOpenSettings = mLayout.addActionButton("Open template", generateIcon("opened-folder"));
  connect(mOpenSettings, &QAction::triggered, [this] { openTemplate(); });

  mSaveSettings = mLayout.addActionButton("Save template", generateIcon("save"));
  connect(mSaveSettings, &QAction::triggered, [this] { saveTemplate(); });

  /* mSelectAllMeasurements = mLayout.addActionButton("Select all measurements", "icons8-select-column);
   connect(mSelectAllMeasurements, &QAction::triggered, [this] { selectAvgOfAllMeasureChannels(); });

   mUnselectAllMeasurements = mLayout.addActionButton("Unselect all measurements", "icons8-select-none);
   connect(mUnselectAllMeasurements, &QAction::triggered, [this] { unselectAllMeasureChannels(); });

   mSelectAllClustersAndClasses = mLayout.addActionButton("Select all clusters", "icons8-select-50-all");
   connect(mSelectAllClustersAndClasses, &QAction::triggered, [this] { selectAllExports(); });*/

  auto *tab  = mLayout.addTab("Columns", [] {});
  auto *col1 = tab->addVerticalPanel();

  mClustersAndClasses = clustersAndClasses;
  mClustersAndClasses.emplace(settings::ClassificatorSettingOut{.clusterId = enums::ClusterId::UNDEFINED, .classId = enums::ClassId::UNDEFINED},
                              "Off");

  mExportColumns.push_back(new ExportColumn(mAnalyzer, mClustersAndClasses, windowMain));
  mExportColumns.push_back(new ExportColumn(mAnalyzer, mClustersAndClasses, windowMain));
  mExportColumns.push_back(new ExportColumn(mAnalyzer, mClustersAndClasses, windowMain));
  mExportColumns.push_back(new ExportColumn(mAnalyzer, mClustersAndClasses, windowMain));
  mExportColumns.push_back(new ExportColumn(mAnalyzer, mClustersAndClasses, windowMain));
  mExportColumns.push_back(new ExportColumn(mAnalyzer, mClustersAndClasses, windowMain));
  mExportColumns.push_back(new ExportColumn(mAnalyzer, mClustersAndClasses, windowMain));
  mExportColumns.push_back(new ExportColumn(mAnalyzer, mClustersAndClasses, windowMain));
  mExportColumns.push_back(new ExportColumn(mAnalyzer, mClustersAndClasses, windowMain));
  mExportColumns.push_back(new ExportColumn(mAnalyzer, mClustersAndClasses, windowMain));

  col1->addWidgetGroup("Columns", {mExportColumns.begin(), mExportColumns.end()}, 0, 16777214);

  //
  // Details
  mReportingDetails = SettingBase::create<SettingComboBox<joda::db::ExportSettings::ExportDetail>>(windowMain, {}, "Details");
  mReportingDetails->addOptions({{joda::db::ExportSettings::ExportDetail::PLATE, "Overview of all images", {}},
                                 {joda::db::ExportSettings::ExportDetail::WELL, "Selected well", {}},
                                 {joda::db::ExportSettings::ExportDetail::IMAGE, "Selected image", {}}});
  mReportingDetails->setDefaultValue(joda::db::ExportSettings::ExportDetail::PLATE);

  //
  // Type
  mReportingType = SettingBase::create<SettingComboBox<joda::db::ExportSettings::ExportType>>(windowMain, {}, "Type");
  mReportingType->addOptions({{joda::db::ExportSettings::ExportType::HEATMAP, "Heatmap", generateIcon("heat-map")},
                              {joda::db::ExportSettings::ExportType::TABLE, "Table", generateIcon("table")},
                              {joda::db::ExportSettings::ExportType::TABLE_DETAIL, "Details", generateIcon("table-detail")}});
  mReportingType->setDefaultValue(joda::db::ExportSettings::ExportType::HEATMAP);

  std::vector<SettingComboBoxMulti<enums::ClusterId>::ComboEntry> clustersCombo;
  auto clusters = mAnalyzer->selectClusters();
  clustersCombo.reserve(clusters.size());
  for(const auto &[clusterId, cluster] : clusters) {
    clustersCombo.push_back(SettingComboBoxMulti<enums::ClusterId>::ComboEntry{.key = clusterId, .label = cluster.name.data(), .icon = {}});
  }

  std::vector<SettingComboBoxMulti<enums::ClassId>::ComboEntry> classCombo;
  auto classes = mAnalyzer->selectClasses();
  classCombo.reserve(classes.size());
  for(const auto &[classId, cluster] : classes) {
    classCombo.push_back(SettingComboBoxMulti<enums::ClassId>::ComboEntry{.key = classId, .label = cluster.name.data(), .icon = {}});
  }

  auto *col2 = tab->addVerticalPanel();
  col2->addGroup("Exports", {mReportingType.get(), mReportingDetails.get()});

  // Progress bar
  progressBar = new QProgressBar(this);
  progressBar->setRange(0, 0);
  progressBar->setMaximum(0);
  progressBar->setMinimum(0);
  mActionProgressBar = mLayout.addItemToBottomToolbar(progressBar);
  mActionProgressBar->setVisible(false);
  connect(this, &DialogExportData::exportFinished, this, &DialogExportData::onExportFinished);
}

///
/// \brief      Export data
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogExportData::onExportClicked(ExportFormat format)
{
  QString filePathOfSettingsFile = QFileDialog::getSaveFileName(this, "Save File", "", "Spreadsheet (*.xlsx)");

  if(filePathOfSettingsFile.isEmpty()) {
    return;
  }
  mActionProgressBar->setVisible(true);
  mExportButtonXLSX->setEnabled(false);
  mExportButtonR->setEnabled(false);

  std::thread([this, filePathOfSettingsFile, format] {
    std::map<settings::ClassificatorSettingOut, joda::db::ExportSettings::Channel> clustersToExport;

    for(const auto &columnToExport : mExportColumns) {
      if(!columnToExport->isEnabled()) {
        continue;
      }
      auto [clusterClassID, name] = columnToExport->getClusterClassesToExport();

      joda::db::ExportSettings::Channel channel;
      channel.clusterName         = std::get<0>(name);
      channel.className           = std::get<1>(name);
      channel.measureChannels     = columnToExport->getMeasurementAndStatsToExport();
      channel.crossChannelStacksC = columnToExport->getCrossChannelIntensityToExport();
      clustersToExport.emplace(clusterClassID, channel);
    }

    joda::db::ExportSettings settings{
        .clustersToExport = clustersToExport,
        .analyzer         = *mAnalyzer,
        .plateId          = mFilter.plateId,
        .groupId          = mFilter.actGroupId,
        .imageId          = mFilter.actImageId,
        .plateRows        = mFilter.plateRows,
        .plateCols        = mFilter.plateCols,
        .heatmapAreaSize  = mFilter.densityMapAreaSize,
        .wellImageOrder   = mFilter.wellImageOrder,
        .exportType       = mReportingType->getValue(),
        .exportDetail     = mReportingDetails->getValue(),
    };

    settings::AnalyzeSettings analyzeSettings;
    try {
      analyzeSettings = nlohmann::json::parse(mAnalyzeMeta->analyzeSettingsJsonString);
    } catch(const std::exception &ex) {
      std::cout << "Ups " << ex.what() << std::endl;
    }

    switch(format) {
      case ExportFormat::XLSX:
        joda::db::BatchExporter::startExport(settings, analyzeSettings, mAnalyzeMeta->jobName, mAnalyzeMeta->timestampStart,
                                             mAnalyzeMeta->timestampFinish, filePathOfSettingsFile.toStdString());
        break;
      case ExportFormat::R:
        joda::db::RExporter::startExport(settings, analyzeSettings, mAnalyzeMeta->jobName, mAnalyzeMeta->timestampStart,
                                         mAnalyzeMeta->timestampFinish, filePathOfSettingsFile.toStdString());
        break;
    }

    emit exportFinished();
  }).detach();
}

void DialogExportData::onCancelClicked()
{
}

void DialogExportData::onExportFinished()
{
  if(mActionProgressBar != nullptr) {
    mActionProgressBar->setVisible(false);
    mExportButtonXLSX->setEnabled(true);
    mExportButtonR->setEnabled(true);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogExportData::selectAvgOfAllMeasureChannels()
{
  // for(const auto &[measure, ch] : mChannelsToExport) {
  //   ch->setValue({enums::Stats::AVG});
  // }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogExportData::unselectAllMeasureChannels()
{
  // for(const auto &[measure, ch] : mChannelsToExport) {
  //   ch->setValue({});
  // }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogExportData::selectAllExports()
{
  // mClustersToExport->selectAll();
  // mClassesToExport->selectAll();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogExportData::saveTemplate()
{
  QString folderToOpen           = joda::templates::TemplateParser::getUsersTemplateDirectory().string().data();
  QString filePathOfSettingsFile = QFileDialog::getSaveFileName(
      this, "Save template", folderToOpen, "ImageC export template files (*" + QString(joda::fs::EXT_EXPORT_TEMPLATE.data()) + ")");
  if(filePathOfSettingsFile.isEmpty()) {
    return;
  }

  SettingsExportData settings;

  // To settings
  {
    for(const auto &col : mExportColumns) {
      SettingsExportData::Column colSetting;
      colSetting.inputCluster          = col->mClustersAndClasses->getValue();
      colSetting.measurements          = col->mMeasurement->getValue();
      colSetting.stats                 = col->mStats->getValue();
      colSetting.crossChannelIntensity = col->mCrossChannelImageChannel->getValue();
      settings.columns.emplace_back(colSetting);
    }
  }

  try {
    nlohmann::json templateJson = settings;
    joda::templates::TemplateParser::saveTemplate(templateJson, std::filesystem::path(filePathOfSettingsFile.toStdString()),
                                                  joda::fs::EXT_EXPORT_TEMPLATE);
  } catch(const std::exception &ex) {
    QMessageBox messageBox(mWindowMain);
    messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
    messageBox.setWindowTitle("Could not save template!");
    messageBox.setText("Could not save template, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogExportData::openTemplate()
{
  using namespace std::chrono_literals;

  QString folderToOpen = joda::templates::TemplateParser::getUsersTemplateDirectory().string().data();
  QString filePath     = QFileDialog::getOpenFileName(this, "Open File", folderToOpen,
                                                      "ImageC export template files (*" + QString(joda::fs::EXT_EXPORT_TEMPLATE.data()) + ")", nullptr);

  if(filePath.isEmpty()) {
    return;
  }
  SettingsExportData settings;

  try {
    std::ifstream ifs(filePath.toStdString());
    settings = nlohmann::json::parse(ifs);
    ifs.close();

    for(auto &col : mExportColumns) {
      col->mClustersAndClasses->setValue({enums::ClusterId::UNDEFINED, enums::ClassId::UNDEFINED});
    }

    int colCnt = 0;
    for(const auto &col : settings.columns) {
      if(colCnt < mExportColumns.size()) {
        mExportColumns[colCnt]->mClustersAndClasses->setValue(col.inputCluster);
        mExportColumns[colCnt]->mMeasurement->setValue(col.measurements);
        mExportColumns[colCnt]->mStats->setValue(col.stats);
        std::this_thread::sleep_for(100ms);
        mExportColumns[colCnt]->mCrossChannelImageChannel->setValue(col.crossChannelIntensity);
      }
      colCnt++;
    }

  } catch(const std::exception &ex) {
    QMessageBox messageBox(mWindowMain);
    messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
    messageBox.setWindowTitle("Could not open template!");
    messageBox.setText("Could not open template, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

}    // namespace joda::ui
