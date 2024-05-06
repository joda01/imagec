///
/// \file      panel_channel.cpp
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

#include "panel_reporting.hpp"
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <exception>
#include <filesystem>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include "../window_main.hpp"
#include "backend/pipelines/reporting/reporting_generator.hpp"
#include "backend/pipelines/reporting/reporting_heatmap.hpp"
#include "backend/results/results.hpp"
#include "ui/reporting/exporter_thread.hpp"
#include "dialog_channel_measurment.hpp"

namespace joda::ui::qt {

using namespace std::chrono_literals;
using namespace std::filesystem;

PanelReporting::PanelReporting(WindowMain *wm) : mWindowMain(wm), mDirWatcher({".json"})
{
  // setStyleSheet("border: 1px solid black; padding: 10px;");
  setObjectName("PanelReporting");

  auto *horizontalLayout             = createLayout();
  auto [verticalLayoutContainer, _1] = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 250, 16);

  //
  // Excel export
  //
  {
    auto [verticalLayoutXlsx, _2] = addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");
    verticalLayoutXlsx->addWidget(createTitle("Export as xlsx"));

    mButtonReportingSettings = new ContainerButton("Measure channel", "", mWindowMain);
    connect(mButtonReportingSettings, &ContainerButton::valueChanged, this,
            &PanelReporting::onExcelExportChannelsClicked);
    verticalLayoutXlsx->addWidget(mButtonReportingSettings->getEditableWidget());

    mButtonExportExcel = new ContainerButton("Start export", "icons8-export-excel-50.png", mWindowMain);
    connect(mButtonExportExcel, &ContainerButton::valueChanged, this, &PanelReporting::onExportToXlsxClicked);
    verticalLayoutXlsx->addWidget(mButtonExportExcel->getEditableWidget());
    mProgressExportExcel = createProgressBar(_2);
    verticalLayoutXlsx->addWidget(mProgressExportExcel);
    _2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  }

  //
  // Heatmap
  //
  {
    auto [verticalLayoutHeatmap, layout] = addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");
    verticalLayoutHeatmap->addWidget(createTitle("Heatmap"));
    // Settings

    mHeatmapSlice = std::shared_ptr<ContainerFunction<QString, int>>(
        new ContainerFunction<QString, int>("icons8-light-50.png", "[200,300]", "Image heatmap area size [px]",
                                            "200, 300", mWindowMain, "heatmap_image_area_size.json"));
    verticalLayoutHeatmap->addWidget(mHeatmapSlice->getEditableWidget());

    mGenerateHeatmapForWells = std::shared_ptr<ContainerFunction<bool, bool>>(new ContainerFunction<bool, bool>(
        "icons8-table-top-view-50.png", "Generate heatmap for wells", "Generate heatmap for wells", false, mWindowMain,
        "heatmap_generate_for_well.json"));
    verticalLayoutHeatmap->addWidget(mGenerateHeatmapForWells->getEditableWidget());

    mButtonReportingSettingsHeatmap = new ContainerButton("Measure channel", "", mWindowMain);
    connect(mButtonReportingSettingsHeatmap, &ContainerButton::valueChanged, this,
            &PanelReporting::onHeatmapExportChannelsClicked);
    verticalLayoutHeatmap->addWidget(mButtonReportingSettingsHeatmap->getEditableWidget());

    // Export button
    mButtonExportHeatmap = new ContainerButton("Start export", "icons8-heat-map-50.png", mWindowMain);
    connect(mButtonExportHeatmap, &ContainerButton::valueChanged, this, &PanelReporting::onExportToXlsxHeatmapClicked);
    verticalLayoutHeatmap->addWidget(mButtonExportHeatmap->getEditableWidget());
    mProgressHeatmap = createProgressBar(layout);
    verticalLayoutHeatmap->addWidget(mProgressHeatmap);
    layout->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  }

  verticalLayoutContainer->addStretch(0);

  //
  // Add layout
  //
  setLayout(horizontalLayout);
  horizontalLayout->addStretch();
}

PanelReporting::~PanelReporting()
{
}

QProgressBar *PanelReporting::createProgressBar(QWidget *parent)
{
  QProgressBar *progress = new QProgressBar(parent);
  progress->setVisible(false);
  progress->setMaximumHeight(8);
  progress->setTextVisible(false);
  progress->setContentsMargins(8, 8, 8, 0);
  return progress;
}

void PanelReporting::setActualSelectedResultsFolder(const QString &folder)
{
  mWindowMain->setMiddelLabelText(folder);
  mDirWatcher.setWorkingDirectory(folder.toStdString());
}

///
/// \brief      Export to xlsx
/// \author     Joachim Danmayr
///
void PanelReporting::onExportToXlsxClicked()
{
  static const std::string separator(1, std::filesystem::path::preferred_separator);

  // Write summary
  mExcelExporter = std::make_shared<ReportingExporterThread>(
      mProgressExportExcel, mButtonExportExcel, mDirWatcher,
      [this](const std::filesystem::path &overviewPath) {
        joda::results::WorkSheet details;
        details.loadFromFile(overviewPath.string());
        std::string outputFolder = overviewPath.parent_path().string() + separator + ".." + separator +
                                   joda::results::REPORT_EXPORT_FOLDER_PATH + separator +
                                   overviewPath.filename().string() + ".xlsx";
        joda::pipeline::reporting::ReportGenerator::flushReportToFile(
            details, mExcelReportSettings, outputFolder,
            joda::pipeline::reporting::ReportGenerator::OutputFormat::HORIZONTAL, true);
      },
      [this](const std::filesystem::path &detailResultPath) {
        joda::results::WorkSheet details;
        details.loadFromFile(detailResultPath.string());
        std::string outputFolder = detailResultPath.parent_path().string() + separator + ".." + separator +
                                   joda::results::REPORT_EXPORT_FOLDER_PATH + separator +
                                   detailResultPath.filename().string() + ".xlsx";
        joda::pipeline::reporting::ReportGenerator::flushReportToFile(
            details, mExcelReportSettings, outputFolder,
            joda::pipeline::reporting::ReportGenerator::OutputFormat::VERTICAL, false);
      });
}

///
/// \brief      Export to xlsx
/// \author     Joachim Danmayr
///
void PanelReporting::onExportToXlsxHeatmapClicked()
{
  static const std::string separator(1, std::filesystem::path::preferred_separator);

  QStringList pieces = mHeatmapSlice->getValue().split(",");
  std::set<int> sizes;
  for(const auto &part : pieces) {
    bool okay = false;
    int idx   = part.toInt(&okay);
    if(okay) {
      sizes.emplace(idx);
    }
  }

  // Write summary
  mExcelExporter = std::make_shared<ReportingExporterThread>(
      mProgressHeatmap, mButtonExportHeatmap, mDirWatcher,
      [this](const std::filesystem::path &overviewPath) {
        joda::results::WorkSheet alloverReport;
        alloverReport.loadFromFile(overviewPath.string());
        auto resultsFile = overviewPath.parent_path().string() + separator + ".." + separator +
                           joda::results::REPORT_EXPORT_FOLDER_PATH + separator + overviewPath.filename().string() +
                           "_heatmap.xlsx";

        joda::pipeline::reporting::Heatmap::createAllOverHeatMap(mHeatmapReportSettings, alloverReport, resultsFile);
      },
      [this](const std::filesystem::path &detailResultPath) {
        joda::results::WorkSheet detailReport;
        detailReport.loadFromFile(detailResultPath.string());
        auto resultsFile = detailResultPath.parent_path().string() + separator + ".." + separator +
                           joda::results::REPORT_EXPORT_FOLDER_PATH + separator + detailResultPath.filename().string() +
                           "_heatmap.xlsx";

        joda::pipeline::reporting::Heatmap::createHeatMapForImage(mHeatmapReportSettings, detailReport, resultsFile);
      });
}

///
/// \brief      Edit measurements for this channel
/// \author     Joachim Danmayr
///
void PanelReporting::onExcelExportChannelsClicked()
{
  DialogChannelMeasurement measure(mWindowMain, mExcelReportSettings);
  measure.exec();
}

///
/// \brief      Edit measurements for this channel
/// \author     Joachim Danmayr
///
void PanelReporting::onHeatmapExportChannelsClicked()
{
  DialogChannelMeasurement measure(mWindowMain, mHeatmapReportSettings);
  measure.exec();
}

/////////////////////////////////////////////////////////////////////////////
//
QLabel *PanelReporting::createTitle(const QString &title)
{
  auto *label = new QLabel();
  QFont font;
  font.setPixelSize(16);
  font.setBold(true);
  label->setFont(font);
  label->setText(title);

  return label;
}

QHBoxLayout *PanelReporting::createLayout()
{
  QScrollArea *scrollArea = new QScrollArea(this);
  scrollArea->setObjectName("scrollArea");
  scrollArea->setStyleSheet("QScrollArea#scrollArea { background-color: rgba(0, 0, 0, 0);}");
  scrollArea->setFrameStyle(0);
  scrollArea->setContentsMargins(0, 0, 0, 0);
  scrollArea->verticalScrollBar()->setStyleSheet(
      "QScrollBar:vertical {"
      "    border: none;"
      "    background: rgba(0, 0, 0, 0);"
      "    width: 6px;"
      "    margin: 0px 0px 0px 0px;"
      "}"
      "QScrollBar::handle:vertical {"
      "    background: rgba(32, 27, 23, 0.6);"
      "    min-height: 20px;"
      "    border-radius: 12px;"
      "}"
      "QScrollBar::add-line:vertical {"
      "    border: none;"
      "    background: rgba(0, 0, 0, 0);"
      "    height: 20px;"
      "    subcontrol-position: bottom;"
      "    subcontrol-origin: margin;"
      "}"
      "QScrollBar::sub-line:vertical {"
      "    border: none;"
      "    background: rgba(0, 0, 0, 0);"
      "    height: 20px;"
      "    subcontrol-position: top;"
      "    subcontrol-origin: margin;"
      "}");

  // Create a widget to hold the panels
  QWidget *contentWidget = new QWidget;
  contentWidget->setObjectName("contentOverview");
  contentWidget->setStyleSheet("QWidget#contentOverview { background-color: rgb(251, 252, 253);}");

  scrollArea->setWidget(contentWidget);
  scrollArea->setWidgetResizable(true);

  // Create a horizontal layout for the panels
  QHBoxLayout *horizontalLayout = new QHBoxLayout(contentWidget);
  horizontalLayout->setContentsMargins(16, 16, 16, 16);
  horizontalLayout->setSpacing(16);    // Adjust this value as needed
  contentWidget->setLayout(horizontalLayout);
  return horizontalLayout;
}

std::tuple<QVBoxLayout *, QWidget *> PanelReporting::addVerticalPanel(QLayout *horizontalLayout, const QString &bgColor,
                                                                      int margin, bool enableScrolling, int maxWidth,
                                                                      int spacing) const
{
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setSpacing(spacing);
  QWidget *contentWidget = new QWidget();

  layout->setContentsMargins(margin, margin, margin, margin);
  layout->setAlignment(Qt::AlignTop);

  contentWidget->setObjectName("verticalContentChannel");
  contentWidget->setLayout(layout);
  contentWidget->setStyleSheet(
      "QWidget#verticalContentChannel { border-radius: 12px; border: 2px none #696969; padding-top: 10px; "
      "padding-bottom: 10px;"
      "background-color: " +
      bgColor + ";}");

  if(enableScrolling) {
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    scrollArea->setObjectName("scrollArea");
    scrollArea->setStyleSheet("QScrollArea#scrollArea { background-color: rgba(0, 0, 0, 0);}");
    scrollArea->setFrameStyle(0);
    scrollArea->setContentsMargins(0, 0, 0, 0);
    scrollArea->verticalScrollBar()->setStyleSheet(
        "QScrollBar:vertical {"
        "    border: none;"
        "    background: rgba(0, 0, 0, 0);"
        "    width: 6px;"
        "    margin: 0px 0px 0px 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: rgba(32, 27, 23, 0.6);"
        "    min-height: 20px;"
        "    border-radius: 12px;"
        "}"
        "QScrollBar::add-line:vertical {"
        "    border: none;"
        "    background: rgba(0, 0, 0, 0);"
        "    height: 20px;"
        "    subcontrol-position: bottom;"
        "    subcontrol-origin: margin;"
        "}"
        "QScrollBar::sub-line:vertical {"
        "    border: none;"
        "    background: rgba(0, 0, 0, 0);"
        "    height: 20px;"
        "    subcontrol-position: top;"
        "    subcontrol-origin: margin;"
        "}");

    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumWidth(maxWidth);
    scrollArea->setMaximumWidth(maxWidth);

    horizontalLayout->addWidget(scrollArea);
    return {layout, scrollArea};
  }
  contentWidget->setMinimumWidth(maxWidth);
  contentWidget->setMaximumWidth(maxWidth);
  horizontalLayout->addWidget(contentWidget);

  return {layout, contentWidget};
}

}    // namespace joda::ui::qt
