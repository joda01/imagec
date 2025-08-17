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

#pragma once

#include <qcombobox.h>
#include <qnamespace.h>
#include <qvariant.h>
#include <QtWidgets>
#include <set>

class QComboBoxMulti : public QComboBox
{
  Q_OBJECT

private:
  QString displayText;
  QStandardItemModel *model;

public:
  explicit QComboBoxMulti(QWidget *parent = nullptr);
  void addItem(const QIcon & /*aicon*/, const QString &atext, const QVariant &auserData)
  {
    int row    = model->rowCount();
    auto *item = new QStandardItem();
    item->setText(atext);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked, Qt::CheckStateRole);
    item->setData(auserData);
    model->setItem(row, 0, item);
    this->setModel(model);
  }

  void addItem(const QString &atext, const QVariant &auserData)
  {
    int row    = model->rowCount();
    auto *item = new QStandardItem();
    item->setText(atext);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked, Qt::CheckStateRole);
    item->setData(auserData);
    model->setItem(row, 0, item);
    this->setModel(model);
  }

  void clear()
  {
    model->clear();
    QComboBox::clear();
  }

  [[nodiscard]] std::vector<std::pair<QVariant, QString>> getCheckedItems() const
  {
    std::vector<std::pair<QVariant, QString>> checkedItems;

    for(int i = 0; i < model->rowCount(); i++) {
      if(model->item(i, 0)->checkState() == Qt::Checked) {
        QStandardItem *item = model->item(i, 0);
        checkedItems.emplace_back(item->data(), item->text());
      }
    }

    return checkedItems;
  }

  void setCheckedItems(const QVariantList &items)
  {
    for(int i = 0; i < count(); i++) {
      model->item(i)->setData(Qt::Unchecked, Qt::CheckStateRole);
    }

    for(int i = 0; i < items.count(); i++) {
      int index = findData(items.at(i), Qt::UserRole + 1);

      if(index != -1) {
        model->item(index)->setData(Qt::Checked, Qt::CheckStateRole);
      }
    }

    slotUpdate();
  }

  void checkAll()
  {
    for(int i = 0; i < count(); i++) {
      model->item(i)->setData(Qt::Checked, Qt::CheckStateRole);
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
