///
/// \file      command.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-18
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
#include <qtmetamacros.h>
#include <qwidget.h>
#include <memory>
#include <thread>
#include "../setting/setting_base.hpp"
#include "backend/commands/command.hpp"
#include "ui/helper/layout_generator.hpp"

namespace joda::ui {

using namespace std::chrono_literals;

class Command : public QWidget
{
  Q_OBJECT
public:
  /////////////////////////////////////////////////////
  Command(const QString &title, const QString &icon, QWidget *parent);

  helper::TabWidget *addTab(const QString &title, std::function<void()> beforeTabClose);

  void addSetting(const std::vector<std::pair<SettingBase *, bool>> &settings)
  {
    addSetting(addTab("", [] {}), "", settings);
  }

  void addSetting(helper::TabWidget *tab, const std::vector<std::pair<SettingBase *, bool>> &settings)
  {
    addSetting(tab, "", settings);
  }
  helper::VerticalPane *addSetting(helper::TabWidget *tab, const QString &boxTitle,
                                   const std::vector<std::pair<SettingBase *, bool>> &settings,
                                   helper::VerticalPane *col = nullptr);

  void removeSetting(const std::set<SettingBase *> &toRemove)
  {
    for(int m = mSettings.size() - 1; m >= 0; m--) {
      if(toRemove.contains(mSettings[m].first)) {
        mSettings.erase(mSettings.begin() + m);
      }
    }
  }

  auto getDisplayWidget() const -> const QWidget *
  {
    return this;
  }

  auto getEditWidget() const -> const QWidget *
  {
    return &mEditView;
  }

  void openEditView()
  {
    mEditDialog.show();
  }

  void adjustDialogSize()
  {
    mEditDialog.adjustSize();
  }

  void addSeparatorToTopToolbar()
  {
    mLayout.addSeparatorToTopToolbar();
  }
  QAction *addItemToTopToolbar(QWidget *widget)
  {
    return mLayout.addItemToTopToolbar(widget);
  }
  QAction *addActionButton(const QString &text, const QString &icon)
  {
    return mLayout.addActionButton(text, icon);
  }

signals:
  void valueChanged();

private:
  /////////////////////////////////////////////////////

  ///
  /// \brief      Constructor
  /// \author     Joachim Danmayr
  ///
  void mousePressEvent(QMouseEvent *event) override
  {
    if(event->button() == Qt::LeftButton) {
      openEditView();
    }
  }

  /////////////////////////////////////////////////////
  QWidget *mParent;
  QWidget mEditView;
  helper::LayoutGenerator mLayout;
  QGridLayout mDisplayViewLayout;
  QDialog mEditDialog;
  QLabel mDisplayableText;
  std::vector<std::pair<SettingBase *, bool>> mSettings;

protected slots:
  void updateDisplayText();
};

}    // namespace joda::ui
