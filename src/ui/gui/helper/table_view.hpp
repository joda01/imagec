
#pragma once

#include <qtableview.h>
#include <QtWidgets>

class PlaceholderTableView : public QTableView
{
public:
  using QTableView::QTableView;

  void setPlaceholderText(const QString &placeholdeText)
  {
    mPlaceholderText = placeholdeText;
  }

private:
  void paintEvent(QPaintEvent *event) override
  {
    QTableView::paintEvent(event);

    if(model()->rowCount() == 0) {
      QPainter painter(viewport());
      painter.setPen(Qt::gray);
      painter.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, mPlaceholderText);
    }
  }

  QString mPlaceholderText = "No data available";
};
