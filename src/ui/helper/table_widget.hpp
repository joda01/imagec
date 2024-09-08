
#pragma once

#include <QtWidgets>

class PlaceholderTableWidget : public QTableWidget
{
public:
  using QTableWidget::QTableWidget;

  void setPlaceholderText(const QString &placeholdeText)
  {
    mPlaceholderText = placeholdeText;
  }

private:
  void paintEvent(QPaintEvent *event) override
  {
    QTableWidget::paintEvent(event);

    if(rowCount() == 0) {
      QPainter painter(viewport());
      painter.setPen(Qt::gray);
      painter.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, mPlaceholderText);
    }
  }

  QString mPlaceholderText = "No data available";
};
