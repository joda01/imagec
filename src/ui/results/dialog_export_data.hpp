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
#include <qtmetamacros.h>
#include <qwindow.h>
#include <memory>
#include <thread>
#include <tuple>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/database/exporter/exporter.hpp"
#include "ui/container/setting/setting_combobox _multi.hpp"
#include "ui/dialog_shadow/dialog_shadow.h"
#include "ui/helper/layout_generator.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::db {
class QueryFilter;
}

namespace joda::ui {

class DialogExportData : public QDialog
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogExportData(std::unique_ptr<joda::db::Database> &analyzer, const db::QueryFilter &filter, QWidget *windowMain);

signals:
  void exportFinished();

private:
  /////////////////////////////////////////////////////
  void onExportClicked();
  void onCancelClicked();
  void selectAvgOfAllMeasureChannels();
  void unselectAllMeasureChannels();
  void selectAllExports();

  /////////////////////////////////////////////////////
  QProgressBar *progressBar;
  QAction *mActionProgressBar = nullptr;
  QAction *mExportButton;

  QAction *mSelectAllMeasurements;
  QAction *mUnselectAllMeasurements;

  QAction *mSelectAllClustersAndClasses;

  /////////////////////////////////////////////////////
  std::unique_ptr<joda::db::Database> &mAnalyzer;
  const db::QueryFilter &mFilter;
  std::unique_ptr<SettingComboBoxMulti<enums::ClusterId>> mClustersToExport;
  std::unique_ptr<SettingComboBoxMulti<enums::ClassId>> mClassesToExport;

  // Cross channel
  std::unique_ptr<SettingComboBoxMulti<int32_t>> mCrossChannelStackC;
  std::unique_ptr<SettingComboBoxMulti<enums::ClusterId>> mCrossChannelClusterId;
  std::unique_ptr<SettingComboBoxMulti<enums::ClassId>> mCrossChannelClassId;

  std::map<enums::Measurement, std::unique_ptr<SettingComboBoxMulti<enums::Stats>>> mChannelsToExport;
  std::unique_ptr<SettingComboBox<joda::db::BatchExporter::Settings::ExportDetail>> mReportingDetails;
  std::unique_ptr<SettingComboBox<joda::db::BatchExporter::Settings::ExportType>> mReportingType;
  helper::LayoutGenerator mLayout;

private slots:
  void onExportFinished();
};

}    // namespace joda::ui
