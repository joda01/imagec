
#pragma once

#include <QApplication>
#include <QComboBox>
#include <QPainter>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QWidget>

// Custom QComboBox to ensure selected item displays its background color
class ColorComboBox : public QComboBox
{
  Q_OBJECT
public:
  using QComboBox::QComboBox;
  ~ColorComboBox() override;

protected:
  void paintEvent(QPaintEvent *event) override;
};
