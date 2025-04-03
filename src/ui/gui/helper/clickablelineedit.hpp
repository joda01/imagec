///
/// \file      clickablelineedit.cpp
/// \author    Joachim Danmayr
/// \date      2024-10-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
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
