///
/// \file      dialog_history.cpp
/// \author    Joachim Danmayr
/// \date      2025-02-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "dialog_history.hpp"
#include <qdialog.h>
#include "ui/gui/window_main/window_main.hpp"
#include "panel_pipeline_settings.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogHistory::DialogHistory(WindowMain *parent) : QDialog(parent), mWindowMain(parent)
{
  mHistory = new PlaceholderTableWidget();
  mHistory->setPlaceholderText("Change history");
  mHistory->setColumnCount(1);
  mHistory->setColumnHidden(0, false);
  mHistory->setHorizontalHeaderLabels({"Timeline"});
  mHistory->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  mHistory->verticalHeader()->setVisible(false);
  mHistory->horizontalHeader()->setVisible(false);
  mHistory->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
  mHistory->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
  mHistory->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
  mHistory->setShowGrid(false);
  mHistory->setFrameStyle(QFrame::NoFrame);
  mHistory->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  // Set transparent background using stylesheet
  mHistory->setStyleSheet(
      "QTableWidget {"
      "   background-color: transparent;"
      "}"
      "QHeaderView::section {"
      "   background-color: transparent;"
      "   border: none;"
      "}"
      "QTableWidget::item {"
      "   background-color: transparent;"
      "   border: none;"
      "}");

  connect(mHistory, &QTableWidget::cellDoubleClicked, [&](int row, int column) { restoreHistory(row); });

  auto *layout = new QVBoxLayout();
  layout->addWidget(mHistory);
  setLayout(layout);
  setMinimumHeight(600);
  setMinimumWidth(300);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogHistory::show(PanelPipelineSettings *panelPipelineSettings)
{
  mPanelPipeline = panelPipelineSettings;
  QDialog::show();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogHistory::updateHistory(const std::string &text)
{
  if(mPanelPipeline == nullptr) {
    return;
  }
  auto entry = mPanelPipeline->mutablePipeline().createSnapShot(text);
  if(entry.has_value()) {
    mHistory->insertRow(0);
    mHistory->setCellWidget(0, 0, generateHistoryEntry(entry));
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogHistory::loadHistory()
{
  if(mPanelPipeline == nullptr) {
    return;
  }
  const auto &history = mPanelPipeline->mutablePipeline().history;
  mHistory->setRowCount(history.size());
  int idx = 0;
  for(const auto &step : history) {
    mHistory->setCellWidget(idx, 0, generateHistoryEntry(step));
    idx++;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogHistory::restoreHistory(int32_t index)
{
  if(mPanelPipeline == nullptr) {
    return;
  }
  mPanelPipeline->clearPipeline();
  auto data = mPanelPipeline->mutablePipeline().restoreSnapShot(index);
  mPanelPipeline->fromSettings(data);
  // updateHistory("Restored: " + std::to_string(index));
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto DialogHistory::generateHistoryEntry(const std::optional<joda::settings::PipelineHistoryEntry> &inData) -> QLabel *
{
  if(!inData.has_value()) {
    return nullptr;
  }
  QString textTmp =
      QString(inData->commitMessage.data()) + "<br/><span style='color:gray;'><i>" +
      joda::helper::timepointToIsoString(std::chrono::high_resolution_clock::time_point(std::chrono::seconds(inData->timeStamp))).data() +
      "</i></span>";
  // Set the icon in the first column
  auto *textIcon = new QLabel();
  textIcon->setText(textTmp);
  textIcon->setTextFormat(Qt::RichText);
  QFont font = textIcon->font();
  font.setPixelSize(10);
  textIcon->setFont(font);
  return textIcon;
}
}    // namespace joda::ui::gui
