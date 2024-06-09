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

#include <qlabel.h>
#include <qpushbutton.h>
#include "ui/container/container_function_base.hpp"

namespace joda::ui::qt {

class ContainerLabel : public ContainerFunctionBase
{
public:
  ContainerLabel(const QString &text, const QString &iconName, QWidget *parent)
  {
    mEditable = new QWidget();
    mEditable->setObjectName("panelFunction");
    mEditable->setStyleSheet("QWidget#panelFunction { background-color: rgba(0, 104, 117, 0);}");
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(8, 8, 8, 0);

    mLabel = new QLabel(text, mEditable);

    // if(!iconName.isEmpty()) {
    //   const QIcon icon(":/icons/outlined/" + iconName);
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

}    // namespace joda::ui::qt
