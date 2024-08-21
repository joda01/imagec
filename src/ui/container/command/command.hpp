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
#include "../setting/setting_base.hpp"
#include "backend/commands/command.hpp"
#include "ui/helper/layout_generator.hpp"

namespace joda::ui {

class Command : public QWidget
{
  Q_OBJECT
public:
  /////////////////////////////////////////////////////
  Command(QWidget *parent);

  void addSetting(const QString &description, const QString &icon,
                  const std::vector<std::pair<std::shared_ptr<SettingBase>, bool>> &settings)
  {
    addSetting("", settings);
    addFooter(description, icon);
  }
  helper::LayoutGenerator::VerticalPane *
  addSetting(const QString &boxTitle, const std::vector<std::pair<std::shared_ptr<SettingBase>, bool>> &settings,
             helper::LayoutGenerator::VerticalPane *col = nullptr);
  void addFooter(const QString &title, const QString &icon);

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
  std::vector<std::pair<std::shared_ptr<SettingBase>, bool>> mSettings;

protected slots:
  void updateDisplayText();
};

}    // namespace joda::ui
