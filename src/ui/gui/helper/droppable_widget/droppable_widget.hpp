///
/// \file      droppable_widget.hpp
/// \author    Joachim Danmayr
/// \date      2025-01-22
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qnamespace.h>
#include <qtmetamacros.h>
#include <QtWidgets>
#include <algorithm>
#include <iostream>
#include <string>

class DroppableWidget : public QWidget
{
  Q_OBJECT

public:
  DroppableWidget(QWidget *parent = nullptr);

  QVBoxLayout *getLayout()
  {
    return layout;
  }

signals:
  void dropFinished();

protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dragLeaveEvent(QDragLeaveEvent *event) override;
  void dropEvent(QDropEvent *event) override;

private:
  void paintEvent(QPaintEvent *event) override;
  int findDropIndex(const QPoint &pos);
  bool mInDrag = false;
  QPoint currentDragPos;
  QVBoxLayout *layout;
};
