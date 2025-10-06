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
void ContourOverlay::refresh(const ColorMap_t *data, const cv::Size &previewSize)
{
  mData        = data;
  mPreviewSize = previewSize;

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
  return;
  if(mData == nullptr) {
    return;
  }
  for(const auto &[_, elem] : *mData) {
    // Set pen (can use the caching strategy from #2 here too)
    QPen pen(elem.first, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->drawPath(elem.second);    // Single call for all polygons of this color
  }
}
