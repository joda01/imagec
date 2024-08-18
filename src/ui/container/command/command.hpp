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

#include <qwidget.h>
#include "../setting/setting_base.hpp"
#include "backend/commands/command.hpp"
#include "ui/helper/layout_generator.hpp"

namespace joda::ui::qt {

class Command
{
public:
  /////////////////////////////////////////////////////
  Command() : mLayout(&mEditView, false), mDisplayViewLayout(&mDisplayView)
  {
    mDisplayView.setContentsMargins(0, 4, 4, 4);
    mDisplayViewLayout.setContentsMargins(0, 0, 0, 0);
    mDisplayView.setLayout(&mDisplayViewLayout);
    mDisplayViewLayout.setSpacing(4);
    mDisplayView.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  }

  void addSetting(std::vector<std::shared_ptr<SettingBase>> &settings)
  {
    auto *col1 = mLayout.addVerticalPanel();
    col1->addGroup("Settings", settings);

    int cnt = 0;
    for(const auto &setting : settings) {
      mDisplayViewLayout.addWidget(setting->getLabelWidget(), cnt / 2, cnt % 2);
      cnt++;
    }
  }

  auto getDisplayWidget() const -> const QWidget *
  {
    return &mDisplayView;
  }

  auto getEditWidget() const -> const QWidget *
  {
    return &mEditView;
  }

private:
  /////////////////////////////////////////////////////
  QWidget mEditView;
  helper::LayoutGenerator mLayout;
  QWidget mDisplayView;
  QGridLayout mDisplayViewLayout;
};

}    // namespace joda::ui::qt
