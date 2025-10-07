
#pragma once

#include <qtableview.h>
#include <qtmetamacros.h>
#include <QtWidgets>

class PlaceholderTableView : public QTableView
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  using QTableView::QTableView;

  void setPlaceholderText(const QString &placeholdeText)
  {
    mPlaceholderText = placeholdeText;
  }

signals:
  /////////////////////////////////////////////////////
  void deletePressed();

private:
  /////////////////////////////////////////////////////
  void paintEvent(QPaintEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;

  QString mPlaceholderText = "No data available";
};
