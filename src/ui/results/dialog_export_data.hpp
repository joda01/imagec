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

#pragma once

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qwindow.h>
#include <memory>
#include <thread>
#include <tuple>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/database/exporter/exporter.hpp"
#include "ui/container/setting/setting_combobox_multi.hpp"
#include "ui/dialog_shadow/dialog_shadow.h"
#include "ui/helper/layout_generator.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::db {
class QueryFilter;
}

namespace joda::ui {

class SettingComboBoxMultiClassificationUnmanaged;
class SettingComboBoxClassificationUnmanaged;

///
/// \class
/// \author
/// \brief
///
class ExportColumn : public QWidget
{
  friend class DialogExportData;

public:
  ExportColumn(std::unique_ptr<joda::db::Database> &analyzer, const std::map<settings::ClassificatorSettingOut, QString> &clustersAndClasses,
               QWidget *windowMain);

  void getImageChannels();
  void getCrossChannelCount();
  bool isEnabled();

  std::pair<settings::ClassificatorSettingOut, std::pair<std::string, std::string>> getClusterClassesToExport();
  std::map<settings::ClassificatorSettingOut, std::pair<std::string, std::string>> getCrossChannelCountToExport();
  std::map<int32_t, std::string> getCrossChannelIntensityToExport();
  std::map<enums::Measurement, std::set<enums::Stats>> getMeasurementAndStatsToExport();

private:
  /////////////////////////////////////////////////////
  void setEnabledDisabled(bool);

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBoxClassificationUnmanaged> mClustersAndClasses;
  std::unique_ptr<SettingComboBoxMulti<int32_t>> mCrossChannelImageChannel;
  std::unique_ptr<SettingComboBoxMultiClassificationUnmanaged> mCrossChannelCount;
  std::unique_ptr<SettingComboBoxMulti<enums::Measurement>> mMeasurement;
  std::unique_ptr<SettingComboBoxMulti<enums::Stats>> mStats;

  std::unique_ptr<joda::db::Database> &mAnalyzer;
  const std::map<settings::ClassificatorSettingOut, QString> &mClustersAndClassesVector;
};

class DialogExportData : public QDialog
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogExportData(std::unique_ptr<joda::db::Database> &analyzer, const db::QueryFilter &filter,
                   const std::map<settings::ClassificatorSettingOut, QString> &clustersAndClasses, db::AnalyzeMeta *analyzeMeta, QWidget *windowMain);

signals:
  void exportFinished();

private:
  /////////////////////////////////////////////////////
  void onExportClicked();
  void onCancelClicked();
  void selectAvgOfAllMeasureChannels();
  void unselectAllMeasureChannels();
  void selectAllExports();
  void saveTemplate();
  void openTemplate();

  /////////////////////////////////////////////////////
  QProgressBar *progressBar;
  QAction *mActionProgressBar = nullptr;
  QAction *mExportButton;
  QAction *mSelectAllMeasurements;
  QAction *mUnselectAllMeasurements;
  QAction *mSelectAllClustersAndClasses;

  QAction *mSaveSettings;
  QAction *mOpenSettings;

  /////////////////////////////////////////////////////
  QWidget *mWindowMain;
  std::unique_ptr<joda::db::Database> &mAnalyzer;
  db::AnalyzeMeta *mAnalyzeMeta = nullptr;
  const db::QueryFilter &mFilter;
  std::vector<ExportColumn *> mExportColumns;

  std::unique_ptr<SettingComboBox<joda::db::BatchExporter::Settings::ExportDetail>> mReportingDetails;
  std::unique_ptr<SettingComboBox<joda::db::BatchExporter::Settings::ExportType>> mReportingType;
  helper::LayoutGenerator mLayout;
  std::map<settings::ClassificatorSettingOut, QString> mClustersAndClasses;

private slots:
  void onExportFinished();
};

}    // namespace joda::ui
