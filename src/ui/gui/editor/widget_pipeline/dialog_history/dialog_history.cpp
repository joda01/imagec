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
#include "backend/settings/pipeline/pipeline.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "table_model_history.hpp"

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
DialogHistory::DialogHistory(WindowMain *parent, joda::settings::Pipeline *pipelineSettings) :
    QDialog(parent), mWindowMain(parent), mPipelineSettings(pipelineSettings)
{
  setWindowTitle("Change history");

  {
    mTableHistory = new PlaceholderTableView(this);
    mTableHistory->setPlaceholderText("Press the + button to add a new pipeline.");
    mTableHistory->setFrameStyle(QFrame::NoFrame);
    mTableHistory->verticalHeader()->setVisible(false);
    mTableHistory->horizontalHeader()->setVisible(true);
    mTableHistory->setAlternatingRowColors(true);
    mTableHistory->setSelectionBehavior(QAbstractItemView::SelectRows);
    mTableHistory->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // mTableHistory->setItemDelegateForColumn(0, new HtmlDelegate(mPipelineTable));
    // mTableHistory->setItemDelegateForColumn(1, new ColorSquareDelegatePipeline(mPipelineTable));
    mTableModelHistory = new TableModelHistory(pipelineSettings, mTableHistory);
    mTableHistory->setModel(mTableModelHistory);

    connect(mTableHistory, &QTableView::doubleClicked, [this](const QModelIndex &index) { mPipelineSettings->restoreSnapShot(index.row()); });
    pipelineSettings->registerHistoryChangeCallback([this] { mTableModelHistory->refresh(); });
  }

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
    mPipelineSettings->clearHistory();
  });

  auto *addTagAction = toolBar->addAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("tag-simple"), "Add tag");
  addTagAction->setToolTip("Tag the actual settings in the history.");
  connect(addTagAction, &QAction::triggered, [this]() { createTag(); });

  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(mTableHistory);
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
  QDialog::show();
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
      mPipelineSettings->tag(text.toStdString());
    }
  }
}
}    // namespace joda::ui::gui
