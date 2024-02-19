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
#include <QAction>
#include <QIcon>
#include <QMainWindow>
#include <QToolBar>
#include "ui/qt/panel_channel.hpp"

namespace joda::ui::qt {

WindowMain::WindowMain()
{
  setWindowTitle("imageC");
  createToolbar();
  createChannelScrollArea();
  setMinimumSize(800, 600);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::createToolbar()
{
  auto *toolbar = addToolBar("toolbar");

  // Create an action with an icon
  auto *saveProject = new QAction(QIcon(":/icons/save_20.png"), "Save", this);
  saveProject->setToolTip("Save project!");
  connect(saveProject, &QAction::triggered, this, &WindowMain::onOpenFolderClicked);
  toolbar->addAction(saveProject);

  auto *openFolder = new QAction(QIcon(":/icons/opened_folder_20.png"), "Open", this);
  openFolder->setToolTip("Open folder!");
  connect(openFolder, &QAction::triggered, this, &WindowMain::onOpenFolderClicked);
  toolbar->addAction(openFolder);

  toolbar->addSeparator();

  auto *start = new QAction(QIcon(":/icons/start_20.png"), "Start", this);
  start->setToolTip("Start analysis!");
  connect(start, &QAction::triggered, this, &WindowMain::onOpenFolderClicked);
  toolbar->addAction(start);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::createChannelScrollArea()
{
  QScrollArea *scrollArea = new QScrollArea(this);
  setCentralWidget(scrollArea);

  // Create a widget to hold the panels
  QWidget *contentWidget = new QWidget;
  scrollArea->setWidget(contentWidget);
  scrollArea->setWidgetResizable(true);

  // Create a horizontal layout for the panels
  QHBoxLayout *horizontalLayout = new QHBoxLayout(contentWidget);
  contentWidget->setLayout(horizontalLayout);

  // Add some panels (QLabels in this example)
  for(int i = 0; i < 10; ++i) {
    PanelChannel *panel = new PanelChannel();
    horizontalLayout->addWidget(panel);
  }
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

}    // namespace joda::ui::qt
