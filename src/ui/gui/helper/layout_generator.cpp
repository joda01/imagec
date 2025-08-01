///
/// \file      layout_generator.cpp
/// \author
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.

#include "layout_generator.hpp"
#include <qtabwidget.h>
#include <qwidget.h>

#include <utility>
#include "ui/gui/helper/icon_generator.hpp"

namespace joda::ui::gui::helper {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
LayoutGenerator::LayoutGenerator(QWidget *parent, bool withDeleteButton, bool withTopToolbar, bool withBackButton, bool withBottomToolbar,
                                 QMainWindow *toolbarParent) :
    mParent(parent),
    mToolbarWindow(toolbarParent)
{
  auto *container = new QVBoxLayout(parent);
  container->setContentsMargins(2, 0, 2, 0);
  container->setSpacing(0);

  if(withTopToolbar) {
    mToolbarTop = new QToolBar();
    if(toolbarParent == nullptr) {
      mToolbarTop->setContentsMargins(0, 0, 0, 0);
      mToolbarTop->setStyleSheet("QToolBar { border-bottom: 1px solid rgb(170, 170, 170); }");
    }
    if(withBackButton) {
      auto *spacerTop = new QWidget();
      spacerTop->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
      mSpaceTopToolbar = mToolbarTop->addWidget(spacerTop);
      mBackButton      = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("x"), "Close", mToolbarTop);
      mToolbarTop->addAction(mBackButton);
    }
  }

  if(withBottomToolbar || withDeleteButton) {
    mToolbarBottom = new QToolBar();
    mToolbarBottom->setContentsMargins(0, 0, 0, 0);
    if(withDeleteButton) {
      auto *spacerBottom = new QWidget();
      spacerBottom->setContentsMargins(0, 0, 0, 0);
      spacerBottom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
      mSpaceBottomToolbar = mToolbarBottom->addWidget(spacerBottom);
      mDeleteButton       = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("trash-simple"), "Delete", mToolbarBottom);
      mDeleteButton->setToolTip("Delete");
      mToolbarBottom->addAction(mDeleteButton);
    }
  }

  if(withTopToolbar) {
    if(toolbarParent == nullptr) {
      container->addWidget(mToolbarTop);
    } else {
      mToolbarTop->setVisible(false);
      toolbarParent->addToolBar(mToolbarTop);
    }
  }
  mTabWidget = new QTabWidget();
  mTabWidget->setTabsClosable(true);
  mTabWidget->setTabPosition(QTabWidget::North);
  mTabWidget->setTabBarAutoHide(true);
  mTabWidget->setStyleSheet("QTabWidget::pane { border: none; }");
  container->addWidget(mTabWidget);
  if(withBottomToolbar || withDeleteButton) {
    container->addWidget(mToolbarBottom);
  }

  parent->setLayout(container);
  connect(mTabWidget, &QTabWidget::tabCloseRequested, this, &LayoutGenerator::onTabClosed);
}
///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
TabWidget *LayoutGenerator::addTab(const QString &title, std::function<void()> beforeTabClose, bool showCloseButton, int32_t topContentSpacing)
{
  auto *tab = new TabWidget(topContentSpacing, mDeleteButton != nullptr, std::move(beforeTabClose), this, mParent);
  mTabWidget->addTab(tab, title);
  if(!showCloseButton) {
    mTabWidget->tabBar()->tabButton(mTabWidget->count() - 1, QTabBar::RightSide)->setVisible(false);
  }
  // mTabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);

  return tab;
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
QAction *LayoutGenerator::addItemToBottomToolbar(QWidget *widget)
{
  return mToolbarBottom->insertWidget(mSpaceBottomToolbar, widget);
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
void LayoutGenerator::addItemToBottomToolbar(QAction *widget)
{
  mToolbarBottom->insertAction(mSpaceTopToolbar, widget);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
QAction *LayoutGenerator::addActionBottomButton(const QString &text, const QIcon &icon)
{
  auto *action = new QAction(icon, text);
  addItemToBottomToolbar(action);
  return action;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
QAction *LayoutGenerator::addActionButton(const QString &text, const QIcon &icon)
{
  auto *action = new QAction(icon, text);
  addItemToTopToolbar(action);
  return action;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void VerticalPane::addGroup(const QString &title, const std::vector<SettingBase *> &elements, int minWidth, int maxWidth)
{
  auto *group = new QGroupBox(title);
  group->setMaximumWidth(maxWidth);
  group->setMinimumWidth(minWidth);
  auto *layout = new QVBoxLayout;
  for(const auto &element : elements) {
    layout->addWidget(element->getEditableWidget());
    connect(element, &SettingBase::valueChanged, layoutGenerator, &LayoutGenerator::onSettingChanged);
  }

  group->setLayout(layout);
  addWidget(group);
}

void VerticalPane::addGroup(const std::vector<SettingBase *> &elements, int minWidth, int maxWidth)
{
  auto *group = new QWidget();
  group->setMaximumWidth(maxWidth);
  group->setMinimumWidth(minWidth);
  auto *layout = new QVBoxLayout;
  group->setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(0, 0, 0, 0);

  for(const auto &element : elements) {
    layout->addWidget(element->getEditableWidget());
    connect(element, &SettingBase::valueChanged, layoutGenerator, &LayoutGenerator::onSettingChanged);
  }

  group->setLayout(layout);
  addWidget(group);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void VerticalPane ::addWidgetGroup(const QString &title, const std::vector<QWidget *> &elements, int minWidth, int maxWidth)
{
  auto *group = new QGroupBox(title);
  group->setMaximumWidth(maxWidth);
  group->setMinimumWidth(minWidth);
  auto *layout = new QVBoxLayout;
  for(const auto &element : elements) {
    element->setParent(group);
    layout->addWidget(element);
  }
  group->setLayout(layout);
  addWidget(group);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void VerticalPane ::addWidgetGroup(const std::vector<QWidget *> &elements, int minWidth, int maxWidth)
{
  auto *group = new QWidget();
  group->setMaximumWidth(maxWidth);
  group->setMinimumWidth(minWidth);
  auto *layout = new QVBoxLayout;
  for(const auto &element : elements) {
    element->setParent(group);
    layout->addWidget(element);
  }
  group->setLayout(layout);
  addWidget(group);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
TabWidget::TabWidget(int32_t topContentSpacing, bool hasBottomToolbar, std::function<void()> beforeTabClose, LayoutGenerator *layoutGenerator,
                     QWidget *parent) :
    beforeTabClose(std::move(beforeTabClose)),
    mLayoutGenerator(layoutGenerator), mParent(parent)
{
  setObjectName("scrollArea");
  setFrameStyle(0);
  setContentsMargins(0, 0, 0, 0);
  verticalScrollBar()->setObjectName("scrollAreaV");
  // setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

  // Create a widget to hold the panels
  mContentWidget = new QWidget;
  mContentWidget->setContentsMargins(0, topContentSpacing, 0, 0);
  mContentWidget->setObjectName("contentOverview");

  // Create a horizontal layout for the panels
  mainLayout = new QHBoxLayout(mContentWidget);
  mainLayout->setSpacing(SPACING);    // Adjust this value as needed
  mainLayout->setAlignment(Qt::AlignLeft);
  mContentWidget->setLayout(mainLayout);

  if(hasBottomToolbar) {
    mainLayout->setContentsMargins(topContentSpacing, 0, topContentSpacing, 0);
  } else {
    mainLayout->setContentsMargins(topContentSpacing, 0, topContentSpacing, topContentSpacing);
  }
  mContentWidget->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
  setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
  setWidget(mContentWidget);
  setWidgetResizable(true);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void LayoutGenerator::onTabClosed(int idx)
{
  ((TabWidget *) mTabWidget->widget(idx))->beforeClose();
  removeTab(idx);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
VerticalPane *TabWidget::addVerticalPanel()
{
  auto *vboxLayout = new VerticalPane(mParent, mLayoutGenerator);
  vboxLayout->setContentsMargins(0, 0, 0, 0);
  vboxLayout->setAlignment(Qt::AlignTop);
  mainLayout->addLayout(vboxLayout, 1);
  return vboxLayout;
}

}    // namespace joda::ui::gui::helper
