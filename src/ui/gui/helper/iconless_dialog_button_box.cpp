///
/// \file      iconless_dialog_button_box.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "iconless_dialog_button_box.hpp"

IconlessDialogButtonBox::IconlessDialogButtonBox(StandardButtons buttons, Qt::Orientation orientation, QWidget *parent) :
    QDialogButtonBox(buttons, orientation, parent)
{
  removeIcons();
}
void IconlessDialogButtonBox::removeIcons()
{
  for(StandardButton btn : {Ok, Cancel, Apply, Save, Discard, Yes, No, Abort, Retry, Ignore, Close, Help, Reset}) {
    QPushButton *button = this->button(btn);
    if(button) {
      button->setIcon(QIcon());
    }
  }
}
