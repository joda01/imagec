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
#include <QtWidgets>
#include <algorithm>

class DroppableWidget : public QWidget
{
public:
  DroppableWidget(QWidget *parent = nullptr) : QWidget(parent)
  {
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);
    layout->setAlignment(Qt::AlignTop);

    setAcceptDrops(true);    // The layout area accepts drops
  }

  QVBoxLayout *getLayout()
  {
    return layout;
  }

protected:
  void dragEnterEvent(QDragEnterEvent *event) override
  {
    if(event->mimeData()->hasText()) {
      mInDrag = true;
      event->acceptProposedAction();
      update();    // Trigger repaint to show the indicator
    }
  }

  void dragMoveEvent(QDragMoveEvent *event) override
  {
    if(event->mimeData()->hasText()) {
      mInDrag = true;
      event->acceptProposedAction();
      currentDragPos = event->position().toPoint();    // Update currentDragPos HERE!
      update();                                        // Trigger repaint to update the indicator position
    }
  }

  void dragLeaveEvent(QDragLeaveEvent *event) override
  {
    mInDrag = false;
    update();    // Clear the indicator
    QWidget::dragLeaveEvent(event);
  }

  void dropEvent(QDropEvent *event) override
  {
    QWidget *droppedWidget = (QWidget *) event->mimeData()->text().toLongLong();
    if(droppedWidget == nullptr) {
      return;
    }

    int oldIndex = layout->indexOf(droppedWidget);
    if(oldIndex == -1) {
      return;
    }

    int dropIndex = findDropIndex(event->position().toPoint());

    layout->removeWidget(droppedWidget);
    layout->insertWidget(dropIndex, droppedWidget);

    event->acceptProposedAction();
    mInDrag = false;
    update();    // Clear the indicator
  }

private:
  void paintEvent(QPaintEvent *event) override
  {
    QWidget::paintEvent(event);

    if(mInDrag) {
      int dropIndex = findDropIndex(currentDragPos);

      int yPos = 0;
      if(layout->count() > 0) {
        if(dropIndex < layout->count()) {
          yPos = layout->itemAt(dropIndex)->geometry().top();
        } else {
          yPos = layout->itemAt(layout->count() - 1)->geometry().bottom();
        }
      }
      QPainter painter(this);
      QPen pen(Qt::red);
      pen.setWidth(2);
      painter.setPen(pen);
      painter.drawLine(0, yPos, width(), yPos);
    }
  }

  int findDropIndex(const QPoint &pos)
  {
    int y     = pos.y();
    int index = 0;
    for(int i = 0; i < layout->count(); ++i) {
      QWidget *widget = layout->itemAt(i)->widget();
      if(y > widget->y() + widget->height() / 2) {
        index = i + 1;
      }
    }
    return index;
  }
  bool mInDrag = false;
  QPoint currentDragPos;
  QVBoxLayout *layout;
};
