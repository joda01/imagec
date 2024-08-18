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
#include <qwidget.h>
#include "../setting/setting_base.hpp"
#include "backend/commands/command.hpp"
#include "ui/helper/layout_generator.hpp"

namespace joda::ui::qt {

class Command : public QWidget
{
public:
  /////////////////////////////////////////////////////
  Command(QWidget *parent) :
      mParent(parent), mLayout(&mEditView, false, false), mDisplayViewLayout(this), mEditDialog(parent)
  {
    setContentsMargins(0, 4, 4, 4);
    mDisplayViewLayout.setContentsMargins(0, 0, 0, 0);
    setLayout(&mDisplayViewLayout);
    mDisplayViewLayout.setSpacing(4);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  }

  void addSetting(const QString &title, const std::vector<std::shared_ptr<SettingBase>> &settings)
  {
    auto *col1 = mLayout.addVerticalPanel();
    col1->addGroup(settings, 800);

    int cnt = 2;
    for(const auto &setting : settings) {
      mDisplayViewLayout.addWidget(setting->getLabelWidget(), cnt / 2, cnt % 2);
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
    mEditDialog.setMinimumWidth(250);
    mEditDialog.setMaximumWidth(250);
    mEditDialog.setWindowTitle(title);
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
