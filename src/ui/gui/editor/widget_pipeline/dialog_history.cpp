///
/// \file      dialog_history.cpp
/// \author    Joachim Danmayr
/// \date      2025-02-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "dialog_history.hpp"
#include <qdialog.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qtoolbar.h>
#include <chrono>
#include <utility>
#include "backend/helper/logger/console_logger.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "panel_pipeline_settings.hpp"

namespace joda::ui::gui {

class TimeHistoryEntry : public QWidget
{
public:
  using QWidget::QWidget;

  explicit TimeHistoryEntry(QIcon icon, QString leftText, const std::chrono::system_clock::time_point &timestamp, QWidget *parent = nullptr) :
      QWidget(parent), mIcon(std::move(icon)), mLeftText(std::move(leftText)), mTimeStamp(timestamp)
  {
  }
  void updateContent(const QIcon &icon, const QString &txt)
  {
    mIcon     = icon;
    mLeftText = txt;
    update();
  }

protected:
  void paintEvent(QPaintEvent * /*event*/) override
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
  connect(mHistory, &QTableWidget::cellDoubleClicked, [&](int row, int /*column*/) { restoreHistory(row); });

  //
  // Toolbar
  //
  auto *toolBar = new QToolBar();

  // Create a stretchable spacer
  auto *spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  toolBar->addWidget(spacer);    // This pushes everything after it to the right

  auto *clearHistory = toolBar->addAction(generateSvgIcon<Style::REGULAR, Color::RED>("trash-simple"), "Clear history");
  clearHistory->setToolTip("Clear history without tags.");
  connect(clearHistory, &QAction::triggered, [this]() {
    QMessageBox messageBox(mWindowMain);
    messageBox.setIconPixmap(generateSvgIcon<Style::REGULAR, Color::YELLOW>("warning").pixmap(48, 48));
    messageBox.setWindowTitle("Clear history?");
    messageBox.setText("Clear history and keep tags?");
    QPushButton *noButton = messageBox.addButton(tr("No"), QMessageBox::NoRole);
    messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
    messageBox.setDefaultButton(noButton);
    messageBox.exec();
    if(messageBox.clickedButton() == noButton) {
      return;
    }
    mPanelPipeline->mutablePipeline().clearHistory();
    loadHistory();
    mWindowMain->checkForSettingsChanged();
  });

  auto *addTagAction = toolBar->addAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("tag-simple"), "Add tag");
  addTagAction->setToolTip("Tag the actual settings in the history.");
  connect(addTagAction, &QAction::triggered, [this]() { createTag(); });

  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(mHistory);
  layout->addWidget(toolBar);
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
  if(mPanelPipeline == nullptr) {
    return;
  }
  updateSelection();
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
  updateSelection();
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
  const auto &history = mPanelPipeline->mutablePipeline().getHistory();
  mHistory->setRowCount(static_cast<int32_t>(history.size()));
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
  try {
    auto dataIn = mPanelPipeline->mutablePipeline().restoreSnapShot(index);
    mPanelPipeline->clearPipeline();
    mPanelPipeline->fromSettings(dataIn);
    updateSelection();
  } catch(...) {
  }
  // updateHistory("Restored: " + std::to_string(index));
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogHistory::updateSelection()
{
  auto idx = mPanelPipeline->mutablePipeline().getHistoryIndex();
  if(mHistory->rowCount() > static_cast<int32_t>(idx)) {
    mHistory->selectRow(static_cast<int32_t>(idx));
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogHistory::undo()
{
  if(mPanelPipeline == nullptr) {
    return;
  }
  try {
    auto dataIn = mPanelPipeline->mutablePipeline().undo();
    mPanelPipeline->clearPipeline();
    mPanelPipeline->fromSettings(dataIn);
    updateSelection();
  } catch(...) {
  }
  // updateHistory("Restored: " + std::to_string(index));
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogHistory::createTag()
{
  QInputDialog inputDialog(mWindowMain);
  inputDialog.setWindowTitle("Create tag");
  inputDialog.setLabelText("Tag name:");
  inputDialog.setInputMode(QInputDialog::TextInput);
  auto ret = inputDialog.exec();
  if(QInputDialog::Accepted == ret) {
    QString text = inputDialog.textValue();
    if(!text.isEmpty()) {
      try {
        mPanelPipeline->mutablePipeline().tag(text.toStdString());
        (static_cast<TimeHistoryEntry *>(mHistory->cellWidget(0, 0)))
            ->updateContent(generateSvgIcon<Style::REGULAR, Color::BLACK>("tag-simple"), text);
        mHistory->update();
        mHistory->viewport()->update();
        mWindowMain->checkForSettingsChanged();
      } catch(...) {
        joda::log::logWarning("Could not create tag!");
      }
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
auto DialogHistory::generateHistoryEntry(const std::optional<joda::settings::PipelineHistoryEntry> &inData) -> TimeHistoryEntry *
{
  if(!inData.has_value()) {
    return nullptr;
  }

  QIcon icon;
  QString text;
  if(!inData->tagMessage.empty()) {
    icon = generateSvgIcon<Style::REGULAR, Color::BLACK>("tag-simple");
    text = inData->tagMessage.data();
  } else {
    text = inData->commitMessage.data();
    switch(inData->category) {
      case enums::HistoryCategory::OTHER:
        icon = generateSvgIcon<Style::REGULAR, Color::BLACK>("circle");
        break;
      case enums::HistoryCategory::ADDED:
        icon = generateSvgIcon<Style::REGULAR, Color::BLACK>("list-plus");
        break;
      case enums::HistoryCategory::DELETED:
        icon = generateSvgIcon<Style::REGULAR, Color::RED>("trash-simple");
        break;
      case enums::HistoryCategory::CHANGED:
        icon = generateSvgIcon<Style::REGULAR, Color::BLACK>("circle");
        break;
      case enums::HistoryCategory::SAVED:
        icon = generateSvgIcon<Style::REGULAR, Color::GREEN>("floppy-disk");
        break;
    }
  }
  // Set the icon in the first column
  auto *textIcon = new TimeHistoryEntry(icon, text, std::chrono::system_clock::time_point(std::chrono::seconds(inData->timeStamp)));
  return textIcon;
}
}    // namespace joda::ui::gui
