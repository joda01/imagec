///
/// \file      container_function.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-23
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     This container holds the settings for one function
///            It can either be just displayed or edited
///

#pragma once

#include <qpushbutton.h>
#include "ui/container/setting/setting_base.hpp"

namespace joda::ui {

class ContainerButton : public SettingBase
{
public:
  ContainerButton(const QString &text, const QString &iconName, QWidget *parent) : SettingBase(parent, iconName, text)
  {
    mEditable = new QWidget();
    mEditable->setObjectName("panelFunction");
    QVBoxLayout *layout = new QVBoxLayout();
    //     layout->setContentsMargins(8, 8, 8, 0);

    mButton = new QPushButton(text, mEditable);

    if(!iconName.isEmpty()) {
      const QIcon icon(":/icons/icons/" + iconName);
      mButton->setIconSize({16, 16});
      mButton->setIcon(icon);
    }

    layout->addWidget(mButton);

    connect(mButton, &QPushButton::pressed, this, &ContainerButton::onButtonPressed);
    mEditable->setLayout(layout);
  }

  QWidget *getEditableWidget()
  {
    return mEditable;
  }

private slots:
  void onButtonPressed()
  {
    /////////////////////////////////////////////////////
    triggerValueChanged("");
  }

private:
  /////////////////////////////////////////////////////
  QWidget *mEditable;
  QPushButton *mButton;
};

}    // namespace joda::ui
