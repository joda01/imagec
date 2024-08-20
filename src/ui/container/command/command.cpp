///
/// \file      command.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "command.hpp"
#include <algorithm>

namespace joda::ui {

Command::Command(QWidget *parent) :
    mParent(parent), mLayout(&mEditView, false, false), mDisplayViewLayout(this), mEditDialog(parent)
{
  setContentsMargins(0, 4, 4, 4);
  mDisplayViewLayout.setContentsMargins(0, 0, 0, 0);
  setLayout(&mDisplayViewLayout);
  mDisplayViewLayout.setSpacing(4);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void Command::addSetting(const QString &title, const QString &icon,
                         const std::map<std::shared_ptr<SettingBase>, bool> &settings)
{
  auto convert = [&]() {
    std::vector<std::shared_ptr<SettingBase>> vec;
    std::transform(settings.begin(), settings.end(), std::back_inserter(vec), [](auto &kv) { return kv.first; });
    return vec;
  };

  auto *col1 = mLayout.addVerticalPanel();
  col1->addGroup(convert(), 800);

  int cnt = 2;
  for(const auto &[setting, show] : settings) {
    if(show) {
      mDisplayViewLayout.addWidget(setting->getLabelWidget(), cnt / 2, cnt % 2);
    }
    connect(setting.get(), &SettingBase::valueChanged, this, &Command::valueChanged);
    cnt++;
  }
  // It must be in a separate line
  if(cnt % 2 == 0) {
    cnt++;
  }
  QFont font;
  font.setPixelSize(12);
  font.setItalic(true);
  font.setBold(false);
  font.setWeight(QFont::Light);
  auto *label = new QLabel(title);
  label->setObjectName("functionHelperText");
  label->setFont(font);
  label->setStyleSheet("QLabel#functionHelperText { color : #808080; }");
  mDisplayViewLayout.addWidget(label, cnt, 0, 1, 2);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  // Prepare edit dialog
  auto *layout = new QVBoxLayout();
  layout->addWidget(&mEditView);
  mEditDialog.setModal(false);
  mEditDialog.setLayout(layout);
  mEditDialog.setMinimumWidth(300);
  mEditDialog.setMaximumWidth(400);
  mEditDialog.setWindowTitle(title);
}

}    // namespace joda::ui
