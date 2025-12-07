//
/// \file      graphics_image_painter.cpp
/// \author    Joachim Danmayr
/// \date      2025-12-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "graphics_thumbnail.hpp"
#include <qgraphicssceneevent.h>
#include <qpainter.h>
#include <QStyle>
#include <string>

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
GraphicsThumbnail::GraphicsThumbnail(const joda::ome::TileToLoad *tile) : mTile(tile)
{
  setFlag(QGraphicsItem::ItemIgnoresTransformations, true);    // fixed size on screen
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::AllButtons);
  setAcceptHoverEvents(true);    // if you also want hover
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void GraphicsThumbnail::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
  //  setScale(1.5);    // enlarge when hovering
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void GraphicsThumbnail::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
  //  setScale(1.0);    // restore size
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void GraphicsThumbnail::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  QPointF p = event->pos();    // item-local coordinates

  for(auto &[rect, selected, x, y] : mTileRects) {
    if(rect.contains(p)) {
      selected = true;
      emit tileClicked(x, y);
      return;
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void GraphicsThumbnail::setImageToPaint(QImage *img, cv::Size originalImageSize)
{
  std::lock_guard<std::mutex> lock(mPaintImage);
  mImageToPaint      = img;
  mOriginalImageSize = originalImageSize;

  //
  // Calc Bounding rect
  //
  const QSizeF targetSize(128, 128);
  const QSize imgSize = mImageToPaint->size();
  mBoundingRect       = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, imgSize.scaled(targetSize.toSize(), Qt::KeepAspectRatio),
                                            QRectF(0, 0, targetSize.width(), targetSize.height()).toRect());

  //
  // Prepare tile rects
  //
  double nrOfTilesX = std::ceil(static_cast<double>(mOriginalImageSize.width) / static_cast<double>(mTile->tileWidth));
  double nrOfTilesY = std::ceil(static_cast<double>(mOriginalImageSize.height) / static_cast<double>(mTile->tileHeight));

  const auto tileRectWidthScaled  = mBoundingRect.width() / nrOfTilesX;
  const auto tileRectHeightScaled = mBoundingRect.height() / nrOfTilesY;
  mTileRects.clear();

  for(int y = 0; y < nrOfTilesY; y++) {
    for(int x = 0; x < nrOfTilesX; x++) {
      float xOffset = std::floor(static_cast<float>(x) * static_cast<float>(tileRectWidthScaled)) + static_cast<float>(mBoundingRect.x());
      float yOffset = std::floor(static_cast<float>(y) * static_cast<float>(tileRectHeightScaled)) + static_cast<float>(mBoundingRect.y());
      QRectF tileRect(QPoint(static_cast<int32_t>(xOffset), static_cast<int32_t>(yOffset)),
                      QSize(static_cast<int>(tileRectWidthScaled), static_cast<int>(tileRectHeightScaled)));

      if(tileRect.x() + tileRect.width() > mBoundingRect.x() + mBoundingRect.width()) {
        auto newWidthTmp = (tileRect.x() + tileRect.width()) - (mBoundingRect.x() + mBoundingRect.width());
        tileRect.setWidth(tileRect.width() - static_cast<int>(newWidthTmp));
      }
      if(tileRect.y() + tileRect.height() > mBoundingRect.y() + mBoundingRect.height()) {
        auto newHeightTmp = (tileRect.y() + tileRect.height()) - (mBoundingRect.y() + mBoundingRect.height());
        tileRect.setHeight(tileRect.height() - static_cast<int>(newHeightTmp));
      }
      if(x == mTile->tileX && y == mTile->tileY) {
        mTileRects.emplace_back(tileRect, true, x, y);
      } else {
        mTileRects.emplace_back(tileRect, false, x, y);
      }
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
QRectF GraphicsThumbnail::boundingRect() const
{
  std::lock_guard<std::mutex> lock(mPaintImage);
  if(mImageToPaint == nullptr || mImageToPaint->isNull()) {
    return {};
  }

  return mBoundingRect;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void GraphicsThumbnail::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  std::lock_guard<std::mutex> guard(mPaintImage);
  if(mImageToPaint == nullptr || mImageToPaint->isNull()) {
    return;
  }
  painter->drawImage(mBoundingRect, *mImageToPaint);

  //
  // Draw tile grid
  //
  painter->setPen(QColor(173, 216, 230));
  for(const auto &[rect, selected, x, y] : mTileRects) {
    if(selected) {
      painter->setBrush(QColor(173, 216, 230, 128));    // Set the brush to no brush for transparent fill
    } else {
      painter->setBrush(Qt::NoBrush);
    }
    painter->drawRect(rect);
  }
}
