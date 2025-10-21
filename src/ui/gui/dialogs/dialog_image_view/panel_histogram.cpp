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
#include "panel_image_view.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
PanelHistogram::PanelHistogram(PanelImageView *image, QWidget *parent) : QWidget(parent), mImagePanel(image)
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
  const float RECT_START_X = 10;
  const float RECT_START_Y = 12;
  // const float RECT_HEIGHT   = 80;
  const float NR_OF_MARKERS = 8;
  float RECT_WIDTH          = static_cast<float>(width()) - (RECT_START_X * 2);

  // Place for the histogram
  QFont font;
  font.setPointSizeF(8);
  painter.setFont(font);
  painter.setPen(QColor(0, 89, 179));    // Set the pen color to light blue
  painter.setBrush(Qt::NoBrush);         // Set the brush to no brush for transparent fill

  // Precalculation
  // float histOffset    = mImagePanel->getHistogramOffset();
  // float histZoom      = mImagePanel->getHitogramZoomFactor();
  int number             = mImagePanel->mutableImage()->getHistogramDisplayAreaUpper() - mImagePanel->mutableImage()->getHistogramDisplayAreaLower();
  float binWidth         = (RECT_WIDTH / static_cast<float>(number));
  int markerPos          = static_cast<int32_t>(static_cast<float>(number) / NR_OF_MARKERS);
  const auto &histograms = mImagePanel->mutableImage()->getHistogram();
  int32_t compression    = 1;

  if(number > UINT16_MAX / 2) {
    compression = 2;
  }
  auto rectHeight = height() - static_cast<int32_t>(RECT_START_Y);

  for(const auto &hist : histograms) {
    for(int i = 1; i < number; i += compression) {
      int idx = i + mImagePanel->mutableImage()->getHistogramDisplayAreaLower();
      if(idx > UINT16_MAX) {
        idx = UINT16_MAX;
      }
      float startX    = (static_cast<float>(width()) - RECT_START_X - RECT_WIDTH) + static_cast<float>(i) * binWidth;
      float startY    = static_cast<float>(height()) - RECT_START_Y;
      float histValue = hist.at<float>(idx) * static_cast<float>(rectHeight);
      painter.drawLine(static_cast<int32_t>(startX), static_cast<int32_t>(startY), static_cast<int32_t>(startX),
                       static_cast<int32_t>(startY - histValue));

      // Upper level indicator
      if(idx == mImagePanel->mutableImage()->getUpperLevelContrast() ||
         (compression != 1 && idx + 1 == mImagePanel->mutableImage()->getUpperLevelContrast())) {
        painter.setPen(QColor(255, 0, 0));    // Set the pen color to red
        painter.drawText(QRect(static_cast<int32_t>(startX - 50), static_cast<int32_t>(startY), 100, 12), Qt::AlignHCenter,
                         std::to_string(idx).data());
        painter.drawLine(static_cast<int32_t>(startX), static_cast<int32_t>(startY), static_cast<int32_t>(startX),
                         static_cast<int32_t>(startY - static_cast<float>(rectHeight)));
        painter.setPen(QColor(0, 89, 179));    // Set the pen color to light blue
      }

      // Lower level indicator
      if(idx == mImagePanel->mutableImage()->getLowerLevelContrast() ||
         (compression != 1 && idx + 1 == mImagePanel->mutableImage()->getLowerLevelContrast())) {
        painter.setPen(QColor(255, 0, 0));    // Set the pen color to red
        painter.drawText(QRect(static_cast<int32_t>(startX - 50), static_cast<int32_t>(startY), 100, 12), Qt::AlignHCenter,
                         std::to_string(idx).data());
        painter.drawLine(static_cast<int32_t>(startX), static_cast<int32_t>(startY), static_cast<int32_t>(startX),
                         static_cast<int32_t>(startY - static_cast<float>(rectHeight)));
        painter.setPen(QColor(0, 89, 179));    // Set the pen color to light blue
      }

      if(i == 1 || i % markerPos == 0) {
        painter.drawText(QRect(static_cast<int32_t>(startX - 50), static_cast<int32_t>(startY), 100, 12), Qt::AlignHCenter,
                         std::to_string(idx).data());
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
