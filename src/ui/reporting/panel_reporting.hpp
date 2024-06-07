///
/// \file      panel_channel.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <qboxlayout.h>
#include <qcombobox.h>
#include <qtmetamacros.h>
#include <QtWidgets>
#include <filesystem>
#include <memory>
#include <mutex>
#include <thread>
#include "backend/helper/directory_iterator.hpp"
#include "backend/helper/xz/archive_reader.hpp"
#include "backend/results/analyzer/analyzer.hpp"
#include "backend/results/db_column_ids.hpp"
#include "backend/results/results.hpp"
#include "ui/container/container_button.hpp"
#include "ui/container/container_function.hpp"
#include "ui/helper/waitingspinnerwidget.hpp"
#include "ui/panel_preview.hpp"
#include "ui/reporting/plugins/panel_heatmap.hpp"

namespace joda::ui::qt {

class WindowMain;

class PanelReporting : public QWidget
{
  Q_OBJECT

signals:
  void loadingFilesfinished();

public:
  PanelReporting(WindowMain *wm);
  ~PanelReporting();

  void setActive(bool setActive)
  {
    if(!mIsActiveShown && setActive) {
      mIsActiveShown = true;
    }
    if(!setActive) {
      mIsActiveShown = false;
    }
  }
  void setActualSelectedWorkingFile(const std::filesystem::path &imageCFile);
  void close();

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<joda::results::Analyzer> mAnalyzer;

  // Selector
  QVBoxLayout *mSelectorLayout;
  std::shared_ptr<ContainerFunction<QString, int>> mAnalyzeSelector;
  std::shared_ptr<ContainerFunction<joda::results::ChannelIndex, int>> mChannelSelector;
  std::shared_ptr<ContainerFunction<uint32_t, int>> mMeasureChannelSelector;
  std::shared_ptr<ContainerFunction<joda::results::Stats, int>> mStats;
  reporting::plugin::PanelHeatmap::SelectedFilter mFilter;
  std::vector<results::db::ChannelMeta> mChannelInfos;

  // Heatmap
  std::shared_ptr<ContainerFunction<uint32_t, uint32_t>> mPlateSize;
  std::shared_ptr<ContainerFunction<QString, int>> mHeatmapSlice;
  std::shared_ptr<ContainerFunction<QString, int>> mWellOrdering;

  QProgressBar *mProgressHeatmap;
  ContainerButton *mButtonExport;

  // Table
  QTableWidget *mTable;
  reporting::plugin::PanelHeatmap *mHeatmap;

  /////////////////////////////////////////////////////
  QProgressBar *createProgressBar(QWidget *parent);

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  QLabel *createTitle(const QString &);
  bool mIsActiveShown = false;

  void lookingForFilesThread();
  void loadDetailReportToTable();

private slots:
  void onExportListClicked();
  void onExportHeatmapClicked();
  void onLoadingFileFinished();
  void onResultsFileSelected();
  void onChannelChanged();
  void onMeasurementChanged();
  void onTableDoubleClicked(const QModelIndex &index);
};

}    // namespace joda::ui::qt
