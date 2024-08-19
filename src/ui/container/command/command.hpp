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

#include <qboxlayout.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "../setting/setting_base.hpp"
#include "backend/commands/command.hpp"
#include "ui/helper/layout_generator.hpp"

namespace joda::ui::qt {

class Command : public QWidget
{
  Q_OBJECT
public:
  /////////////////////////////////////////////////////
  Command(QWidget *parent);

  void addSetting(const QString &title, const QString &icon, const std::vector<std::shared_ptr<SettingBase>> &settings);
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

signals:
  void valueChanged();

private:
  /////////////////////////////////////////////////////
  QWidget *mParent;
  QWidget mEditView;
  helper::LayoutGenerator mLayout;
  QGridLayout mDisplayViewLayout;
  QDialog mEditDialog;

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
};

}    // namespace joda::ui::qt
