///
/// \file      layout_generator.hpp
/// \author
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <qaction.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qformlayout.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <qscrollbar.h>
#include <qtableview.h>
#include <qtablewidget.h>
#include <qtoolbar.h>
#include <qwidget.h>
#include <future>
#include <utility>
#include "ui/container/setting/setting_base.hpp"

namespace joda::ui::helper {

class TabWidget;

static constexpr int32_t SPACING   = 16;
static constexpr int32_t ICON_SIZE = 16;

///
/// \class      LayoutGenerator
/// \author     Joachim Danmayr
/// \brief
///
class LayoutGenerator : public QObject
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  explicit LayoutGenerator(QWidget *parent, bool withDeleteButton = true, bool withTopToolbar = true, bool withBackButton = true,
                           bool withBottomToolbar = false);

  TabWidget *addTab(const QString &title, std::function<void()> beforeTabClose);

  void removeTab(int idx)
  {
    mTabWidget->removeTab(idx);
  }

  void onRemoveTab(int idx)
  {
    onTabClosed(idx);
  }

  int32_t getNrOfTabs() const
  {
    return mTabWidget->count();
  }

  static void addSeparator(QFormLayout *formLayout)
  {
    auto *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    formLayout->addRow(separator);
  }

  static QLabel *createTitle(const QString &title)
  {
    auto *label = new QLabel();
    QFont font;
    font.setPixelSize(16);
    font.setBold(true);
    label->setFont(font);
    label->setText(title);
    return label;
  }

  QAction *getDeleteButton()
  {
    return mDeleteButton;
  }

  QAction *getBackButton()
  {
    return mBackButton;
  }

  void addSeparatorToTopToolbar();
  void addItemToBottomToolbar(QAction *widget);
  QAction *addItemToTopToolbar(QWidget *);
  void addItemToTopToolbar(QAction *widget);
  QAction *addActionButton(const QString &text, const QIcon &icon);
  QAction *addActionBottomButton(const QString &text, const QIcon &icon);

  QAction *addItemToBottomToolbar(QWidget *);

signals:
  void onSettingChanged();

private slots:
  void onTabClosed(int);

private:
  /////////////////////////////////////////////////////
  QToolBar *mToolbarTop        = nullptr;
  QToolBar *mToolbarBottom     = nullptr;
  QWidget *mParent             = nullptr;
  QAction *mSpaceTopToolbar    = nullptr;
  QAction *mSpaceBottomToolbar = nullptr;
  QAction *mBackButton         = nullptr;
  QAction *mDeleteButton       = nullptr;
  QTabWidget *mTabWidget;
};

///
/// \class      Vertical pane
/// \author     Joachim Danmayr
/// \brief
///
class VerticalPane : public QVBoxLayout
{
public:
  explicit VerticalPane(QWidget *parent, LayoutGenerator *generator) : layoutGenerator(generator), mParent(parent)
  {
  }
  void addGroup(const std::vector<SettingBase *> &elements, int minWidth = 220, int maxWidth = 220);
  void addGroup(const QString &title, const std::vector<SettingBase *> &elements, int minWidth = 220, int maxWidth = 220);
  void addWidgetGroup(const QString &title, const std::vector<QWidget *> &elements, int minWidth = 220, int maxWidth = 220);

private:
  LayoutGenerator *layoutGenerator;
  QWidget *mParent;
};

///
/// \class      Tab Pane
/// \author     Joachim Danmayr
/// \brief
///
class TabWidget : public QScrollArea
{
  Q_OBJECT

public:
  TabWidget(bool hasBottomToolbar, std::function<void()> beforeTabClose, LayoutGenerator *layoutGenerator, QWidget *parent);
  VerticalPane *addVerticalPanel();
  void beforeClose()
  {
    beforeTabClose();
  }

private:
  /////////////////////////////////////////////////////
  QHBoxLayout *mainLayout;
  LayoutGenerator *mLayoutGenerator;
  QWidget *mParent;
  std::function<void()> beforeTabClose;
};

}    // namespace joda::ui::helper
