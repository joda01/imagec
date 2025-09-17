#pragma once

#include <qbrush.h>
#include <qgraphicsitem.h>
#include <qnamespace.h>
#include <qpen.h>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class MyPolygonItem : public QGraphicsPolygonItem
{
public:
  using QGraphicsPolygonItem::QGraphicsPolygonItem;

  MyPolygonItem(const QPolygonF &polygon, const QPen &pen, const QBrush &brush, QGraphicsItem *parent = nullptr) :
      QGraphicsPolygonItem(polygon, parent)
  {
    setBrush(brush);
    setPen(pen);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  }

protected:
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override
  {
    Q_UNUSED(widget);

    if(option->state & QStyle::State_Selected) {
      QColor yellow = Qt::yellow;
      yellow.setAlpha(brush().color().alpha());
      painter->setBrush(yellow);
      painter->setPen(Qt::yellow);
    } else {
      painter->setBrush(brush());
      painter->setPen(pen());
    }
    painter->drawPolygon(polygon());
  }
};
