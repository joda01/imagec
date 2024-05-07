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

#include <qtmetamacros.h>
#include <QtWidgets>
#include <filesystem>
#include <memory>
#include <mutex>
#include "backend/helper/directory_iterator.hpp"
#include "backend/results/results_reporting_settings.hpp"
#include "ui/container/container_button.hpp"
#include "ui/container/container_function.hpp"
#include "ui/helper/waitingspinnerwidget.hpp"
#include "ui/panel_preview.hpp"
#include "ui/reporting/exporter_thread.hpp"

namespace joda::ui::qt {

class WindowMain;

class PanelReporting : public QWidget
{
  Q_OBJECT

signals:
  void updatePreviewStarted();
  void updatePreviewFinished();

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
  void setActualSelectedWorkingFile(const QString &imageCFile);

private:
  /////////////////////////////////////////////////////
  QProgressBar *mProgressExportExcel;
  ContainerButton *mButtonReportingSettings;
  ContainerButton *mButtonExportExcel;
  std::shared_ptr<ReportingExporterThread> mExcelExporter;
  results::ReportingSettings mExcelReportSettings;

  // Heatmap
  std::shared_ptr<ContainerFunction<QString, int>> mHeatmapSlice;
  std::shared_ptr<ContainerFunction<bool, bool>> mGenerateHeatmapForWells;
  results::ReportingSettings mHeatmapReportSettings;

  QProgressBar *mProgressHeatmap;
  ContainerButton *mButtonReportingSettingsHeatmap;
  ContainerButton *mButtonExportHeatmap;
  std::shared_ptr<ReportingExporterThread> mHeatmapExporter;

  /////////////////////////////////////////////////////
  QHBoxLayout *createLayout();
  QProgressBar *createProgressBar(QWidget *parent);
  std::tuple<QVBoxLayout *, QWidget *> addVerticalPanel(QLayout *horizontalLayout, const QString &bgColor,
                                                        int margin = 16, bool enableScrolling = false,
                                                        int maxWidth = 250, int spacing = 4) const;

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  QLabel *createTitle(const QString &);
  bool mIsActiveShown = false;
  std::filesystem::path mSelectedImageCFile;

private slots:
  void onExcelExportChannelsClicked();
  void onHeatmapExportChannelsClicked();

  void onExportToXlsxClicked();

  void onExportToXlsxHeatmapClicked();
};

}    // namespace joda::ui::qt
