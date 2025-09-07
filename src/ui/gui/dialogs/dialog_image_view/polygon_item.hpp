#include <QGraphicsItem>
#include <QPainter>
#include <vector>

class PolygonsItem : public QGraphicsItem
{
  std::vector<std::vector<QPoint>> polygons;
  qreal penWidth = 1.0;

public:
  PolygonsItem(const std::vector<std::vector<QPoint>> &polys) : polygons(polys)
  {
  }

  QRectF boundingRect() const override
  {
    if(polygons.empty())
      return QRectF();

    int minX = polygons[0][0].x();
    int minY = polygons[0][0].y();
    int maxX = polygons[0][0].x();
    int maxY = polygons[0][0].y();

    for(const auto &poly : polygons) {
      for(const auto &pt : poly) {
        minX = std::min(minX, pt.x());
        minY = std::min(minY, pt.y());
        maxX = std::max(maxX, pt.x());
        maxY = std::max(maxY, pt.y());
      }
    }

    return QRectF(minX - penWidth, minY - penWidth, maxX - minX + 2 * penWidth, maxY - minY + 2 * penWidth);
  }

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override
  {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::blue, penWidth));
    painter->setBrush(QBrush(Qt::red, Qt::SolidPattern));

    for(const auto &poly : polygons) {
      if(!poly.empty()) {
        QPolygon qpoly;
        for(const auto &pt : poly) {
          qpoly << pt;
        }
        painter->drawPolygon(qpoly);
      }
    }
  }
};
