///
/// \file      container_function_base.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <QtWidgets>

namespace joda::ui::qt {

class ContainerFunctionBase : public QWidget
{
  Q_OBJECT

public:
  ContainerFunctionBase();
  ~ContainerFunctionBase()
  {
  }

  virtual QWidget *getEditableWidget() = 0;

  static QWidget *createDisplayAbleWidgetPlaceholder()
  {
    QWidget *displayable = new QWidget();

    // Create a QLabel
    QLabel *displayLabelIcon = new QLabel();
    QLabel *displayLabel     = new QLabel();

    // Set text for the label
    displayLabel->setText("");
    displayLabel->setToolTip("");

    // Create a QPixmap for the icon (you may need to adjust the path)
    QIcon bmp(":/icons/outlined/");

    // Set the icon for the label
    displayLabelIcon->setPixmap(bmp.pixmap(16, 16));    // You can adjust the size of the icon as needed
    displayLabelIcon->setToolTip("");

    // Create a QHBoxLayout to arrange the text and icon horizontally
    QHBoxLayout *layout = new QHBoxLayout;
    displayable->setLayout(layout);
    layout->addWidget(displayLabelIcon);
    layout->addWidget(displayLabel);
    layout->addStretch();
    return displayable;
  }

signals:
  void valueChanged();

protected:
  void triggerValueChanged();
};

}    // namespace joda::ui::qt
