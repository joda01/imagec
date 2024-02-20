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
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qstackedwidget.h>
#include <QAction>
#include <QIcon>
#include <QMainWindow>
#include <QToolBar>
#include "ui/qt/panel_channel.hpp"
#include "ui/qt/panel_channel_overview.hpp"

namespace joda::ui::qt {

WindowMain::WindowMain()
{
  setWindowTitle("imageC");
  createToolbar();
  setMinimumSize(1300, 900);
  setObjectName("windowMain");
  setStyleSheet("QMainWindow#windowMain {background-color: rgb(251, 252, 253); border: none;}");

  setCentralWidget(createStackedWidget());
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

  // Create an action with an icon
  auto *saveProject = new QAction(QIcon(":/icons/icons8-save-50.png"), "Save", this);
  saveProject->setToolTip("Save project!");
  connect(saveProject, &QAction::triggered, this, &WindowMain::onOpenFolderClicked);
  toolbar->addAction(saveProject);

  auto *openFolder = new QAction(QIcon(":/icons/icons8-folder-50.png"), "Open", this);
  openFolder->setToolTip("Open folder!");
  connect(openFolder, &QAction::triggered, this, &WindowMain::onOpenFolderClicked);
  toolbar->addAction(openFolder);

  toolbar->addSeparator();

  auto *start = new QAction(QIcon(":/icons/icons8-play-50.png"), "Start", this);
  start->setToolTip("Start analysis!");
  connect(start, &QAction::triggered, this, &WindowMain::onOpenFolderClicked);
  toolbar->addAction(start);

  toolbar->addSeparator();

  auto *settings = new QAction(QIcon(":/icons/icons8-settings-50.png"), "Settings", this);
  settings->setToolTip("Settings");
  connect(settings, &QAction::triggered, this, &WindowMain::onOpenFolderClicked);
  toolbar->addAction(settings);
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createStackedWidget()
{
  mStackedWidget = new QStackedWidget();

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
  horizontalLayout->setContentsMargins(16, 16, 16, 16);
  horizontalLayout->setSpacing(16);    // Adjust this value as needed
  contentWidget->setLayout(horizontalLayout);

  auto createVerticalContainer = []() -> std::tuple<QGridLayout *, QWidget *> {
    QWidget *contentWidget = new QWidget;
    QGridLayout *layout    = new QGridLayout(contentWidget);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(8);    // Adjust this value as needed
    contentWidget->setLayout(layout);
    return {layout, contentWidget};
  };

  {
    auto [channelsOverViewLayout, channelsOverviewWidget] = createVerticalContainer();
    mLayoutChannelOverview                                = channelsOverViewLayout;
    mAddChannelButton                                     = new QPushButton();
    mAddChannelButton->setStyleSheet(
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
    mAddChannelButton->setText("Add Channel");
    connect(mAddChannelButton, &QPushButton::pressed, this, &WindowMain::onAddChannelClicked);

    channelsOverViewLayout->addWidget(mAddChannelButton);
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
  return new PanelChannel(this);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onOpenFolderClicked()
{
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
void WindowMain::onAddChannelClicked()
{
  {
    int row = (mChannels.size() + 1) / 3;
    int col = (mChannels.size() + 1) % 3;
    mLayoutChannelOverview->removeWidget(mAddChannelButton);
    mLayoutChannelOverview->removeWidget(mLastElement);
    mLayoutChannelOverview->addWidget(mAddChannelButton, row, col);
    mLayoutChannelOverview->addWidget(mLastElement, row + 1, 0, 1, 3);
  }

  int row                      = mChannels.size() / 3;
  int col                      = mChannels.size() % 3;
  PanelChannelOverview *panel1 = new PanelChannelOverview(this);
  mLayoutChannelOverview->addWidget(panel1, row, col);
  mChannels.emplace(panel1);
}

void WindowMain::showOverview()
{
  mSelectedChannel = nullptr;
  mStackedWidget->setCurrentIndex(0);
}

void WindowMain::showChannelEdit(PanelChannelOverview *sel)
{
  mSelectedChannel = sel;
  mStackedWidget->setCurrentIndex(1);
}

void WindowMain::removeChannel()
{
  /// \todo reorder
  if(mSelectedChannel != nullptr) {
    mChannels.erase(mSelectedChannel);
    mLayoutChannelOverview->removeWidget(mSelectedChannel);

    {
      int row = (mChannels.size() + 1) / 3;
      int col = (mChannels.size() + 1) % 3;

      mLayoutChannelOverview->removeWidget(mAddChannelButton);
      mLayoutChannelOverview->removeWidget(mLastElement);
      mLayoutChannelOverview->addWidget(mAddChannelButton, row, col);
      mLayoutChannelOverview->addWidget(mLastElement, row + 1, 0, 1, 3);
    }
  }
}

}    // namespace joda::ui::qt
