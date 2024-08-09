///
/// \file      layout_generator.cpp
/// \author
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "layout_generator.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui::qt::helper {

LayoutGenerator::LayoutGenerator(PanelEdit *parent) : mParent(parent)
{
  auto *scrollArea = new QScrollArea();
  scrollArea->setObjectName("scrollArea");
  scrollArea->setFrameStyle(0);
  scrollArea->setContentsMargins(0, 0, 0, 0);
  scrollArea->verticalScrollBar()->setObjectName("scrollAreaV");

  // Create a widget to hold the panels
  auto *contentWidget = new QWidget;
  contentWidget->setObjectName("contentOverview");

  scrollArea->setWidget(contentWidget);
  scrollArea->setWidgetResizable(true);

  // Create a horizontal layout for the panels
  mMainLayout = new QHBoxLayout(contentWidget);
  mMainLayout->setContentsMargins(SPACING, SPACING, 0, 0);
  mMainLayout->setSpacing(SPACING);    // Adjust this value as needed
  mMainLayout->setAlignment(Qt::AlignLeft);
  contentWidget->setLayout(mMainLayout);

  auto *container = new QVBoxLayout(parent);

  auto *toolBarTop = new QToolBar();
  auto *backButton = new QAction(QIcon(":/icons/outlined/icons8-close-50.png"), "Close", toolBarTop);
  WindowMain::connect(backButton, &QAction::triggered, mParent->getWindowMain(), &WindowMain::onBackClicked);
  toolBarTop->addAction(backButton);

  auto *toolBarBottom = new QToolBar();
  auto *deleteChannel = new QAction(QIcon(":/icons/outlined/icons8-trash-50.png"), "Remove channel", toolBarBottom);
  deleteChannel->setToolTip("Delete channel!");
  WindowMain::connect(deleteChannel, &QAction::triggered, mParent->getWindowMain(),
                      &WindowMain::onRemoveChannelClicked);
  toolBarBottom->addAction(deleteChannel);

  container->addWidget(toolBarTop);
  container->addWidget(scrollArea);
  container->addWidget(toolBarBottom);

  parent->setLayout(container);
}

}    // namespace joda::ui::qt::helper
