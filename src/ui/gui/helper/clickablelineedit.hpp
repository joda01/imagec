///
/// \file      clickablelineedit.cpp
/// \author    Joachim Danmayr
/// \date      2024-10-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qlineedit.h>

namespace joda::ui::gui {
class ClickableLineEdit : public QLineEdit
{
  Q_OBJECT

public:
  ClickableLineEdit(QWidget *parent = nullptr);
  ~ClickableLineEdit()
  {
  }

signals:
  void mousePressedEvent();

protected:
  // Override the mousePressEvent method
  void mousePressEvent(QMouseEvent *e) override
  {
    QLineEdit::mousePressEvent(e);    // Call the base class implementation
    emit mousePressedEvent();
  }
};

}    // namespace joda::ui::gui
