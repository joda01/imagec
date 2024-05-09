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
#include <qnamespace.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qtableview.h>
#include <qtablewidget.h>
#include <qwidget.h>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "../window_main.hpp"
#include "backend/pipelines/reporting/reporting_generator.hpp"
#include "backend/pipelines/reporting/reporting_heatmap.hpp"
#include "backend/postprocessing/postprocessing.hpp"
#include "backend/results/results.hpp"
#include "ui/container/container_function_base.hpp"
#include "ui/reporting/exporter_thread.hpp"
#include "dialog_channel_measurment.hpp"

namespace joda::ui::qt {

using namespace std::chrono_literals;
using namespace std::filesystem;

PanelReporting::PanelReporting(WindowMain *wm) : mWindowMain(wm)
{
  // setStyleSheet("border: 1px solid black; padding: 10px;");
  setObjectName("PanelReporting");

  auto *horizontalLayout             = createLayout();
  auto [verticalLayoutContainer, _1] = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 250, 16);

  //
  // Selector
  //
  {
    connect(this, &PanelReporting::loadingFilesfinished, this, &PanelReporting::onLoadingFileFinished);

    auto [selector, _2] = addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");
    selector->addWidget(createTitle("Results"));
    mSelectorLayout = selector;

    mProgressSelector = createProgressBar(_2);
    mProgressSelector->setVisible(true);
    mProgressSelector->setRange(0, 0);
    mProgressSelector->setMaximum(0);
    mProgressSelector->setMinimum(0);
    selector->addWidget(mProgressSelector);
    _2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  }

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

  {
    auto [tableContainer, _1] = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 16777215, 16);

    mTable = new QTableWidget(0, 0, _1);
    mTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QObject::connect(mTable, &QTableView::doubleClicked, this, &PanelReporting::onTableDoubleClicked);

    tableContainer->addWidget(mTable);
  }

  //
  // Add layout
  //
  setLayout(horizontalLayout);
  // horizontalLayout->addStretch();
}

PanelReporting::~PanelReporting()
{
  mSearchFileStopToken = true;
  if(mLoadingFilesThread != nullptr && mLoadingFilesThread->joinable()) {
    mLoadingFilesThread->join();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelReporting::lookingForFilesThread()
{
  entry.clear();
  mFoundFiles.clear();
  mFoundFiles = results::WorkBook::listResultsFiles(mSelectedImageCFile.string(),
                                                    joda::results::MESSAGE_PACK_FILE_EXTENSION, &mSearchFileStopToken);
  entry.reserve(mFoundFiles.size());
  for(const auto &en : mFoundFiles) {
    entry.push_back({en.string().data(), en.string().data()});
  }

  emit loadingFilesfinished();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelReporting::onLoadingFileFinished()
{
  mFileSelector = std::shared_ptr<ContainerFunction<QString, int>>(new ContainerFunction<QString, int>(
      "icons8-folder-50.png", "Results files", "Results files", "", "", entry, mWindowMain, ""));
  mSelectorLayout->insertWidget(1, mFileSelector->getEditableWidget());
  connect(mFileSelector.get(), &ContainerFunctionBase::valueChanged, this, &PanelReporting::onResultsFileSelected);
  onResultsFileSelected();
  mProgressSelector->setVisible(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelReporting::onResultsFileSelected()
{
  loadDetailReportToTable(results::WorkBook::readWorksheetFromArchive(mSelectedImageCFile.string(),
                                                                      mFileSelector->getValue().toStdString()));
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
QProgressBar *PanelReporting::createProgressBar(QWidget *parent)
{
  QProgressBar *progress = new QProgressBar(parent);
  progress->setVisible(false);
  progress->setMaximumHeight(8);
  progress->setTextVisible(false);
  progress->setContentsMargins(8, 8, 8, 0);
  return progress;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelReporting::setActualSelectedWorkingFile(const std::filesystem::path &imageCFile)
{
  mWindowMain->setMiddelLabelText(imageCFile.filename().string().data());
  mSelectedImageCFile = imageCFile;

  if(mFileSelector != nullptr) {
    mSelectorLayout->removeWidget(mFileSelector->getEditableWidget());
  }
  mProgressSelector->setVisible(true);
  mProgressSelector->setRange(0, 0);
  mProgressSelector->setMaximum(0);
  mProgressSelector->setMinimum(0);

  if(mLoadingFilesThread != nullptr && mLoadingFilesThread->joinable()) {
    mLoadingFilesThread->join();
  }
  mLoadingFilesThread = std::make_shared<std::thread>(&PanelReporting::lookingForFilesThread, this);
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
      mProgressExportExcel, mButtonExportExcel, mSelectedImageCFile, mFoundFiles,
      [this](const results::WorkSheet &overviewPath) {
        std::string outputFolder = mSelectedImageCFile.parent_path().string() + separator +
                                   joda::results::REPORT_EXPORT_FOLDER_PATH + separator +
                                   results::RESULTS_SUMMARY_FILE_NAME + "_" + overviewPath.getJobMeta().jobName;
        joda::pipeline::reporting::ReportGenerator::flushReportToFile(
            overviewPath, mExcelReportSettings, outputFolder,
            joda::pipeline::reporting::ReportGenerator::OutputFormat::HORIZONTAL, true);
      },
      [this](const results::WorkSheet &details) {
        std::string outputFolder = mSelectedImageCFile.parent_path().string() + separator +
                                   joda::results::REPORT_EXPORT_FOLDER_PATH + separator +
                                   results::RESULTS_IMAGE_FILE_NAME + "_" + details.getImageMeta()->imageFileName +
                                   "_" + details.getJobMeta().jobName;
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
      mProgressHeatmap, mButtonExportHeatmap, mSelectedImageCFile, mFoundFiles,
      [this](const results::WorkSheet &overviewPath) {
        std::string outputFolder = mSelectedImageCFile.parent_path().string() + separator +
                                   joda::results::REPORT_EXPORT_FOLDER_PATH + separator +
                                   results::RESULTS_SUMMARY_FILE_NAME + "_heatmap_" + overviewPath.getJobMeta().jobName;

        joda::pipeline::reporting::Heatmap::createAllOverHeatMap(mHeatmapReportSettings, overviewPath, outputFolder);
      },
      [this, sizes](const results::WorkSheet &details) {
        std::string outputFolder = mSelectedImageCFile.parent_path().string() + separator +
                                   joda::results::REPORT_EXPORT_FOLDER_PATH + separator +
                                   results::RESULTS_IMAGE_FILE_NAME + "_heatmap_" +
                                   details.getImageMeta()->imageFileName + "_" + details.getJobMeta().jobName;

        joda::pipeline::reporting::Heatmap::createHeatMapForImage(sizes, mHeatmapReportSettings, details, outputFolder);
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

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelReporting::loadDetailReportToTable(const results::WorkSheet &sheet)
{
  mActualSelectedWorksheet = sheet;
  auto &imageMeta          = sheet.getImageMeta();
  if(imageMeta.has_value()) {
    auto firstChannel = sheet.root().getChannels().begin();
    auto &channel     = firstChannel->second;

    mTable->clear();

    mTable->setRowCount(channel.getNrOfObjects());
    mTable->setColumnCount(channel.getMeasuredChannels().size() + 1);

    std::map<MeasureKey, int> colIdxMap;

    int colIdx                   = 1;
    QTableWidgetItem *headerItem = new QTableWidgetItem(QString("Ctrl"));
    mTable->setHorizontalHeaderItem(0, headerItem);
    for(const auto &[measureKey, measur] : channel.getMeasuredChannels()) {
      colIdxMap[measureKey]        = colIdx;
      QTableWidgetItem *headerItem = new QTableWidgetItem(QString(measur.name.data()));
      mTable->setHorizontalHeaderItem(colIdx, headerItem);
      colIdx++;
    }

    int rowIdx = 0;
    for(const auto &[objextKey, object] : channel.getObjects()) {
      {
        auto &tileInfo  = object.getObjectMeta().tileInfo;
        int64_t tileIdx = 0;
        if(tileInfo.has_value()) {
          tileIdx = tileInfo->tileIndex;
        }
        std::string controlImagePath = channel.getChannelMeta().controlImagePath;
        helper::stringReplace(controlImagePath, "${tileIdx}", std::to_string(tileIdx));
        QTableWidgetItem *newItem = new QTableWidgetItem(QString(controlImagePath.data()));
        mTable->setItem(rowIdx, 0, newItem);
      }

      for(const auto &[key, val] : object.getMeasurements()) {
        if(std::holds_alternative<double>(val.getVal())) {
          QTableWidgetItem *newItem = new QTableWidgetItem(QString(QString::number(std::get<double>(val.getVal()))));
          mTable->setItem(rowIdx, colIdxMap[key], newItem);
        }
      }
      rowIdx++;
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelReporting::onTableDoubleClicked(const QModelIndex &index)
{
  static const std::string separator(1, std::filesystem::path::preferred_separator);

  const auto &channel = mActualSelectedWorksheet.root().getChannels().begin()->second;
  const auto &objects = channel.getObjects();
  auto measure        = objects.at(index.row());
  int x               = std::get<double>(
      measure
          .at(joda::results::MeasureChannelKey{results::ReportingSettings::MeasureChannels::CENTER_OF_MASS_X,
                                               results::ReportingSettings::MeasureChannelStat::VAL})
          .getVal());

  int y = std::get<double>(
      measure
          .at(joda::results::MeasureChannelKey{results::ReportingSettings::MeasureChannels::CENTER_OF_MASS_Y,
                                               results::ReportingSettings::MeasureChannelStat::VAL})
          .getVal());

  std::string controlImagePath = channel.getChannelMeta().controlImagePath;
  helper::stringReplace(controlImagePath, "${tileIdx}", std::to_string(measure.getObjectMeta().tileInfo->tileIndex));
  auto markedImage = joda::image::postprocessing::PostProcessor::markPositionInImage(
      results::WorkBook::readImageFromArchive(mSelectedImageCFile.string(), controlImagePath), x, y);

  std::string outputFolder = mSelectedImageCFile.parent_path().string() + separator +
                             joda::results::REPORT_EXPORT_FOLDER_PATH + separator + results::RESULTS_SUMMARY_FILE_NAME +
                             "_marked_" + mActualSelectedWorksheet.getJobMeta().jobName + ".png";

  cv::imwrite(outputFolder, markedImage);
}

}    // namespace joda::ui::qt
