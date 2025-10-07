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

#include "graphics_roi_overlay.hpp"
#include <qgraphicseffect.h>
#include <qgraphicssceneevent.h>
#include <qpainter.h>
#include <qpen.h>
#include <qpolygon.h>
#include <cstddef>
#include <string>
#include "backend/helper/duration_count/duration_count.h"
#include "graphics_contour_overlay.hpp"

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
RoiOverlay::RoiOverlay(const joda::atom::ObjectMap *objectMap, const joda::settings::Classification *classSettings, ContourOverlay *contourOverlay) :
    mObjectMap(objectMap), mClassificationSettings(classSettings), mContourOverlay(contourOverlay)
{
  mOpacityEffect = new QGraphicsOpacityEffect();
  this->setGraphicsEffect(mOpacityEffect);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RoiOverlay::setOverlay(const cv::Size &imageSize, const cv::Size &previewSize)
{
  mImageSize   = imageSize;
  mPreviewSize = previewSize;
  refresh();
}

///
/// \brief      Takes ~60ms
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RoiOverlay::refresh()
{
  const float foreground_weight = 0.25F;                       // New object contributes 25%
  const float background_weight = 1.0F - foreground_weight;    // Background contributes 75%

  if(mObjectMap == nullptr || mClassificationSettings == nullptr) {
    return;
  }
  // Create a QImage with ARGB32 (direct pixel access)
  QImage qimg(mImageSize.width, mImageSize.height, QImage::Format_ARGB32);
  qimg.fill(Qt::transparent);
  mContoursPerColor.clear();

  // Optimization 1: Pre-calculate the pixel value and alpha blending
  for(const auto &[clasId, classs] : *mObjectMap) {
    if(mToHide.contains(clasId)) {
      continue;
    }

    const auto &classSetting = mClassificationSettings->getClassFromId(clasId);

    // Optimization 2: Use QImage::pixel format for direct pixel manipulation
    const QColor col = QColor(classSetting.color.c_str());
    QRgb pixelValue  = qRgb(col.red(), col.green(), col.blue());

    for(const auto &roi : *classs) {
      if(roi.isSelected()) {
        const QColor colTmp = Qt::yellow;
        pixelValue          = qRgb(colTmp.red(), colTmp.green(), colTmp.blue());
      } else {
        pixelValue = qRgb(col.red(), col.green(), col.blue());
      }

      // Prepare contour
      prepareContour(&roi, col);

      // Optimization 3: Efficiently access Mat data
      const auto &mask = roi.getMask();
      const auto &box  = roi.getBoundingBoxTile();

      // Check if mask data is continuous for faster row-by-row processing
      if(mFill) {
        if(mask.isContinuous()) {
          const int total_pixels = mask.rows * mask.cols;
          const uint8_t *p_mask  = mask.ptr<uint8_t>();

          for(int i = 0; i < total_pixels; ++i) {
            if(p_mask[i] > 0) {
              // Calculate (x, y) from flat index 'i'
              const int y_offset = i / mask.cols;
              const int x_offset = i % mask.cols;

              int xx = x_offset + box.x;
              int yy = y_offset + box.y;

              // Optimization 6: Use direct raw pointer access for QImage
              if(qimg.valid(yy, xx)) {
                QRgb *line    = reinterpret_cast<QRgb *>(qimg.scanLine(yy));
                QRgb actColor = line[xx];
                if(qAlpha(actColor) == 0) {
                  line[xx] = pixelValue;
                } else {
                  // Apply the weighted average for each channel:
                  int finalR = static_cast<int>(static_cast<float>(qRed(pixelValue)) * foreground_weight +
                                                static_cast<float>(qRed(actColor)) * background_weight);
                  int finalG = static_cast<int>(static_cast<float>(qGreen(pixelValue)) * foreground_weight +
                                                static_cast<float>(qGreen(actColor)) * background_weight);
                  int finalB = static_cast<int>(static_cast<float>(qBlue(pixelValue)) * foreground_weight +
                                                static_cast<float>(qBlue(actColor)) * background_weight);

                  // Recombine into a new QRgb (using an opaque alpha 255)
                  line[xx] = qRgb(finalR, finalG, finalB);
                }
              }
            }
          }
        }
      }
    }
  }

  // Optimization 8: Check if scaling is actually needed
  QPixmap pix;
  QSize targetSize(mPreviewSize.width, mPreviewSize.height);

  if(targetSize == qimg.size()) {
    pix = QPixmap::fromImage(qimg);
  } else {
    pix = QPixmap::fromImage(qimg.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  }

  prepareGeometryChange();
  setPixmap(pix);
  setAlpha(mAlpha);
  mContourOverlay->refresh(&mContoursPerColor, mPreviewSize);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RoiOverlay::prepareContour(const joda::atom::ROI *roi, const QColor &colBorder)
{
  double scaleX = static_cast<double>(mPreviewSize.width) / static_cast<double>(mImageSize.width);
  double scaleY = static_cast<double>(mPreviewSize.height) / static_cast<double>(mImageSize.height);

  const auto &contour = roi->getContour();
  const auto &box     = roi->getBoundingBoxTile();
  QList<QPointF> points;
  points.reserve(contour.size());    // Pre-allocate memory
  const double offsetX = static_cast<double>(box.x) * scaleX;
  const double offsetY = static_cast<double>(box.y) * scaleY;
  for(const auto &cont : contour) {
    QPointF itemPoint(static_cast<double>(cont.x) * scaleX + offsetX, static_cast<double>(cont.y) * scaleY + offsetY);
    points.push_back(itemPoint);
  }
  QColor c = Qt::yellow;
  // The QRgb type is defined as quint32.
  if(!points.empty()) {
    if(roi->isSelected()) {
      uint32_t cVal                 = static_cast<uint32_t>(c.rgb());
      mContoursPerColor[cVal].first = c;
      mContoursPerColor[cVal].second.addPolygon(QPolygonF(points));
    } else {
      uint32_t cVal                 = static_cast<uint32_t>(colBorder.rgb());
      mContoursPerColor[cVal].first = colBorder;
      mContoursPerColor[cVal].second.addPolygon(QPolygonF(points));
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
void RoiOverlay::setAlpha(float alpha)
{
  mAlpha = alpha;
  mOpacityEffect->setOpacity(static_cast<double>(mAlpha));
  // update();    // Request a redraw
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RoiOverlay::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  if(mSelectable && event->button() == Qt::LeftButton) {
    for(const auto &roi : mSelectedRois) {
      roi->setIsSelected(false);
    }
    mSelectedRois.clear();
    // 1. Get the click point in the item's local coordinates
    QPointF clickPoint = event->pos();

    // 2. Find which ROI contains this point
    joda::atom::ROI *clickedRoi = findRoiAt(clickPoint);
    if(clickedRoi != nullptr) {
      clickedRoi->setIsSelected(true);
      mSelectedRois.emplace(clickedRoi);
    }
    refresh();
  }
  QGraphicsPixmapItem::mousePressEvent(event);    // Call base class
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
joda::atom::ROI *RoiOverlay::findRoiAt(const QPointF &itemPoint) const
{
  double scaleX = static_cast<double>(mPreviewSize.width) / static_cast<double>(mImageSize.width);
  double scaleY = static_cast<double>(mPreviewSize.height) / static_cast<double>(mImageSize.height);

  // Reverse iteration is often preferred so clicking selects the top-most item
  for(const auto &[clasId, classs] : *mObjectMap) {
    for(auto &roi : *classs) {
      // 1. Get the local coordinates of the click relative to the ROI's origin
      // The itemPoint is in the parent's coordinates.
      const auto &box  = roi.getBoundingBoxTile();
      QPointF roiPoint = itemPoint - QPointF(box.x * scaleX, box.y * scaleY);

      // 2. Check if the point is within the *unscaled* ROI dimensions
      if(roiPoint.x() >= 0 && roiPoint.y() >= 0 && roiPoint.x() < box.width * scaleX && roiPoint.y() < box.height * scaleY) {
        // This is the efficient check: check the original mask pixel
        const cv::Mat &mask = roi.getMask();
        if(mask.empty()) {
          continue;
        }

        // Ensure the point is within the mask bounds before calling at<>
        int y = static_cast<int>(roiPoint.y() / scaleY);
        int x = static_cast<int>(roiPoint.x() / scaleX);

        if(y < mask.rows && x < mask.cols) {
          // Check the mask pixel value (assuming the mask is 8UC1)
          if(mask.at<uint8_t>(y, x) > 0) {
            return &roi;    // Return the pointer to the selected ROI
          }
        }
      }
    }
  }
  return nullptr;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RoiOverlay::setClassesToHide(const std::set<joda::enums::ClassId> &toHide)
{
  mToHide = toHide;
  refresh();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RoiOverlay::setSelectable(bool select)
{
  mSelectable = select;
}
