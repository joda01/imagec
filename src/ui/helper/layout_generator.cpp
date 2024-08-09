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

  mToolbarTop     = new QToolBar();
  auto *spacerTop = new QWidget();
  spacerTop->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  mSpaceTopToolbar = mToolbarTop->addWidget(spacerTop);
  auto *backButton = new QAction(QIcon(":/icons/outlined/icons8-close-50.png"), "Close", mToolbarTop);
  WindowMain::connect(backButton, &QAction::triggered, mParent->getWindowMain(), &WindowMain::onBackClicked);
  mToolbarTop->addAction(backButton);

  auto *toolBarBottom = new QToolBar();
  auto *spacerBottom  = new QWidget();
  spacerBottom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  toolBarBottom->addWidget(spacerBottom);
  auto *deleteChannel = new QAction(QIcon(":/icons/outlined/icons8-trash-50.png"), "Remove channel", toolBarBottom);
  deleteChannel->setToolTip("Delete channel!");
  WindowMain::connect(deleteChannel, &QAction::triggered, mParent->getWindowMain(),
                      &WindowMain::onRemoveChannelClicked);
  toolBarBottom->addAction(deleteChannel);

  container->addWidget(mToolbarTop);
  container->addWidget(scrollArea);
  container->addWidget(toolBarBottom);

  parent->setLayout(container);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void LayoutGenerator::addSeparatorToTopToolbar()
{
  mToolbarTop->insertSeparator(mSpaceTopToolbar);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void LayoutGenerator::addItemToTopToolbar(QWidget *widget)
{
  mToolbarTop->insertWidget(mSpaceTopToolbar, widget);
}

}    // namespace joda::ui::qt::helper
