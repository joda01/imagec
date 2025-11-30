#include "droppable_widget.hpp"

DroppableWidget::DroppableWidget(QWidget *parent) : QWidget(parent)
{
  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);
  layout->setAlignment(Qt::AlignTop);

  setAcceptDrops(true);    // The layout area accepts drops
}
void DroppableWidget::dragEnterEvent(QDragEnterEvent *event)
{
  if(event->mimeData()->hasText()) {
    mInDrag = true;
    event->acceptProposedAction();
    update();    // Trigger repaint to show the indicator
  }
}
void DroppableWidget::dragMoveEvent(QDragMoveEvent *event)
{
  if(event->mimeData()->hasText()) {
    mInDrag = true;
    event->acceptProposedAction();
    currentDragPos = event->position().toPoint();    // Update currentDragPos HERE!
    update();                                        // Trigger repaint to update the indicator position
  }
}
void DroppableWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
  mInDrag = false;
  update();    // Clear the indicator
  QWidget::dragLeaveEvent(event);
}
void DroppableWidget::dropEvent(QDropEvent *event)
{
  QWidget *droppedWidget = reinterpret_cast<QWidget *>(event->mimeData()->text().toLongLong());
  if(droppedWidget == nullptr) {
    mInDrag = false;
    update();
    return;
  }

  int oldIndex = layout->indexOf(droppedWidget);
  if(oldIndex == -1) {
    mInDrag = false;
    update();
    return;
  }

  int dropIndex = findDropIndex(event->position().toPoint());
  if(dropIndex >= layout->count()) {
    mInDrag = false;
    update();
    return;
  }

  layout->removeWidget(droppedWidget);
  layout->insertWidget(dropIndex, droppedWidget);

  event->acceptProposedAction();
  mInDrag = false;
  update();
  emit dropFinished();
}
void DroppableWidget::paintEvent(QPaintEvent *event)
{
  QWidget::paintEvent(event);

  if(mInDrag) {
    int dropIndex = findDropIndex(currentDragPos);
    if(dropIndex >= layout->count()) {
      return;
    }

    int yPos       = 0;
    int itemHeight = 0;
    if(layout->count() > 0) {
      if(dropIndex < layout->count()) {
        yPos       = layout->itemAt(dropIndex)->geometry().top();
        itemHeight = layout->itemAt(dropIndex)->geometry().height();

      } else {
        yPos = layout->itemAt(layout->count() - 1)->geometry().bottom();
      }
    }
    QPainter painter(this);
    QPen pen(Qt::darkGray);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawRect(0, yPos, width(), itemHeight);
  }
}
int DroppableWidget::findDropIndex(const QPoint &pos)
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
