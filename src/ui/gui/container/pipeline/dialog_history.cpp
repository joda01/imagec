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
#include <qlabel.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <chrono>
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/window_main/window_main.hpp"
#include "panel_pipeline_settings.hpp"

namespace joda::ui::gui {

class TimeHistoryEntry : public QWidget
{
public:
  using QWidget::QWidget;

  explicit TimeHistoryEntry(const QIcon &icon, const QString &leftText, const std::chrono::system_clock::time_point &timestamp,
                            QWidget *parent = nullptr) :
      QWidget(parent),
      mIcon(icon), mLeftText(leftText), mTimeStamp(timestamp)
  {
  }

protected:
  void paintEvent(QPaintEvent *event) override
  {
    QPainter painter(this);
    int padding = 5;    // Padding around elements

    // Draw icon (assuming 16x16)
    int iconSize = 12;
    int iconY    = (height() - iconSize) / 2;
    painter.drawPixmap(padding, iconY, iconSize, iconSize, mIcon.pixmap(iconSize, iconSize));

    // Set font and pen for left text
    painter.setPen(Qt::black);
    QFont font;
    font.setPixelSize(8);
    painter.setFont(font);
    int textX = padding * 2 + iconSize;
    int textY = height() / 2 + painter.fontMetrics().height() / 4;
    painter.drawText(textX, textY, mLeftText);

    // Right vertical line
    // painter.setPen(QPen(Qt::darkGray, 1));    // Black line with 2-pixel thickness
    // int x = width() - 8;                      // 10 pixels from the right
    // painter.drawLine(x, 0, x, height());      // Vertical line from top to bottom  }

    // Set pen for right-aligned text (dark gray)
    painter.setPen(Qt::darkGray);
    QString tmp        = ::joda::helper::timepointToDelay(mTimeStamp).data();
    int rightTextWidth = painter.fontMetrics().horizontalAdvance(tmp);
    int rightTextX     = width() - padding - rightTextWidth;
    painter.drawText(rightTextX, textY, tmp);
  }

private:
  QIcon mIcon;
  QString mLeftText;
  std::chrono::system_clock::time_point mTimeStamp;
};

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogHistory::DialogHistory(WindowMain *parent, PanelPipelineSettings *panelPipelineSettings) :
    QDialog(parent), mWindowMain(parent), mPanelPipeline(panelPipelineSettings)
{
  setWindowTitle("Change history");
  mHistory = new PlaceholderTableWidget();
  mHistory->setPlaceholderText("Change history");
  mHistory->setColumnCount(1);
  mHistory->setColumnHidden(0, false);
  mHistory->setHorizontalHeaderLabels({"Timeline"});
  mHistory->verticalHeader()->setVisible(false);
  mHistory->horizontalHeader()->setVisible(false);
  mHistory->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
  mHistory->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
  // mHistory->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  mHistory->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  mHistory->setShowGrid(false);
  mHistory->setFrameStyle(QFrame::NoFrame);
  mHistory->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
void DialogHistory::show()
{
  QDialog::show();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogHistory::updateHistory(enums::HistoryCategory category, const std::string &text)
{
  if(mPanelPipeline == nullptr) {
    return;
  }
  auto entry = mPanelPipeline->mutablePipeline().createSnapShot(category, text);
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
auto DialogHistory::generateHistoryEntry(const std::optional<joda::settings::PipelineHistoryEntry> &inData) -> TimeHistoryEntry *
{
  if(!inData.has_value()) {
    return nullptr;
  }

  QIcon icon;
  switch(inData->category) {
    case enums::HistoryCategory::OTHER:
      icon = generateIcon("circle");
      break;
    case enums::HistoryCategory::ADDED:
      icon = generateIcon("plus-simple");
      break;
    case enums::HistoryCategory::DELETED:
      icon = generateIcon("minus-simple");
      break;
    case enums::HistoryCategory::CHANGED:
      icon = generateIcon("circle");
      break;
    case enums::HistoryCategory::SAVED:
      icon = generateIcon("save-simple");
      break;
  }
  // Set the icon in the first column
  auto *textIcon =
      new TimeHistoryEntry(icon, inData->commitMessage.data(), std::chrono::system_clock::time_point(std::chrono::seconds(inData->timeStamp)));
  return textIcon;
}
}    // namespace joda::ui::gui
