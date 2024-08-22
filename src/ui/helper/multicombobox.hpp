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

#pragma once

#include <qvariant.h>
#include <QtWidgets>

class QComboBoxMulti : public QComboBox
{
  Q_OBJECT

private:
  QString displayText;
  QStandardItemModel *model;

public:
  QComboBoxMulti(QWidget *parent = 0);
  void addItem(const QIcon &aicon, const QString &atext, const QVariant &auserData)
  {
    int row             = model->rowCount();
    QStandardItem *item = new QStandardItem();
    item->setText(atext);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked, Qt::CheckStateRole);
    item->setData(auserData);
    model->setItem(row, 0, item);
    this->setModel(model);
  }

  QVariantList getCheckedItems() const
  {
    QVariantList checkedItems;

    for(int i = 0; i < model->rowCount(); i++) {
      if(model->item(i, 0)->checkState() == Qt::Checked) {
        checkedItems << model->item(i, 0)->data();
      }
    }

    return checkedItems;
  }

  void setCheckedItems(const QVariantList &items)
  {
    for(int i = 0; i < items.count(); i++) {
      int index = findData(items.at(i), Qt::UserRole + 1);

      if(index != -1) {
        model->item(index)->setData(Qt::Checked, Qt::CheckStateRole);
      }
    }

    slotUpdate();
  }

  const QString &getDisplayText()
  {
    return displayText;
  }

public slots:
  void slotUpdateText()
  {
    lineEdit()->setText(displayText);
  }

  void slotUpdate()
  {
    displayText = "";

    for(int i = 0; i < model->rowCount(); i++) {
      if(model->item(i, 0)->checkState() == Qt::Checked) {
        displayText = displayText + model->item(i, 0)->text() + ", ";
      }
    }
    displayText.chop(2);

    QTimer::singleShot(0, this, SLOT(slotUpdateText()));
  }
};
