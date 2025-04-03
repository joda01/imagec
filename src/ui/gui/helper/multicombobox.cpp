///
/// \file      multicombobox.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-22
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "multicombobox.hpp"

QComboBoxMulti::QComboBoxMulti(QWidget *parent) : QComboBox(parent)
{
  setEditable(true);

  displayText = "";
  model       = new QStandardItemModel;
  slotUpdateText();
  lineEdit()->setReadOnly(true);

  connect(model, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(slotUpdate()));
}
