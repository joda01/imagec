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
#include "backend/results/analyzer/plugins/stats_for_image.hpp"
#include "backend/results/analyzer/plugins/stats_for_well.hpp"
#include "backend/results/exporter/exporter.hpp"
#include "backend/results/exporter/exporter_xlsx.hpp"
#include "backend/results/results.hpp"
#include "ui/container/container_function_base.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/reporting/plugins/panel_heatmap.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui::qt {

using namespace std::chrono_literals;
using namespace std::filesystem;

PanelReporting::PanelReporting(WindowMain *wm) : mWindowMain(wm)
{
  setObjectName("PanelReporting");
  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  mHeatmap = new reporting::plugin::PanelHeatmap(mWindowMain, this);
  layout->addWidget(mHeatmap);
  setLayout(layout);
}

PanelReporting::~PanelReporting()
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
  mWindowMain->setWindowTitlePrefix(imageCFile.filename().string().data());
  mAnalyzer = std::make_shared<joda::results::Analyzer>(imageCFile);
  mHeatmap->setAnalyzer(mAnalyzer);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
/*
void PanelReporting::loadDetailReportToTable()
{

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
}*/

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
/*
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
      results::WorkBook::readImageFromArchive(mArchive, controlImagePath), x, y);

  std::string outputFolder = mExportPath.string() + separator + joda::results::REPORT_EXPORT_FOLDER_PATH + separator +
                             results::RESULTS_SUMMARY_FILE_NAME + "_marked_" +
                             mActualSelectedWorksheet.getAnalyzeMeta().analyzeName + ".png";

  cv::imwrite(outputFolder, markedImage);

}*/

}    // namespace joda::ui::qt
