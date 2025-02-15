///
/// \file      panel_pipeline_compile_log.cpp
/// \author    Joachim Danmayr
/// \date      2024-09-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "panel_pipeline_compile_log.hpp"
#include <qboxlayout.h>
#include <qdialog.h>
#include <qheaderview.h>
#include "backend/settings/analze_settings.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
PanelCompilerLog::PanelCompilerLog(WindowMain *parent)
{
  mLogOutput = new QTableWidget(1, 4);
  mLogOutput->verticalHeader()->setVisible(false);
  mLogOutput->setColumnHidden(0, true);
  mLogOutput->setColumnWidth(1, 250);
  mLogOutput->setColumnWidth(2, 250);
  mLogOutput->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
  mLogOutput->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
  mLogOutput->setHorizontalHeaderLabels({"", "Severity", "Location", "Message"});

  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(mLogOutput);

  //
  mDialog = new QDialog(parent);
  mDialog->setWindowTitle("Debug log");
  mDialog->setMinimumHeight(200);
  mDialog->setMinimumWidth(1000);
  mDialog->setLayout(layout);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelCompilerLog::showDialog()
{
  mDialog->show();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelCompilerLog::updateCompilerLog(const joda::settings::AnalyzeSettings &settings)
{
  std::lock_guard<std::mutex> lock(mWriteMutex);
  mNrOfErrors   = 0;
  auto addEntry = [this](const std::string &pipelineName, const SettingParserLog &log) {
    int newRow = mLogOutput->rowCount();
    mLogOutput->insertRow(newRow);
    // Set the icon in the first column
    auto *iconItem = new QTableWidgetItem();
    auto *sortItem = new QTableWidgetItem();
    QIcon *icon    = nullptr;
    if(log.severity == SettingParserLog::Severity::JODA_ERROR) {
      icon = new QIcon(generateIcon("error-red"));
      iconItem->setText("Error");
      sortItem->setText("A");
      mNrOfErrors++;
    } else if(log.severity == SettingParserLog::Severity::JODA_WARNING) {
      icon = new QIcon(generateIcon("warning-yellow"));
      iconItem->setText("Warning");
      sortItem->setText("B");
    } else if(log.severity == SettingParserLog::Severity::JODA_INFO) {
      icon = new QIcon(generateIcon("info-blue"));
      iconItem->setText("Info");
      sortItem->setText("C");
    }
    iconItem->setIcon(icon->pixmap(16, 16));
    iconItem->setFlags(iconItem->flags() & ~Qt::ItemIsEditable);
    mLogOutput->setItem(newRow, 0, sortItem);
    mLogOutput->setItem(newRow, 1, iconItem);

    //
    //
    //
    auto *pipeline = new QTableWidgetItem();

    // We have following string >N4joda8settings18ExperimentSettingsE< we just want the end
    std::regex pattern("([A-Za-z]+[0-9]+)+");
    std::smatch match;
    std::string doDelete;
    if(std::regex_search(log.commandNameOfOccurrence, match, pattern)) {
      doDelete = match[0];
    }
    QString loc(log.commandNameOfOccurrence.data());
    auto result = loc.replace(doDelete.data(), "");
    result.chop(1);
    if(!pipelineName.empty()) {
      result = QString(pipelineName.data()) + "/" + result;
    }
    pipeline->setText(result);
    pipeline->setFlags(iconItem->flags() & ~Qt::ItemIsEditable);
    mLogOutput->setItem(newRow, 2, pipeline);

    //
    //
    //
    auto *message = new QTableWidgetItem();
    message->setText(log.message.data());
    message->setFlags(iconItem->flags() & ~Qt::ItemIsEditable);
    mLogOutput->setItem(newRow, 3, message);
  };

  for(int row = 0; row < mLogOutput->rowCount(); ++row) {
    for(int col = 0; col < mLogOutput->columnCount(); ++col) {
      QTableWidgetItem *item = mLogOutput->takeItem(row, col);    // Take ownership of the item
      delete item;                                                // Delete the item to free memory
    }
  }
  mLogOutput->setRowCount(0);
  auto errors = settings.checkForErrors();
  for(const auto &[pipelineName, error] : errors) {
    for(const auto &log : error) {
      addEntry(pipelineName, log);
    }
  }

  if(mLogOutput->rowCount() > 0) {
    mLogOutput->sortItems(0, Qt::AscendingOrder);    // Sort in ascending order
    mLogOutput->selectRow(0);
  }
}

}    // namespace joda::ui::gui
