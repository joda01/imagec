///
/// \file      panel_histogram.cpp
/// \author    Joachim Danmayr
/// \date      2025-07-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "panel_histogram.hpp"
#include <qboxlayout.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qwidget.h>
#include "backend/helper/image/image.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
PanelHistogram::PanelHistogram(joda::image::Image *image, QWidget *parent) : QWidget(parent), mImage(image)
{
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PanelHistogram::paintEvent(QPaintEvent *event)
{
  QWidget::paintEvent(event);

  QPainter painter(this);
  drawHistogram(painter);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelHistogram::drawHistogram(QPainter &painter)
{
  const auto *image = mImage->getImage();
  if(image == nullptr) {
    return;
  }

  const float RECT_START_X = 10;
  const float RECT_START_Y = 12;
  // const float RECT_HEIGHT   = 80;
  const float NR_OF_MARKERS = 8;
  float RECT_WIDTH          = static_cast<float>(width()) - (RECT_START_X * 2);

  int type  = image->type();
  int depth = type & CV_MAT_DEPTH_MASK;
  if(depth == CV_16U && 1 == image->channels()) {
    if(!image->empty()) {
      // Place for the histogram
      QFont font;
      font.setPointSizeF(8);
      painter.setFont(font);
      painter.setPen(QColor(0, 89, 179));    // Set the pen color to light blue
      painter.setBrush(Qt::NoBrush);         // Set the brush to no brush for transparent fill

      // Precalculation
      // float histOffset    = mImage->getHistogramOffset();
      // float histZoom      = mImage->getHitogramZoomFactor();
      int number          = mImage->getHistogramDisplayAreaUpper() - mImage->getHistogramDisplayAreaLower();
      float binWidth      = (RECT_WIDTH / static_cast<float>(number));
      int markerPos       = number / NR_OF_MARKERS;
      const auto &hist    = mImage->getHistogram();
      int32_t compression = 1;

      if(number > UINT16_MAX / 2) {
        compression = 2;
      }
      auto rectHeight = height() - RECT_START_Y;
      for(int i = 1; i < number; i += compression) {
        int idx = i + mImage->getHistogramDisplayAreaLower();
        if(idx > UINT16_MAX) {
          idx = UINT16_MAX;
        }
        float startX    = (static_cast<float>(width()) - RECT_START_X - RECT_WIDTH) + static_cast<float>(i) * binWidth;
        float startY    = static_cast<float>(height()) - RECT_START_Y;
        float histValue = hist.at<float>(idx) * rectHeight;
        painter.drawLine(startX, startY, startX, startY - histValue);

        // Upper level indicator
        if(idx == mImage->getUpperLevelContrast() || (compression != 1 && idx + 1 == mImage->getUpperLevelContrast())) {
          painter.setPen(QColor(255, 0, 0));    // Set the pen color to red
          painter.drawText(QRect(startX - 50, startY, 100, 12), Qt::AlignHCenter, std::to_string(idx).data());
          painter.drawLine(startX, startY, startX, startY - rectHeight);
          painter.setPen(QColor(0, 89, 179));    // Set the pen color to light blue
        }

        // Lower level indicator
        if(idx == mImage->getLowerLevelContrast() || (compression != 1 && idx + 1 == mImage->getLowerLevelContrast())) {
          painter.setPen(QColor(255, 0, 0));    // Set the pen color to red
          painter.drawText(QRect(startX - 50, startY, 100, 12), Qt::AlignHCenter, std::to_string(idx).data());
          painter.drawLine(startX, startY, startX, startY - rectHeight);
          painter.setPen(QColor(0, 89, 179));    // Set the pen color to light blue
        }

        if(i == 1 || i % markerPos == 0) {
          painter.drawText(QRect(startX - 50, startY, 100, 12), Qt::AlignHCenter, std::to_string(idx).data());
        }
      }
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelHistogram::mouseMoveEvent(QMouseEvent *event)
{
  QWidget::mouseMoveEvent(event);
}

}    // namespace joda::ui::gui
