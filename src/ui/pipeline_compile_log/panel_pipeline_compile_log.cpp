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
#include "ui/window_main/window_main.hpp"

namespace joda::ui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
PanelCompilerLog::PanelCompilerLog(WindowMain *parent)
{
  mLogOutput = new QTableWidget(1, 3);
  mLogOutput->verticalHeader()->setVisible(false);
  mLogOutput->setColumnWidth(0, 150);
  mLogOutput->setColumnWidth(1, 250);
  mLogOutput->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
  mLogOutput->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
  mLogOutput->setHorizontalHeaderLabels({"Severity", "Location", "Message"});

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
  auto addEntry = [this](const std::string &pipelineName, const SettingParserLog &log) {
    int newRow = mLogOutput->rowCount();
    mLogOutput->insertRow(newRow);
    // Set the icon in the first column
    auto *iconItem = new QTableWidgetItem();
    QIcon *icon    = nullptr;
    if(log.severity == SettingParserLog::Severity::JODA_ERROR) {
      icon = new QIcon(":/icons/icons/icons8-error-50.png");
      iconItem->setText("Error");
    } else if(log.severity == SettingParserLog::Severity::JODA_WARNING) {
      icon = new QIcon(":/icons/icons/icons8-warning-50.png");
      iconItem->setText("Warning");
    } else if(log.severity == SettingParserLog::Severity::JODA_INFO) {
      icon = new QIcon(":/icons/icons/icons8-info-50-blue.png");
      iconItem->setText("Info");
    }
    iconItem->setIcon(icon->pixmap(16, 16));

    iconItem->setFlags(iconItem->flags() & ~Qt::ItemIsEditable);
    mLogOutput->setItem(newRow, 0, iconItem);

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
    mLogOutput->setItem(newRow, 1, pipeline);

    //
    //
    //
    auto *message = new QTableWidgetItem();
    message->setText(log.message.data());
    message->setFlags(iconItem->flags() & ~Qt::ItemIsEditable);
    mLogOutput->setItem(newRow, 2, message);
  };

  mLogOutput->setRowCount(0);
  auto errors = settings.checkForErrors();
  for(const auto &[pipelineName, error] : errors) {
    for(const auto &log : error) {
      addEntry(pipelineName, log);
    }
  }
}

}    // namespace joda::ui
