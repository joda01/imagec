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
  painter.setRenderHint(QPainter::Antialiasing, true);

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
  const auto &colors     = mImagePanel->mutableImage()->getChannelColors();

  int32_t compression = 1;

  if(number > UINT16_MAX / 2) {
    compression = 2;
  }
  auto rectHeight = height() - static_cast<int32_t>(RECT_START_Y);

  int32_t ch = 0;
  for(const auto &hist : histograms) {
    QPainterPath path;

    int start = 1 + mImagePanel->mutableImage()->getHistogramDisplayAreaLower();
    path.moveTo(0, height() - hist.at<float>(start));

    for(int i = 2; i < number; i += compression) {
      int idx = i + mImagePanel->mutableImage()->getHistogramDisplayAreaLower();
      if(idx > hist.rows) {
        idx = hist.rows;
      }
      // float startX    = (static_cast<float>(width()) - RECT_START_X - RECT_WIDTH) + static_cast<float>(i) * binWidth;
      // float startY    = static_cast<float>(height()) - RECT_START_Y;
      // float histValue = hist.at<float>(idx) * static_cast<float>(rectHeight);
      // painter.drawLine(static_cast<int32_t>(startX), static_cast<int32_t>(startY), static_cast<int32_t>(startX),
      //                  static_cast<int32_t>(startY - histValue));

      {
        double x1 = (i - 1) * binWidth;
        double y1 = height() - hist.at<float>(idx - 1) * rectHeight;
        double x2 = i * binWidth;
        double y2 = height() - hist.at<float>(idx) * rectHeight;

        double cx1 = x1 + binWidth / 3.0;
        double cy1 = y1;
        double cx2 = x2 - binWidth / 3.0;
        double cy2 = y2;

        path.cubicTo(cx1, cy1, cx2, cy2, x2, y2);
      }

      //
      // Indicators
      //
      // Upper level indicator
      /*
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
                          */
    }

    QColor qcolor(static_cast<int>(colors[ch][2] * 255.0f),    // R
                  static_cast<int>(colors[ch][1] * 255.0f),    // G
                  static_cast<int>(colors[ch][0] * 255.0f)     // B
    );
    QPen pen(qcolor);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawPath(path);
    ch++;
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
