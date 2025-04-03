///
/// \file      container_function.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-23
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
/// \brief     This container holds the settings for one function
///            It can either be just displayed or edited
///

#pragma once

#include <qpushbutton.h>
#include "ui/gui/container/setting/setting_base.hpp"

namespace joda::ui::gui {

class ContainerButton : public SettingBase
{
public:
  ContainerButton(const QString &text, const QIcon &icon, QWidget *parent) : SettingBase(parent, icon, text)
  {
    mEditable = new QWidget();
    mEditable->setObjectName("panelFunction");
    QVBoxLayout *layout = new QVBoxLayout();
    //     layout->setContentsMargins(8, 8, 8, 0);

    mButton = new QPushButton(text, mEditable);

    if(!icon.isNull()) {
      //      mButton->setIconSize({16, 16});
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
    triggerValueChanged("", true);
  }

private:
  /////////////////////////////////////////////////////
  QWidget *mEditable;
  QPushButton *mButton;
};

}    // namespace joda::ui::gui
