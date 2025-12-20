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
  mFont.setPointSizeF(8);
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

  const double RECT_START_X = 10;
  const double RECT_START_Y = 12;
  // const float RECT_HEIGHT   = 80;
  const float NR_OF_MARKERS = 8;
  double RECT_WIDTH         = static_cast<double>(width()) - (RECT_START_X * 2);

  // Place for the histogram
  painter.setFont(mFont);
  painter.setPen(QColor(0, 89, 179));    // Set the pen color to light blue
  painter.setBrush(Qt::NoBrush);         // Set the brush to no brush for transparent fill

  // Precalculation
  // float histOffset    = mImagePanel->getHistogramOffset();
  // float histZoom      = mImagePanel->getHitogramZoomFactor();
  auto *mutableImage = mImagePanel->mutableImage();
  if(nullptr == mutableImage) {
    return;
  }

  int number             = mutableImage->getHistogramDisplayAreaUpper() - mutableImage->getHistogramDisplayAreaLower();
  double binWidth        = (RECT_WIDTH / static_cast<double>(number));
  int markerPos          = static_cast<int32_t>(static_cast<float>(number) / NR_OF_MARKERS);
  const auto &histograms = mutableImage->getHistogram();
  const auto &colors     = mutableImage->getChannelColors();

  int32_t compression = 1;

  if(number > UINT16_MAX / 2) {
    compression = 2;
  }
  auto rectHeight = height() - 2 * static_cast<int32_t>(RECT_START_Y);

  int32_t ch = 0;
  for(const auto &hist : histograms) {
    QPainterPath path;

    int start = 1 + static_cast<int>(mutableImage->getHistogramDisplayAreaLower());
    path.moveTo(0.0, static_cast<double>(height()) - static_cast<double>(hist.at<float>(start)) - 2.0 * RECT_START_Y);

    for(int i = 2; i < number; i += compression) {
      int idx = i + static_cast<int>(mutableImage->getHistogramDisplayAreaLower());
      if(idx > hist.rows) {
        idx = hist.rows;
      }

      double startX = (static_cast<double>(width()) - RECT_START_X - RECT_WIDTH) + static_cast<double>(i) * binWidth;
      double startY = static_cast<double>(height()) - RECT_START_Y;

      {
        double x1 = static_cast<double>(i - 1) * binWidth;
        double y1 = static_cast<double>(height()) - static_cast<double>(hist.at<float>(idx - 1)) * rectHeight;
        double x2 = static_cast<double>(i) * binWidth;
        double y2 = static_cast<double>(height()) - static_cast<double>(hist.at<float>(idx)) * rectHeight;

        double cx1 = x1 + binWidth / 3.0;
        double cy1 = y1;
        double cx2 = x2 - binWidth / 3.0;
        double cy2 = y2;

        path.cubicTo(cx1, cy1 - 2.0 * RECT_START_Y, cx2, cy2 - 2.0 * RECT_START_Y, x2, y2 - 2.0 * RECT_START_Y);
      }

      //
      // Indicators
      //
      // Upper level indicator
      if(idx == static_cast<int>(mutableImage->getUpperLevelContrast()) ||
         (compression != 1 && idx + 1 == static_cast<int>(mutableImage->getUpperLevelContrast()))) {
        painter.setPen(Qt::black);
        painter.drawText(QRect(static_cast<int>(startX - 50.0), static_cast<int>(startY - 2.0 * RECT_START_Y - 4.0), 100, 12), Qt::AlignHCenter,
                         std::to_string(idx).data());
        painter.drawLine(static_cast<int>(startX), static_cast<int>(startY), static_cast<int>(startX),
                         static_cast<int>(startY - static_cast<double>(rectHeight + 2.0 * RECT_START_Y)));
        painter.setPen(Qt::black);
      }

      // Lower level indicator
      if(idx == static_cast<int>(mutableImage->getLowerLevelContrast()) ||
         (compression != 1 && idx + 1 == static_cast<int>(mutableImage->getLowerLevelContrast()))) {
        painter.setPen(Qt::black);
        painter.drawText(QRect(static_cast<int>(startX - 50.0), static_cast<int>(startY - 2.0 * RECT_START_Y - 4.0), 100, 12), Qt::AlignHCenter,
                         std::to_string(idx).data());
        painter.drawLine(static_cast<int>(startX), static_cast<int>(startY), static_cast<int>(startX),
                         static_cast<int>(startY - static_cast<double>(rectHeight + 2.0 * RECT_START_Y)));
        painter.setPen(Qt::black);
      }

      if(i == 2 || (markerPos > 0 && i % markerPos == 0)) {
        painter.setPen(Qt::black);
        painter.drawText(QRect(static_cast<int>(startX - 50.0), static_cast<int>(startY), 100, 12), Qt::AlignHCenter, std::to_string(idx).data());
      }
    }

    QColor qcolor(static_cast<int>(colors[static_cast<size_t>(ch)][2] * 255.0F),     // R
                  static_cast<int>(colors[static_cast<size_t>(ch)][1] * 255.0F),     // G
                  static_cast<int>(colors[static_cast<size_t>(ch)][0] * 255.0F));    // B

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
