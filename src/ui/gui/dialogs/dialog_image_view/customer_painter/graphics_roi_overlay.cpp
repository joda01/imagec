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
#include <qevent.h>
#include <qgraphicseffect.h>
#include <qgraphicssceneevent.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qpen.h>
#include <qpolygon.h>
#include <qpushbutton.h>
#include <cstddef>
#include <string>
#include "backend/helper/duration_count/duration_count.h"
#include "ui/gui/helper/icon_generator.hpp"
#include "graphics_contour_overlay.hpp"

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
RoiOverlay::RoiOverlay(const std::shared_ptr<joda::atom::ObjectList> &objectMap, const joda::settings::Classification *classSettings,
                       ContourOverlay *contourOverlay, QWidget *parent) :
    mObjectMap(objectMap),
    mClassificationSettings(classSettings), mContourOverlay(contourOverlay), mParentWidget(parent)
{
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RoiOverlay::setOverlay(const cv::Size &imageSize, const cv::Size &previewSize, const joda::enums::TileInfo &tileInfo)
{
  mTileInfo    = tileInfo;
  mImageSize   = imageSize;
  mPreviewSize = previewSize;
  refresh();
}

void RoiOverlay::refresh(const joda::enums::TileInfo &tileInfo)
{
  mTileInfo = tileInfo;
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

  double scaleX = static_cast<double>(mPreviewSize.width) / static_cast<double>(mImageSize.width);
  double scaleY = static_cast<double>(mPreviewSize.height) / static_cast<double>(mImageSize.height);
  // Create a QImage with ARGB32 (direct pixel access)
  QImage qimg(mPreviewSize.width, mPreviewSize.height, QImage::Format_ARGB32);
  qimg.fill(Qt::transparent);
  mContoursPerColor.clear();

  // Optimization 1: Pre-calculate the pixel value and alpha blending
  for(const auto &[clasId, classs] : *mObjectMap) {
    const auto &classSetting = mClassificationSettings->getClassFromId(clasId);

    if(classSetting.hidden) {
      continue;
    }

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
      const auto &box  = roi.getBoundingBoxTile(mTileInfo);

      // Check if real box is in coordinates of the image

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

              const int xx = static_cast<int>(static_cast<double>(x_offset + box.x) * scaleX);
              const int yy = static_cast<int>(static_cast<double>(y_offset + box.y) * scaleY);

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
  pix = QPixmap::fromImage(qimg);
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
  const auto &box     = roi->getBoundingBoxTile(mTileInfo);
  QList<QPointF> points;
  points.reserve(static_cast<int>(contour.size()));    // Pre-allocate memory
  const double offsetX = static_cast<double>(box.x) * scaleX;
  const double offsetY = static_cast<double>(box.y) * scaleY;
  for(const auto &cont : contour) {
    QPointF itemPoint(static_cast<double>(cont.x) * scaleX + offsetX, static_cast<double>(cont.y) * scaleY + offsetY);
    points.push_back(itemPoint);
  }

  // The QRgb type is defined as quint32.
  if(!points.empty()) {
    if(roi->isSelected()) {
      mContoursPerColor.push_back(std::pair<QPen, QPolygonF>{QPen(Qt::yellow, 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin), QPolygonF(points)});
    } else {
      mContoursPerColor.push_back(std::pair<QPen, QPolygonF>{QPen(colBorder, 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin), QPolygonF(points)});
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
  setOpacity(mAlpha);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RoiOverlay::setSelectedRois(const std::set<joda::atom::ROI *> &idxs)
{
  for(const auto &roi : mSelectedRois) {
    roi->setIsSelected(false);
  }

  for(const auto &elem : idxs) {
    elem->setIsSelected(true);
  }
  mSelectedRois = idxs;
  refresh();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
bool RoiOverlay::deleteSelectedRois()
{
  QMessageBox messageBox(mParentWidget);
  auto icon = joda::ui::gui::generateSvgIcon<joda::ui::gui::Style::REGULAR, joda::ui::gui::Color::YELLOW>("warning-circle");
  messageBox.setIconPixmap(icon.pixmap(42, 42));
  messageBox.setWindowTitle("Delete?");
  messageBox.setText("Delete selected annotations?");
  messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
  auto *noButton = messageBox.addButton(tr("No"), QMessageBox::NoRole);
  messageBox.setDefaultButton(noButton);
  messageBox.exec();
  if(messageBox.clickedButton() == noButton) {
    return false;
  }

  mObjectMap->erase(mSelectedRois);
  mSelectedRois.clear();
  refresh();
  return true;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RoiOverlay::deleteRois(const std::set<joda::atom::ROI *> &idxs)
{
  for(joda::atom::ROI *roi : idxs) {
    mSelectedRois.erase(roi);
  }
  mObjectMap->erase(idxs);
  refresh();
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
    emit paintedPolygonClicked(mSelectedRois);
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
      const auto &box  = roi.getBoundingBoxTile(mTileInfo);
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
void RoiOverlay::setSelectable(bool select)
{
  mSelectable = select;
}
