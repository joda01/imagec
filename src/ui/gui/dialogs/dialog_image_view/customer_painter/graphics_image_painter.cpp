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

#include "graphics_image_painter.hpp"
#include <qpainter.h>

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void GraphicsImagePainter::setImageToPaint(QImage *img)
{
  std::lock_guard<std::mutex> lock(mPaintImage);
  mImageToPaint = img;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
QRectF GraphicsImagePainter::boundingRect() const
{
  std::lock_guard<std::mutex> lock(mPaintImage);
  if(mImageToPaint == nullptr || mImageToPaint->isNull() || mImageToPaint->width() == 0) {
    return QRectF(0, 0, 0, 0);
  }
  return QRectF(QPointF(0, 0), mImageToPaint->size());
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void GraphicsImagePainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  std::lock_guard<std::mutex> guard(mPaintImage);
  if(mImageToPaint != nullptr && !mImageToPaint->isNull()) {
    painter->drawImage(0, 0, *mImageToPaint);
  }
}
