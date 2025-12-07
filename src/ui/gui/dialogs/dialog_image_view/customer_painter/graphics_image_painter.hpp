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

#include <qgraphicsitem.h>
#include <qtmetamacros.h>
#include <mutex>

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class GraphicsImagePainter : public QGraphicsItem
{
public:
  /////////////////////////////////////////////////////
  GraphicsImagePainter() = default;
  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
  void setImageToPaint(QImage *);

private:
  /////////////////////////////////////////////////////
  QImage *mImageToPaint = nullptr;
  mutable std::mutex mPaintImage;
};
