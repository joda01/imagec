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

///

#include "layout_generator.hpp"
#include <qwidget.h>

namespace joda::ui::qt::helper {

LayoutGenerator::LayoutGenerator(QWidget *parent, bool withDeleteButton, bool withPanel) : mParent(parent)
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
  if(withPanel) {
    mMainLayout->setContentsMargins(SPACING, SPACING, 0, 0);
  } else {
    mMainLayout->setContentsMargins(0, 0, 0, 0);
  }
  mMainLayout->setSpacing(SPACING);    // Adjust this value as needed
  mMainLayout->setAlignment(Qt::AlignLeft);
  contentWidget->setLayout(mMainLayout);

  auto *container = new QVBoxLayout(parent);

  if(withPanel) {
    mToolbarTop = new QToolBar();
    mToolbarTop->setStyleSheet("QToolBar { border-bottom: 1px solid rgb(170, 170, 170); }");
    auto *spacerTop = new QWidget();
    spacerTop->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mSpaceTopToolbar = mToolbarTop->addWidget(spacerTop);
    auto *backButton = new QAction(QIcon(":/icons/outlined/icons8-close-50.png"), "Close", mToolbarTop);
    mToolbarTop->addAction(backButton);
  }

  if(withDeleteButton) {
    mToolbarBottom     = new QToolBar();
    auto *spacerBottom = new QWidget();
    spacerBottom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mToolbarBottom->addWidget(spacerBottom);
    auto *deleteChannel = new QAction(QIcon(":/icons/outlined/icons8-trash-50.png"), "Remove channel", mToolbarBottom);
    deleteChannel->setToolTip("Delete channel!");
    mToolbarBottom->addAction(deleteChannel);
  }

  if(withPanel) {
    container->addWidget(mToolbarTop);
  }
  container->addWidget(scrollArea);
  if(withDeleteButton) {
    container->addWidget(mToolbarBottom);
  }

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
QAction *LayoutGenerator::addItemToTopToolbar(QWidget *widget)
{
  return mToolbarTop->insertWidget(mSpaceTopToolbar, widget);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void LayoutGenerator::addItemToTopToolbar(QAction *widget)
{
  mToolbarTop->insertAction(mSpaceTopToolbar, widget);
}

void LayoutGenerator::VerticalPane::addGroup(const QString &title,
                                             const std::vector<std::shared_ptr<SettingBase>> &elements, int maxWidth)
{
  auto *group = new QGroupBox(title);
  group->setMaximumWidth(maxWidth);
  auto *layout = new QVBoxLayout;
  for(const auto &element : elements) {
    layout->addWidget(element->getEditableWidget());
    connect(element.get(), &SettingBase::valueChanged, layoutGenerator, &LayoutGenerator::onSettingChanged);
  }

  group->setLayout(layout);
  addWidget(group);
}

void LayoutGenerator::VerticalPane::addGroup(const std::vector<std::shared_ptr<SettingBase>> &elements, int maxWidth)
{
  auto *group = new QWidget();
  group->setContentsMargins(0, 0, 0, 0);
  group->setMaximumWidth(maxWidth);
  auto *layout = new QVBoxLayout;
  for(const auto &element : elements) {
    layout->addWidget(element->getEditableWidget());
    connect(element.get(), &SettingBase::valueChanged, layoutGenerator, &LayoutGenerator::onSettingChanged);
  }

  group->setLayout(layout);
  addWidget(group);
}

void LayoutGenerator::VerticalPane ::addGroup(const QString &title, const std::vector<QWidget *> &elements,
                                              int maxWidth)
{
  auto *group = new QGroupBox(title);
  group->setMaximumWidth(maxWidth);
  auto *layout = new QVBoxLayout;
  for(const auto &element : elements) {
    element->setParent(group);
    layout->addWidget(element);
  }
  group->setLayout(layout);
  addWidget(group);
}

}    // namespace joda::ui::qt::helper
