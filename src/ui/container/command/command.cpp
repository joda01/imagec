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
#include <qlabel.h>
#include <algorithm>
#include <string>

namespace joda::ui {

Command::Command(QWidget *parent) :
    mParent(parent), mLayout(&mEditView, true, true, false), mDisplayViewLayout(this), mEditDialog(parent)
{
  setContentsMargins(0, 4, 4, 4);
  mDisplayViewLayout.setContentsMargins(0, 0, 0, 0);
  setLayout(&mDisplayViewLayout);
  mDisplayViewLayout.setSpacing(4);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

helper::LayoutGenerator::VerticalPane *
Command::addSetting(const QString &boxTitle, const std::vector<std::pair<std::shared_ptr<SettingBase>, bool>> &settings,
                    helper::LayoutGenerator::VerticalPane *col)
{
  auto containsPtr = [&](std::shared_ptr<SettingBase> toCheck) {
    for(const auto &[ptr, _] : mSettings) {
      if(ptr.get() == toCheck.get()) {
        return true;
      }
    }
    return false;
  };

  for(const auto &data : settings) {
    if(!containsPtr(data.first)) {
      mSettings.emplace_back(data);
    }
  }
  auto convert = [&]() {
    std::vector<std::shared_ptr<SettingBase>> vec;
    std::transform(settings.begin(), settings.end(), std::back_inserter(vec), [](auto &kv) { return kv.first; });
    return vec;
  };

  if(nullptr == col) {
    col = mLayout.addVerticalPanel();
  }
  if(boxTitle.isEmpty()) {
    col->addGroup(convert(), 800);
  } else {
    col->addGroup(boxTitle, convert(), 800);
  }
  QString txt;
  for(const auto &[setting, show] : settings) {
    if(show) {
      txt = txt + setting->getLabelText() + ", ";
      setting->setDisplayIconVisible(false);
    }
    connect(setting.get(), &SettingBase::valueChanged, this, &Command::valueChanged);
  }
  txt.chop(2);
  mDisplayableText.setText(txt);

  connect(this, &Command::valueChanged, this, &Command::updateDisplayText);
  return col;
}

void Command::updateDisplayText()
{
  QString txt;
  for(const auto &[setting, show] : mSettings) {
    if(show) {
      txt = txt + setting->getLabelText() + ", ";
      setting->setDisplayIconVisible(false);
    }
  }
  txt.chop(2);
  mDisplayableText.setText(txt);
}

void Command::addFooter(const QString &title, const QString &icon)
{
  // Header
  {
    // Create a QHBoxLayout to arrange the text and icon horizontally
    auto *headerWidget = new QWidget();
    headerWidget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);
    headerWidget->setLayout(layout);
    auto *mDisplayLabelIcon = new QLabel();
    if(!icon.isEmpty()) {
      QIcon bmp(":/icons/outlined/" + icon);
      mDisplayLabelIcon->setPixmap(bmp.pixmap(16, 16));    // You can adjust the size of the icon as needed
      layout->addWidget(mDisplayLabelIcon);
    }
    // layout->addWidget(new QLabel(title));
    // layout->addStretch();
    headerWidget->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
    mDisplayViewLayout.addWidget(headerWidget, 0, 0);
  }
  // Content
  {
    mDisplayableText.setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
    mDisplayableText.setWordWrap(true);
    mDisplayViewLayout.addWidget(&mDisplayableText, 0, 1);
  }

  // Footer
  {
    QFont font;
    font.setPixelSize(12);
    font.setItalic(true);
    font.setBold(false);
    font.setWeight(QFont::Light);
    auto *label = new QLabel(title);
    label->setObjectName("functionHelperText");
    label->setFont(font);
    label->setStyleSheet("QLabel#functionHelperText { color : #808080; }");
    mDisplayViewLayout.addWidget(label, 1, 0, 1, 2);
  }

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  // Prepare edit dialog
  mEditView.setContentsMargins(0, 0, 0, 0);
  auto *layout = new QVBoxLayout();
  layout->addWidget(&mEditView);
  layout->setContentsMargins(0, 0, 0, 0);
  mEditDialog.setModal(false);
  mEditDialog.setLayout(layout);
  mEditDialog.setMinimumWidth(300);
  // mEditDialog.setMaximumWidth(400);
  mEditDialog.setWindowTitle(title);
}

}    // namespace joda::ui
