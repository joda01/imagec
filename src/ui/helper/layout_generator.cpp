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
#include <qtabwidget.h>
#include <qwidget.h>

namespace joda::ui::helper {

LayoutGenerator::LayoutGenerator(QWidget *parent, bool withDeleteButton, bool withTopToolbar, bool withBackButton) :
    mParent(parent)
{
  auto *scrollArea = new QScrollArea();
  scrollArea->setObjectName("scrollArea");
  scrollArea->setFrameStyle(0);
  scrollArea->setContentsMargins(0, 0, 0, 0);
  scrollArea->verticalScrollBar()->setObjectName("scrollAreaV");

  // Create a widget to hold the panels
  auto *contentWidget = new QWidget;
  contentWidget->setContentsMargins(0, SPACING, 0, 0);
  contentWidget->setObjectName("contentOverview");

  scrollArea->setWidget(contentWidget);
  scrollArea->setWidgetResizable(true);

  QTabWidget *tabWidget = new QTabWidget();

  // Create a horizontal layout for the panels
  mMainLayout = new QHBoxLayout(contentWidget);
  if(withDeleteButton) {
    mMainLayout->setContentsMargins(SPACING, 0, SPACING, 0);
  } else {
    mMainLayout->setContentsMargins(SPACING, 0, SPACING, SPACING);
  }
  mMainLayout->setSpacing(SPACING);    // Adjust this value as needed
  mMainLayout->setAlignment(Qt::AlignLeft);

  contentWidget->setLayout(mMainLayout);

  auto *container = new QVBoxLayout(parent);
  container->setContentsMargins(0, 0, 0, 0);
  container->setSpacing(0);

  if(withTopToolbar) {
    mToolbarTop = new QToolBar();
    mToolbarTop->setContentsMargins(0, 0, 0, 0);
    mToolbarTop->setStyleSheet("QToolBar { border-bottom: 1px solid rgb(170, 170, 170); }");
    if(withBackButton) {
      auto *spacerTop = new QWidget();
      spacerTop->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
      mSpaceTopToolbar = mToolbarTop->addWidget(spacerTop);
      mBackButton = new QAction(QIcon(":/icons/outlined/icons8-close-50.png").pixmap(16, 16), "Close", mToolbarTop);
      mToolbarTop->addAction(mBackButton);
    }
  }

  if(withDeleteButton) {
    mToolbarBottom = new QToolBar();
    mToolbarBottom->setContentsMargins(0, 0, 0, 0);
    auto *spacerBottom = new QWidget();
    spacerBottom->setContentsMargins(0, 0, 0, 0);
    spacerBottom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mToolbarBottom->addWidget(spacerBottom);
    mDeleteButton = new QAction(QIcon(":/icons/outlined/icons8-trash-50.png").pixmap(16, 16), "Delete", mToolbarBottom);
    mDeleteButton->setToolTip("Delete");
    mToolbarBottom->addAction(mDeleteButton);
  }

  if(withTopToolbar) {
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

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
QAction *LayoutGenerator::addActionButton(const QString &text, const QString &icon)
{
  QIcon bmp(":/icons/outlined/" + icon);
  auto *action = new QAction(bmp.pixmap(16, 16), text);
  addItemToTopToolbar(action);
  return action;
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
  // group->setContentsMargins(0, 0, 0, 0);
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

}    // namespace joda::ui::helper
