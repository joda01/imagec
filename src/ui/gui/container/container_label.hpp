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

#include <qlabel.h>
#include <qpushbutton.h>
#include "ui/gui/container/setting/setting_base.hpp"

namespace joda::ui::gui {

class ContainerLabel : public SettingBase
{
public:
  ContainerLabel(const QString &text, const QIcon &iconName, QWidget *parent) : SettingBase(parent, iconName, text)
  {
    mEditable = new QWidget();
    mEditable->setObjectName("panelFunction");
    QVBoxLayout *layout = new QVBoxLayout();
    //     layout->setContentsMargins(8, 8, 8, 0);

    mLabel = new QLabel(text, mEditable);

    // if(!iconName.isEmpty()) {
    //   const QIcon icon(":/icons/icons/" + iconName);
    //   mLabel->setIconSize({16, 16});
    //   mLabel->setIcon(icon);
    // }

    layout->addWidget(mLabel);

    mEditable->setLayout(layout);
  }

  QWidget *getEditableWidget()
  {
    return mEditable;
  }

  void setText(const QString &txt)
  {
    mLabel->setText(txt);
  }

private:
  /////////////////////////////////////////////////////
  QWidget *mEditable;
  QLabel *mLabel;
};

}    // namespace joda::ui::gui
