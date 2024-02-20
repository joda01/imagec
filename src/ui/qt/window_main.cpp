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
  setMinimumSize(800, 600);
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
  mStackedWidget->addWidget(createChannelScrollArea());

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

  auto createVerticalContainer = []() -> std::tuple<QVBoxLayout *, QWidget *> {
    QWidget *contentWidget = new QWidget;
    QVBoxLayout *layout    = new QVBoxLayout(contentWidget);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(16);    // Adjust this value as needed
    contentWidget->setLayout(layout);
    return {layout, contentWidget};
  };

  {
    auto [channelsOverViewLayout, channelsOverviewWidget] = createVerticalContainer();
    PanelChannelOverview *panel1                          = new PanelChannelOverview();
    channelsOverViewLayout->addWidget(panel1);
    PanelChannelOverview *panel2 = new PanelChannelOverview();
    channelsOverViewLayout->addWidget(panel2);

    QPushButton *addChannel = new QPushButton();
    addChannel->setStyleSheet(
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
    addChannel->setText("Add Channel");
    connect(addChannel, &QPushButton::pressed, this, &WindowMain::onAddChannelClicked);

    channelsOverViewLayout->addWidget(addChannel);

    channelsOverViewLayout->addStretch();
    horizontalLayout->addWidget(channelsOverviewWidget);
  }

  {
    auto [channelsOverViewLayout, channelsOverviewWidget] = createVerticalContainer();
    PanelChannelOverview *panel1                          = new PanelChannelOverview();
    channelsOverViewLayout->addWidget(panel1);
    PanelChannelOverview *panel2 = new PanelChannelOverview();
    channelsOverViewLayout->addWidget(panel2);
    channelsOverViewLayout->addStretch();
    horizontalLayout->addWidget(channelsOverviewWidget);
  }

  horizontalLayout->addStretch();

  return scrollArea;
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createChannelScrollArea()
{
  QScrollArea *scrollArea = new QScrollArea(this);
  scrollArea->setFrameStyle(0);
  scrollArea->setObjectName("scrollAreaMainWindow");
  scrollArea->setStyleSheet("QScrollArea#scrollAreaMainWindow { background-color: rgb(251, 252, 253);}");

  // Create a widget to hold the panels
  QWidget *contentWidget = new QWidget;
  contentWidget->setObjectName("contentWidget");
  contentWidget->setStyleSheet("QWidget#contentWidget { background-color: rgb(251, 252, 253);}");

  scrollArea->setWidget(contentWidget);
  scrollArea->setWidgetResizable(true);

  // Create a horizontal layout for the panels
  QHBoxLayout *horizontalLayout = new QHBoxLayout(contentWidget);
  horizontalLayout->setContentsMargins(16, 16, 16, 16);
  horizontalLayout->setSpacing(16);    // Adjust this value as needed
  contentWidget->setLayout(horizontalLayout);

  // Add some panels (QLabels in this example)
  for(int i = 0; i < 3; ++i) {
    PanelChannel *panel = new PanelChannel();
    horizontalLayout->addWidget(panel);
  }

  horizontalLayout->addStretch();

  return scrollArea;
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
  mStackedWidget->setCurrentIndex(1);
}

}    // namespace joda::ui::qt
