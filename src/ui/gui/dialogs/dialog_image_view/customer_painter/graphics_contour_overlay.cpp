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
ContourOverlay::ContourOverlay(const joda::atom::ObjectMap *objectMap, const joda::settings::Classification *classSettings, QGraphicsItem *parent) :
    QGraphicsItem(parent), mObjectMap(objectMap), mClassificationSettings(classSettings)
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
void ContourOverlay::setOverlay(const cv::Size &imageSize, const cv::Size &previewSize)
{
  mImageSize   = imageSize;
  mPreviewSize = previewSize;
  refresh();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void ContourOverlay::refresh()
{
  std::lock_guard<std::mutex> lock(mMutex);
  mPointsToPaint.clear();
  float scaleX = static_cast<float>(mPreviewSize.width) / static_cast<float>(mImageSize.width);
  float scaleY = static_cast<float>(mPreviewSize.height) / static_cast<float>(mImageSize.height);

  for(const auto &[clasId, classs] : *mObjectMap) {
    const auto &classSetting = mClassificationSettings->getClassFromId(clasId);
    QColor col               = QColor(classSetting.color.c_str());
    QColor colBorder         = col.darker(150);
    for(const auto &roi : *classs) {
      const auto &contour = roi.getContour();
      const auto &box     = roi.getBoundingBoxTile();
      std::vector<QPointF> points;
      points.reserve(contour.size());    // Pre-allocate memory
      const float offsetX = static_cast<float>(box.x) * scaleX;
      const float offsetY = static_cast<float>(box.y) * scaleY;
      for(const auto &cont : contour) {
        QPointF itemPoint(static_cast<float>(cont.x) * scaleX + offsetX, static_cast<float>(cont.y) * scaleY + offsetY);
        points.push_back(itemPoint);
      }
      mPointsToPaint.emplace_back(colBorder, points);
    }
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
  // Set the painter to full opacity for the contour overlay
  painter->setOpacity(1.0);

  // Iterate through all prepared contour segments
  for(const auto &[col, points] : mPointsToPaint) {
    QPen pen(col, 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    pen.setCosmetic(true);
    painter->setPen(pen);
    if(!points.empty()) {
      painter->drawPolygon(points.data(), static_cast<int32_t>(points.size()));
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
void ContourOverlay::setAlpha(float)
{
}
