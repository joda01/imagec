///
/// \file      graphics_roi_overlay.cpp
/// \author    Joachim Danmayr
/// \date      2025-10-04
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "graphics_contour_overlay.hpp"
#include <qgraphicseffect.h>
#include <qpainter.h>
#include <qpen.h>
#include <cstddef>
#include <mutex>

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
ContourOverlay::ContourOverlay(QGraphicsItem *parent) : QGraphicsItem(parent)
{
  setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void ContourOverlay::refresh(ColorMap_t &&data, const cv::Size &previewSize)
{
  {
    std::lock_guard<std::mutex> lock(mPaintMutex);
    mData        = std::move(data);
    mPreviewSize = previewSize;
  }
  update();
}

///
/// \brief      Paint contours
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void ContourOverlay::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
  std::lock_guard<std::mutex> lock(mPaintMutex);
  for(const auto &[pen, elem] : mData) {
    painter->setPen(pen);
    painter->drawPolygon(elem);    // Single call for all polygons of this color
  }
}
