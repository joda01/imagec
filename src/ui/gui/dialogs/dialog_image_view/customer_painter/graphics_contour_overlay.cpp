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
void ContourOverlay::refresh(const std::vector<std::pair<QColor, std::vector<QPointF>>> *data, const cv::Size &previewSize)
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
  std::cout << "Paint" << std::endl;
  if(mData == nullptr) {
    return;
  }
  // Set the painter to full opacity for the contour overlay
  painter->setOpacity(1.0);

  // Iterate through all prepared contour segments
  for(const auto &[col, points] : *mData) {
    QPen pen(col, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    pen.setCosmetic(true);
    painter->setPen(pen);
    if(!points.empty()) {
      painter->drawPolygon(points.data(), static_cast<int32_t>(points.size()));
    }
  }
}
