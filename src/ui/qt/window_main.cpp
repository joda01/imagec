///
/// \file      window_main.cpp
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

#include "window_main.hpp"
#include <qcombobox.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <QAction>
#include <QIcon>
#include <QMainWindow>
#include <QToolBar>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include "container_channel.hpp"

namespace joda::ui::qt {

using namespace std::chrono_literals;

WindowMain::WindowMain(joda::ctrl::Controller *controller) : mController(controller)
{
  setWindowTitle("imageC");
  createToolbar();
  setMinimumSize(1300, 900);
  setObjectName("windowMain");
  setStyleSheet(
      "QMainWindow#windowMain {"
      "   background-color: rgb(251, 252, 253); "
      "   border: none;"
      "}");
  setCentralWidget(createStackedWidget());

  // Start with the main page
  onBackClicked();

  mMainThread = new std::thread(&WindowMain::waitForFileSearchFinished, this);
  connect(this, &WindowMain::lookingForFilesFinished, this, &WindowMain::onLookingForFilesFinished);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::createToolbar()
{
  auto *toolbar = addToolBar("toolbar");
  toolbar->setMovable(false);
  toolbar->setStyleSheet("QToolBar {background-color: rgb(251, 252, 253); border: 0px; border-bottom: 0px;}");

  // Left
  {
    mBackButton = new QAction(QIcon(":/icons/outlined/icons8-left-50.png"), "Back", toolbar);
    mBackButton->setEnabled(false);
    connect(mBackButton, &QAction::triggered, this, &WindowMain::onBackClicked);
    toolbar->addAction(mBackButton);
    mFirstSeparator = toolbar->addSeparator();

    mSaveProject = new QAction(QIcon(":/icons/outlined/icons8-save-50.png"), "Save", toolbar);
    mSaveProject->setToolTip("Save project!");
    connect(mSaveProject, &QAction::triggered, this, &WindowMain::onSaveProjectClicked);
    toolbar->addAction(mSaveProject);

    mOPenProject = new QAction(QIcon(":/icons/outlined/icons8-folder-50.png"), "Open", toolbar);
    mOPenProject->setToolTip("Open folder!");
    connect(mOPenProject, &QAction::triggered, this, &WindowMain::onOpenProjectClicked);
    toolbar->addAction(mOPenProject);

    mStartAnalysis = new QAction(QIcon(":/icons/outlined/icons8-play-50.png"), "Start", toolbar);
    mStartAnalysis->setToolTip("Start analysis!");
    connect(mStartAnalysis, &QAction::triggered, this, &WindowMain::onStartClicked);
    toolbar->addAction(mStartAnalysis);
    mSecondSeparator = toolbar->addSeparator();
  }

  {
    // Add a spacer to push the next action to the middle
    QWidget *spacerWidget = new QWidget();
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolbar->addWidget(spacerWidget);
  }

  // Middle
  {
    // Add the QComboBox in the middle
    mFoundFilesCombo = new QComboBox(toolbar);
    mFoundFilesCombo->setMinimumWidth(250);
    mFoundFilesCombo->setMaximumWidth(300);
    mFileSelectorComboBox = toolbar->addWidget(mFoundFilesCombo);
    mFileSelectorComboBox->setVisible(false);

    mImageSeriesCombo = new QComboBox(toolbar);
    mImageSeriesCombo->addItem("Series 0", 0);
    mImageSeriesCombo->addItem("Series 1", 1);
    mImageSeriesCombo->addItem("Series 2", 2);
    mImageSeriesCombo->addItem("Series 3", 3);
    mImageSeriesComboBox = toolbar->addWidget(mImageSeriesCombo);
    mImageSeriesComboBox->setVisible(false);

    mFoundFilesHint = new ClickableLabel(toolbar);
    mFoundFilesHint->setText("Please open a working directory ...");
    mFileSearchHintLabel = toolbar->addWidget(mFoundFilesHint);
    connect(mFoundFilesHint, &ClickableLabel::clicked, this, &WindowMain::onOpenProjectClicked);
  }

  // Right
  {
    QWidget *spacerWidget = new QWidget();
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolbar->addWidget(spacerWidget);
  }

  toolbar->addSeparator();

  {
    mDeleteChannel = new QAction(QIcon(":/icons/outlined/icons8-trash-50.png"), "Remove channel", toolbar);
    mDeleteChannel->setToolTip("Delete channel!");
    connect(mDeleteChannel, &QAction::triggered, this, &WindowMain::onRemoveChannelClicked);
    toolbar->addAction(mDeleteChannel);

    mSettings = new QAction(QIcon(":/icons/outlined/icons8-settings-50.png"), "Settings", toolbar);
    mSettings->setToolTip("Settings");
    connect(mSettings, &QAction::triggered, this, &WindowMain::onOpenProjectClicked);
    toolbar->addAction(mSettings);

    toolbar->addSeparator();

    mShowInfoDialog = new QAction(QIcon(":/icons/outlined/icons8-info-50.png"), "Info", toolbar);
    mShowInfoDialog->setToolTip("Info");
    connect(mShowInfoDialog, &QAction::triggered, this, &WindowMain::onShowInfoDialog);
    toolbar->addAction(mShowInfoDialog);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createStackedWidget()
{
  mStackedWidget = new QStackedWidget();
  mStackedWidget->setObjectName("stackedWidget");
  mStackedWidget->addWidget(createOverviewWidget());
  mStackedWidget->addWidget(createChannelWidget());
  return mStackedWidget;
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createOverviewWidget()
{
  QScrollArea *scrollArea = new QScrollArea(this);
  scrollArea->setFrameStyle(0);
  scrollArea->setObjectName("scrollAreaOverview");
  scrollArea->setStyleSheet("QScrollArea#scrollAreaOverview { background-color: rgb(251, 252, 253);}");

  // Create a widget to hold the panels
  QWidget *contentWidget = new QWidget;
  contentWidget->setObjectName("contentOverview");
  contentWidget->setStyleSheet("QWidget#contentOverview { background-color: rgb(251, 252, 253);}");

  scrollArea->setWidget(contentWidget);
  scrollArea->setWidgetResizable(true);

  // Create a horizontal layout for the panels
  QHBoxLayout *horizontalLayout = new QHBoxLayout(contentWidget);
  horizontalLayout->setObjectName("mainWindowHLayout");
  horizontalLayout->setContentsMargins(16, 16, 16, 16);
  horizontalLayout->setSpacing(16);    // Adjust this value as needed
  contentWidget->setLayout(horizontalLayout);

  auto createVerticalContainer = []() -> std::tuple<QGridLayout *, QWidget *> {
    QWidget *contentWidget = new QWidget;
    QGridLayout *layout    = new QGridLayout(contentWidget);
    layout->setObjectName("mainWindowGridLayout");
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(8);    // Adjust this value as needed
    contentWidget->setLayout(layout);
    return {layout, contentWidget};
  };

  {
    auto [channelsOverViewLayout, channelsOverviewWidget] = createVerticalContainer();
    mLayoutChannelOverview                                = channelsOverViewLayout;

    channelsOverViewLayout->addWidget(createAddChannelPanel());
    mLastElement = new QLabel();
    channelsOverViewLayout->addWidget(mLastElement, 1, 0, 1, 3);

    channelsOverViewLayout->setRowStretch(0, 1);
    channelsOverViewLayout->setRowStretch(1, 1);
    channelsOverViewLayout->setRowStretch(2, 1);
    channelsOverViewLayout->setRowStretch(4, 3);

    // channelsOverViewLayout->addStretch();

    horizontalLayout->addStretch();
    horizontalLayout->addWidget(channelsOverviewWidget);
  }

  {
    /*
    auto [channelsOverViewLayout, channelsOverviewWidget] = createVerticalContainer();
    PanelChannelOverview *panel1                          = new PanelChannelOverview();
    channelsOverViewLayout->addWidget(panel1);
    PanelChannelOverview *panel2 = new PanelChannelOverview();
    channelsOverViewLayout->addWidget(panel2);
    channelsOverViewLayout->addStretch();
    horizontalLayout->addWidget(channelsOverviewWidget);
    */
  }

  horizontalLayout->addStretch();

  return scrollArea;
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createChannelWidget()
{
  return new QWidget(this);
}

QWidget *WindowMain::createAddChannelPanel()
{
  QWidget *addChannelWidget = new QWidget();
  // setStyleSheet("border: 1px solid black; padding: 10px;");
  addChannelWidget->setObjectName("PanelChannelOverview");
  addChannelWidget->setMinimumHeight(250);
  addChannelWidget->setMinimumWidth(350);
  addChannelWidget->setMaximumWidth(350);
  QGridLayout *layout = new QGridLayout(this);
  layout->setObjectName("mainWindowChannelGridLayout");
  addChannelWidget->setStyleSheet(
      "QWidget#PanelChannelOverview { border-radius: 12px; border: 2px solid rgba(0, 104, 117, 0.05); padding-top: "
      "10px; "
      "padding-bottom: 10px;"
      "background-color: rgba(0, 104, 117, 0);}");

  addChannelWidget->setLayout(layout);
  layout->setSpacing(0);

  QPushButton *addChannelButton = new QPushButton();
  addChannelButton->setStyleSheet(
      "QPushButton {"
      "   background-color: rgba(0, 0, 0, 0);"
      "   border: 1px solid rgb(111, 121, 123);"
      "   color: rgb(0, 104, 117);"
      "   padding: 10px 20px;"
      "   border-radius: 12px;"
      "   font-size: 14px;"
      "   font-weight: normal;"
      "   text-align: center;"
      "   text-decoration: none;"
      "}"

      "QPushButton:hover {"
      "   background-color: rgba(0, 0, 0, 0);"    // Darken on hover
      "}"

      "QPushButton:pressed {"
      "   background-color: rgba(0, 0, 0, 0);"    // Darken on press
      "}");
  addChannelButton->setText("Add Channel");
  connect(addChannelButton, &QPushButton::pressed, this, &WindowMain::onAddChannelClicked);
  layout->addWidget(addChannelButton);

  addChannelWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  mAddChannelPanel = addChannelWidget;
  return addChannelWidget;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onOpenProjectClicked()
{
  QString folderToOpen = QDir::homePath();
  if(mSelectedWorkingDirectory.isEmpty()) {
    folderToOpen = mSelectedWorkingDirectory;
  }
  QString selectedDirectory = QFileDialog::getExistingDirectory(this, "Select a directory", folderToOpen);

  if(selectedDirectory.isEmpty()) {
    return;
  }
  mSelectedWorkingDirectory = selectedDirectory;

  std::lock_guard<std::mutex> lock(mLookingForFilesMutex);
  mFoundFilesHint->setText("Looking for images ...");
  mFoundFilesCombo->clear();
  mFileSelectorComboBox->setVisible(false);
  mImageSeriesComboBox->setVisible(false);
  mFileSearchHintLabel->setVisible(true);
  mController->setWorkingDirectory(mSelectedWorkingDirectory.toStdString());
  mNewFolderSelected = true;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::waitForFileSearchFinished()
{
  while(true) {
    while(true) {
      {
        std::lock_guard<std::mutex> lock(mLookingForFilesMutex);
        if(mNewFolderSelected) {
          break;
        }
      }
      std::this_thread::sleep_for(2s);
    }
    while(mController->isLookingForFiles()) {
      std::this_thread::sleep_for(500ms);
    }
    {
      std::lock_guard<std::mutex> lock(mLookingForFilesMutex);
      mNewFolderSelected = false;
    }
    emit lookingForFilesFinished();
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onLookingForFilesFinished()
{
  for(const auto &file : mController->getListOfFoundImages()) {
    mFoundFilesCombo->addItem(QString(file.getFilename().data()), QString(file.getPath().data()));
  }
  if(mController->getNrOfFoundImages() > 0) {
    mFoundFilesCombo->setCurrentIndex(0);
    mFoundFilesHint->setText("Finished");
    mFileSearchHintLabel->setVisible(false);
    mFileSelectorComboBox->setVisible(true);
    mImageSeriesComboBox->setVisible(true);
  } else {
    // mFoundFilesCombo->setVisible(false);
    mFoundFilesHint->setText("No images found!");
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onSaveProjectClicked()
{
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onStartClicked()
{
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onBackClicked()
{
  mBackButton->setEnabled(false);
  mSaveProject->setVisible(true);
  mSaveProject->setVisible(true);
  mOPenProject->setVisible(true);
  mStartAnalysis->setVisible(true);
  mSettings->setVisible(true);
  mDeleteChannel->setVisible(false);
  mFirstSeparator->setVisible(true);
  mSecondSeparator->setVisible(true);

  mStackedWidget->setCurrentIndex(0);
  mSelectedChannel = nullptr;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::showChannelEdit(ContainerChannel *selectedChannel)
{
  mSelectedChannel = selectedChannel;

  mBackButton->setEnabled(true);
  mSaveProject->setVisible(false);
  mSaveProject->setVisible(false);
  mOPenProject->setVisible(false);
  mStartAnalysis->setVisible(false);
  mSettings->setVisible(false);
  mDeleteChannel->setVisible(true);
  mFirstSeparator->setVisible(false);
  mSecondSeparator->setVisible(false);

  mStackedWidget->removeWidget(mStackedWidget->widget(1));
  mStackedWidget->addWidget(selectedChannel->getEditPanel());
  mStackedWidget->setCurrentIndex(1);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onRemoveChannelClicked()
{
  if(mSelectedChannel != nullptr) {
    QMessageBox messageBox(this);
    auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
    messageBox.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Dialog);
    // messageBox.setAttribute(Qt::WA_TranslucentBackground);
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Remove channel?");
    messageBox.setText("Do you want to remove the channel?");
    messageBox.addButton(tr("No"), QMessageBox::NoRole);
    messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
    auto reply = messageBox.exec();
    std::cout << std::to_string(reply) << std::endl;
    if(reply == 1) {
      removeChannel(mSelectedChannel);
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onAddChannelClicked()
{
  if(mAddChannelPanel != nullptr) {
    {
      int row = (mChannels.size() + 1) / 3;
      int col = (mChannels.size() + 1) % 3;
      mLayoutChannelOverview->removeWidget(mAddChannelPanel);
      mLayoutChannelOverview->removeWidget(mLastElement);
      mLayoutChannelOverview->addWidget(mAddChannelPanel, row, col);
      mLayoutChannelOverview->addWidget(mLastElement, row + 1, 0, 1, 3);
    }

    int row     = mChannels.size() / 3;
    int col     = mChannels.size() % 3;
    auto panel1 = new ContainerChannel(this);
    mLayoutChannelOverview->addWidget(panel1->getOverviewPanel(), row, col);
    mChannels.emplace(panel1);
  }
}

void WindowMain::removeChannel(ContainerChannel *toRemove)
{
  /// \todo reorder
  if(toRemove != nullptr) {
    mChannels.erase(toRemove);
    mLayoutChannelOverview->removeWidget(toRemove->getOverviewPanel());
    toRemove->getOverviewPanel()->setParent(nullptr);

    // Reorder all panels
    int cnt = 0;
    for(const auto &panelToReorder : mChannels) {
      mLayoutChannelOverview->removeWidget(panelToReorder->getOverviewPanel());
      int row = (cnt) / 3;
      int col = (cnt) % 3;
      mLayoutChannelOverview->addWidget(panelToReorder->getOverviewPanel(), row, col);
      cnt++;
    }

    {
      int row = (mChannels.size()) / 3;
      int col = (mChannels.size()) % 3;
      mLayoutChannelOverview->removeWidget(mAddChannelPanel);
      mLayoutChannelOverview->removeWidget(mLastElement);
      mLayoutChannelOverview->addWidget(mAddChannelPanel, row, col);
      mLayoutChannelOverview->addWidget(mLastElement, row + 1, 0, 1, 3);
    }
    onBackClicked();
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \return
///
void WindowMain::onShowInfoDialog()
{
  QMessageBox messageBox(this);
  // auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
  messageBox.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Dialog);
  // messageBox.setAttribute(Qt::WA_TranslucentBackground);
  // messageBox.setIconPixmap(icon->pixmap(42, 42));
  messageBox.setWindowTitle("Info");
  messageBox.setText("<html>imageC 1.0.0-alpha.1</html>");
  messageBox.addButton(tr("Close"), QMessageBox::AcceptRole);
  messageBox.exec();
}

}    // namespace joda::ui::qt
