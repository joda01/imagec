///
/// \file      graphics_image_painter.hpp
/// \author    Joachim Danmayr
/// \date      2025-12-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qevent.h>
#include <qgraphicsitem.h>
#include <qtmetamacros.h>
#include <mutex>
#include "backend/helper/image/image.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class GraphicsThumbnail : public QGraphicsObject
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  GraphicsThumbnail(const joda::ome::TileToLoad *tile);
  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
  void setImageToPaint(QImage *, cv::Size originalImageSize);
  void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

signals:
  void tileClicked(int x, int y);

private:
  /////////////////////////////////////////////////////
  QImage *mImageToPaint = nullptr;
  const joda::ome::TileToLoad *mTile;
  cv::Size mOriginalImageSize;
  std::vector<std::tuple<QRectF, bool, int, int>> mTileRects;
  QRectF mBoundingRect;

  mutable std::mutex mPaintImage;
};
