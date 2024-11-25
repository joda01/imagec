
#pragma once

#include <qstyleditemdelegate.h>
#include <qwidget.h>
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
  ColorComboBox(QWidget *parent = nullptr);
  ~ColorComboBox() override;
  void init();

protected:
  void paintEvent(QPaintEvent *event) override;
};
