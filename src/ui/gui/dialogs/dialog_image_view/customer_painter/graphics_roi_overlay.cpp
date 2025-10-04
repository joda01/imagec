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
#include <qpainter.h>
#include <qpen.h>
#include <cstddef>
#include "backend/helper/duration_count/duration_count.h"
#include "graphics_contour_overlay.hpp"

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
RoiOverlay::RoiOverlay(const joda::atom::ObjectMap *objectMap, const joda::settings::Classification *classSettings) :
    mObjectMap(objectMap), mClassificationSettings(classSettings)
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
  if(mObjectMap == nullptr || mClassificationSettings == nullptr) {
    return;
  }
  // Create a QImage with ARGB32 (direct pixel access)
  QImage qimg(mImageSize.width, mImageSize.height, QImage::Format_ARGB32);
  qimg.fill(Qt::transparent);

  // Optimization 1: Pre-calculate the pixel value and alpha blending
  for(const auto &[clasId, classs] : *mObjectMap) {
    const auto &classSetting = mClassificationSettings->getClassFromId(clasId);

    // Optimization 2: Use QImage::pixel format for direct pixel manipulation
    QColor col = QColor(classSetting.color.c_str());

    // Pre-calculate the full ARGB pixel value
    QRgb pixelValue = qRgb(col.red(), col.green(), col.blue());

    for(const auto &roi : *classs) {
      // Optimization 3: Efficiently access Mat data
      const auto &mask = roi.getMask();
      const auto &box  = roi.getBoundingBoxTile();

      // Check if mask data is continuous for faster row-by-row processing
      if(mFill) {
        if(mask.isContinuous()) {
          const int total_pixels = mask.rows * mask.cols;
          const uint8_t *p_mask  = mask.ptr<uint8_t>();

          //
          // Draw mask
          //
          for(int i = 0; i < total_pixels; ++i) {
            if(p_mask[i] > 0) {
              // Calculate (x, y) from flat index 'i'
              const int y_offset = i / mask.cols;
              const int x_offset = i % mask.cols;

              int xx = x_offset + box.x;
              int yy = y_offset + box.y;

              // Optimization 6: Use direct raw pointer access for QImage
              if(qimg.valid(yy, xx)) {
                QRgb *line = reinterpret_cast<QRgb *>(qimg.scanLine(yy));
                line[xx]   = pixelValue;
              }
            }
          }

        } else {
          // Original row-by-row iteration for non-continuous Mats
          for(int y = 0; y < mask.rows; ++y) {
            const uint8_t *p_mask_row = mask.ptr<uint8_t>(y);
            int yy                    = y + box.y;

            // Optimization 7: Pre-calculate scanLine pointer per row
            QRgb *p_qimg_row = nullptr;
            if(yy >= 0 && yy < qimg.height()) {
              p_qimg_row = reinterpret_cast<QRgb *>(qimg.scanLine(yy));
            }

            for(int x = 0; x < mask.cols; ++x) {
              int xx = x + box.x;
              // Optimization 3.1: Direct pointer access (faster than .at<>)
              if(p_mask_row[x] > 0) {
                // Optimization 6: Use direct raw pointer access for QImage
                if(p_qimg_row != nullptr && xx >= 0 && xx < qimg.width()) {
                  p_qimg_row[xx] = pixelValue;
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
  update();    // Request a redraw
}
