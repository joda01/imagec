///
/// \file      multicombobox.hpp
/// \author    https://notes84.blogspot.com/2016/05/c-qt56-combobox-with-checkboxes.html
/// \date      2024-08-22
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
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
