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
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include "../window_main.hpp"
#include "backend/results/analyzer/plugins/stats_for_image.hpp"
#include "backend/results/analyzer/plugins/stats_for_well.hpp"
#include "backend/results/exporter/exporter_xlsx.hpp"
#include "backend/results/results.hpp"
#include "ui/container/container_function_base.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/reporting/plugins/panel_heatmap.hpp"

namespace joda::ui::qt {

using namespace std::chrono_literals;
using namespace std::filesystem;

PanelReporting::PanelReporting(WindowMain *wm) : mWindowMain(wm)
{
  // setStyleSheet("border: 1px solid black; padding: 10px;");
  setObjectName("PanelReporting");

  auto [horizontalLayout, _] = joda::ui::qt::helper::createLayout(this);
  auto [verticalLayoutContainer, _1] =
      joda::ui::qt::helper::addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 250, 250, 16);

  //
  // Selector
  //
  {
    connect(this, &PanelReporting::loadingFilesfinished, this, &PanelReporting::onLoadingFileFinished);

    auto [selector, _2] = joda::ui::qt::helper::addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");
    selector->addWidget(createTitle("Selector"));
    mSelectorLayout = selector;

    //
    mAnalyzeSelector = std::shared_ptr<ContainerFunction<QString, int>>(new ContainerFunction<QString, int>(
        "icons8-folder-50.png", "Analysis", "Analysis", "", "", {}, mWindowMain, ""));
    selector->addWidget(mAnalyzeSelector->getEditableWidget());
    mAnalyzeSelector->getEditableWidget()->setVisible(false);
    connect(mAnalyzeSelector.get(), &ContainerFunctionBase::valueChanged, this, &PanelReporting::onResultsFileSelected);

    //
    mChannelSelector = std::shared_ptr<ContainerFunction<joda::results::ChannelIndex, int>>(
        new ContainerFunction<joda::results::ChannelIndex, int>("icons8-folder-50.png", "Channel", "Channel", "",
                                                                joda::results::ChannelIndex::ME, {}, mWindowMain, ""));
    selector->addWidget(mChannelSelector->getEditableWidget());
    connect(mChannelSelector.get(), &ContainerFunctionBase::valueChanged, this, &PanelReporting::onChannelChanged);

    //
    mMeasureChannelSelector = std::shared_ptr<ContainerFunction<uint32_t, int>>(new ContainerFunction<uint32_t, int>(
        "icons8-folder-50.png", "Measurement", "Measurement", "", 0, {}, mWindowMain, ""));
    selector->addWidget(mMeasureChannelSelector->getEditableWidget());
    connect(mMeasureChannelSelector.get(), &ContainerFunctionBase::valueChanged, this,
            &PanelReporting::onMeasurementChanged);

    mStats =
        std::shared_ptr<ContainerFunction<joda::results::Stats, int>>(new ContainerFunction<joda::results::Stats, int>(
            "icons8-folder-50.png", "Statistics", "Statistics", "", joda::results::Stats::AVG,
            {{joda::results::Stats::AVG, "AVG"},
             {joda::results::Stats::MEDIAN, "MEDIAN"},
             {joda::results::Stats::MIN, "MIN"},
             {joda::results::Stats::MAX, "MAX"},
             {joda::results::Stats::STDDEV, "STDDEV"},
             {joda::results::Stats::SUM, "SUM"}},
            mWindowMain, ""));
    selector->addWidget(mStats->getEditableWidget());
    connect(mStats.get(), &ContainerFunctionBase::valueChanged, this, &PanelReporting::onMeasurementChanged);

    _2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  }

  //
  // Plate settings
  //
  {
    auto [verticalLayoutHeatmap, layout] =
        joda::ui::qt::helper::addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");
    verticalLayoutHeatmap->addWidget(createTitle("Heatmap"));
    // Settings

    mHeatmapSlice = std::shared_ptr<ContainerFunction<QString, int>>(
        new ContainerFunction<QString, int>("icons8-light-50.png", "[200,300]", "Image heatmap area size [px]", "200",
                                            mWindowMain, "heatmap_image_area_size.json"));
    verticalLayoutHeatmap->addWidget(mHeatmapSlice->getEditableWidget());
    connect(mHeatmapSlice.get(), &ContainerFunctionBase::valueChanged, this, &PanelReporting::onMeasurementChanged);

    mPlateSize = std::shared_ptr<ContainerFunction<uint32_t, uint32_t>>(
        new ContainerFunction<uint32_t, uint32_t>("icons8-full-image-50.png", "Plate size", "Plate size", "", 1624,
                                                  {
                                                      {1, "1", ""},
                                                      {203, "2 x 3", ""},
                                                      {304, "3 x 4", ""},
                                                      {406, "4 x 6", ""},
                                                      {608, "6 x 8", ""},
                                                      {812, "8 x 12", ""},
                                                      {1624, "16 x 24", ""},
                                                      {3248, "32 x 48", ""},
                                                      {4872, "48 x 72", ""},
                                                  },
                                                  mWindowMain, "heatmap_generate_for_well.json"));
    connect(mPlateSize.get(), &ContainerFunctionBase::valueChanged, this, &PanelReporting::onMeasurementChanged);
    verticalLayoutHeatmap->addWidget(mPlateSize->getEditableWidget());
  }

  //
  // Excel export
  //
  {
    auto [verticalLayoutXlsx, _2] =
        joda::ui::qt::helper::addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");
    verticalLayoutXlsx->addWidget(createTitle("Export"));

    mButtonReportingSettings = new ContainerButton("Measure channel", "", mWindowMain);
    verticalLayoutXlsx->addWidget(mButtonReportingSettings->getEditableWidget());

    mButtonExportExcel = new ContainerButton("Start export", "icons8-export-excel-50.png", mWindowMain);
    connect(mButtonExportExcel, &ContainerButton::valueChanged, this, &PanelReporting::onExportToXlsxClicked);
    verticalLayoutXlsx->addWidget(mButtonExportExcel->getEditableWidget());
    mProgressExportExcel = createProgressBar(_2);
    verticalLayoutXlsx->addWidget(mProgressExportExcel);
    _2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  }

  verticalLayoutContainer->addStretch(0);

  auto [tableContainer, tableContainerLayout] = joda::ui::qt::helper::addVerticalPanel(
      horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 16777215, 16777215, 0);

  {
      // mTable = new QTableWidget(0, 0, tableContainerLayout);
      // mTable->setSelectionBehavior(QAbstractItemView::SelectRows);
      // mTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
      // QObject::connect(mTable, &QTableView::doubleClicked, this, &PanelReporting::onTableDoubleClicked);

      // tableContainer->addWidget(mTable);
  } {
    tableContainer->setContentsMargins(0, 0, 0, 0);
    mHeatmap = new reporting::plugin::PanelHeatmap(mWindowMain, tableContainerLayout);
    tableContainer->addWidget(mHeatmap);
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
  onResultsFileSelected();
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
void PanelReporting::close()
{
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
  mExportPath = imageCFile.parent_path();
  mWindowMain->setMiddelLabelText(imageCFile.filename().string().data());
  mAnalyzer = std::make_shared<joda::results::Analyzer>(imageCFile);

  std::string analysisId;
  {
    // Analysis
    auto analyses = mAnalyzer->getAnalyzes();
    std::vector<ContainerFunction<QString, int>::ComboEntry> entry;
    for(const auto &analyse : analyses) {
      if(analysisId.empty()) {
        analysisId = analyse.analyzeId;
      }
      entry.push_back(ContainerFunction<QString, int>::ComboEntry{
          .key = analyse.analyzeId.data(), .label = analyse.name.data(), .icon = ""});
    }
    if(!analyses.empty()) {
      mAnalyzeSelector->setOptions("icons8-gantt-chart-50.png", entry, analysisId.data());
    } else {
      mAnalyzeSelector->setOptions("icons8-gantt-chart-50.png", {}, "");
    }
  }
  {
    // Channels
    mChannelInfos = mAnalyzer->getChannelsForAnalyses(analysisId);
    std::vector<ContainerFunction<joda::results::ChannelIndex, int>::ComboEntry> entry;
    for(const auto &channel : mChannelInfos) {
      entry.push_back(ContainerFunction<joda::results::ChannelIndex, int>::ComboEntry{
          .key = channel.channelId, .label = channel.name.data(), .icon = ""});
    }
    if(!mChannelInfos.empty()) {
      mChannelSelector->setOptions("icons8-sheets-50.png", entry, mChannelInfos[0].channelId);
    } else {
      mChannelSelector->setOptions("icons8-sheets-50.png", {}, joda::results::ChannelIndex::ME);
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
void PanelReporting::onChannelChanged()
{
  for(const auto &channel : mChannelInfos) {
    if(channel.channelId == mChannelSelector->getValue()) {
      std::vector<ContainerFunction<uint32_t, int>::ComboEntry> entry;
      for(const auto &measure : channel.measurements) {
        entry.push_back(ContainerFunction<uint32_t, int>::ComboEntry{
            .key = measure.getKey(), .label = measure.toString().data(), .icon = ""});
      }
      if(!entry.empty()) {
        mMeasureChannelSelector->setOptions("icons8-filter-50.png", entry, entry[0].key);
      } else {
        mMeasureChannelSelector->setOptions("icons8-filter-50.png", {}, 0);
      }
      break;
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
void PanelReporting::onMeasurementChanged()
{
  auto value    = mPlateSize->getValue();
  uint32_t rows = value / 100;
  uint32_t cols = value % 100;

  mFilter = reporting::plugin::PanelHeatmap::SelectedFilter{
      .analyzeId          = mAnalyzeSelector->getValue().toStdString(),
      .plateRows          = rows,
      .plateCols          = cols,
      .plateId            = 1,
      .channelIdx         = mChannelSelector->getValue(),
      .measureChannel     = joda::results::MeasureChannelId(mMeasureChannelSelector->getValue()),
      .stats              = mStats->getValue(),
      .densityMapAreaSize = mHeatmapSlice->getValue().toUInt()};

  mHeatmap->setData(mAnalyzer, mFilter);
}

///
/// \brief      Export to xlsx
/// \author     Joachim Danmayr
///
void PanelReporting::onExportToXlsxClicked()
{
  QString filePath = QFileDialog::getSaveFileName(this, "Save File", QDir::homePath(), "XLSX Files (*.xlsx)");
  if(filePath.isEmpty()) {
    return;
  }

  switch(mHeatmap->getActualNavigation()) {
    case reporting::plugin::PanelHeatmap::Navigation::PLATE:
      break;
    case reporting::plugin::PanelHeatmap::Navigation::WELL: {
      auto result = joda::results::analyze::plugins::StatsPerWell::getData(
          *mAnalyzer, mFilter.plateId, mHeatmap->getSelectedWell(), mFilter.channelIdx, mFilter.measureChannel);
      joda::results::exporter::ExporterXlsx::startExport(result, filePath.toStdString());
    } break;
    case reporting::plugin::PanelHeatmap::Navigation::IMAGE:
      auto result = joda::results::analyze::plugins::StatsPerImage::getData(
          *mAnalyzer, mFilter.plateId, mHeatmap->getSelectedImage(), mFilter.channelIdx, mFilter.measureChannel);
      joda::results::exporter::ExporterXlsx::startExport(result, filePath.toStdString());
      break;
  }
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

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelReporting::loadDetailReportToTable()
{
  /*
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
  }*/
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
  /*
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
      results::WorkBook::readImageFromArchive(mArchive, controlImagePath), x, y);

  std::string outputFolder = mExportPath.string() + separator + joda::results::REPORT_EXPORT_FOLDER_PATH + separator +
                             results::RESULTS_SUMMARY_FILE_NAME + "_marked_" +
                             mActualSelectedWorksheet.getAnalyzeMeta().analyzeName + ".png";

  cv::imwrite(outputFolder, markedImage);
  */
}

}    // namespace joda::ui::qt
