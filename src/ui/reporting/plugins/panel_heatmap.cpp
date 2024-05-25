///
/// \file      panel_heatmap.cpp
/// \author    Joachim Danmayr
/// \date      2024-05-21
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "panel_heatmap.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qgridlayout.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <iostream>
#include <random>
#include <string>
#include "ui/helper/layout_generator.hpp"

namespace joda::ui::qt::reporting::plugin {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
PanelHeatmap::PanelHeatmap(QWidget *parent) : QWidget(parent)
{
  // Create and set up the grid layout
  auto *horizontalLayout = joda::ui::qt::helper::createLayout(this);

  //
  // Plate view
  //
  {
    auto [plateViewer, plateViewerWidget] =
        joda::ui::qt::helper::addVerticalPanel(horizontalLayout, "rgb(251, 252, 253)", 16, false, 800, 2048);
    mHeatmap01 = new ChartHeatMap(this);
    plateViewer->addWidget(mHeatmap01);
    mHeatmap01->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    plateViewerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  }

  //
  // Well edit
  //
  {
    auto [verticalLayoutContainer, _1] =
        joda::ui::qt::helper::addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 250, 250, 16);

    auto [verticalLayoutMeta, _2] =
        joda::ui::qt::helper::addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");

    verticalLayoutMeta->addWidget(joda::ui::qt::helper::createTitle("Well editor"));

    _2->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    verticalLayoutContainer->addStretch();
  }

  setLayout(horizontalLayout);
  // horizontalLayout->addStretch();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelHeatmap::setData(const joda::results::Table &data)
{
  mHeatmap01->setData(data);
  update();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
ChartHeatMap::ChartHeatMap(QWidget *parent) : QWidget(parent)
{
  setMinimumSize(parent->size());
  setMouseTracking(true);
}

void ChartHeatMap::setData(const joda::results::Table &data)
{
  mData = data;
  mRows = mData.getRows();
  mCols = mData.getCols();

  update();
}

///
/// \brief      Painter
/// \author     Joachim Danmayr
///
void ChartHeatMap::paintEvent(QPaintEvent *event)
{
  // mData.print();
  //  Create a random device and use it to seed the random number generator
  std::random_device rd;
  std::mt19937 gen(rd());

  // Create a uniform real distribution to produce numbers in the range [0, 1)
  std::uniform_real_distribution<> dis(0.0, 1.0);

  uint32_t width  = size().width() - (spacing + X_LEFT_MARGIN);
  uint32_t height = size().height() - (spacing + Y_TOP_MARING);

  auto [min, max] = mData.getMinMax();

  if(mRows > 0 && mCols > 0) {
    uint32_t rectWidth = std::min(width / mCols, height / mRows);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);    // Enable smooth edges

    QFont fontHeader;
    fontHeader.setPixelSize(12);
    fontHeader.setBold(false);
    fontHeader.setFamily("Courier New");
    painter.setFont(fontHeader);
    QFontMetrics fm(fontHeader);

    // Define rectangle properties
    uint32_t idx = 0;
    for(uint32_t x = 0; x < mCols; x++) {
      uint32_t txtX = x * rectWidth + spacing + X_LEFT_MARGIN + rectWidth / 2 - 6;
      painter.drawText(txtX, spacing * 4, QString::number(x));

      for(uint32_t y = 0; y < mRows; y++) {
        uint32_t txtY   = y * rectWidth + rectWidth / 2 + spacing + Y_TOP_MARING;
        char toPrint[2] = {0};
        toPrint[0]      = y + 'A';
        painter.drawText(spacing, txtY, std::string(toPrint, 1).data());

        uint32_t rectXPos = x * rectWidth + spacing + X_LEFT_MARGIN;
        uint32_t rectYPos = y * rectWidth + spacing + Y_TOP_MARING;
        QRectF rect(rectXPos, rectYPos, rectWidth, rectWidth);
        int cornerRadius = 10;
        QPainterPath path;
        // path.addRoundedRect(rect, cornerRadius, cornerRadius);
        path.addEllipse(rect);
        // Generate a random number
        // double random_number = dis(gen);
        auto data    = mData.data();
        double value = data[y][x].getVal();
        double val   = (value - min) / (max - min);
        auto color   = mColorMap.upper_bound(val)->second;
        if(!data[y][x].isValid()) {
          color = QColor(255, 255, 255);
        }

        painter.setBrush(color);    // Change color as desired
        painter.fillPath(path, painter.brush());
        if(idx == mHoveredWell) {
          painter.setPen(QPen(Qt::red, 1));
        } else {
          painter.setPen(QPen(Qt::black, 1));
        }
        painter.drawPath(path);

        const int32_t xReduce = rectWidth / 3;
        const int32_t yReduce = rectWidth / 3;
        if(!data[y][x].isValid()) {
          painter.drawLine(x * rectWidth + spacing + xReduce + X_LEFT_MARGIN,
                           y * rectWidth + spacing + yReduce + Y_TOP_MARING,
                           x * rectWidth + spacing + rectWidth - xReduce + X_LEFT_MARGIN,
                           y * rectWidth + spacing + rectWidth - yReduce + Y_TOP_MARING);

          painter.drawLine(x * rectWidth + spacing + rectWidth - xReduce + X_LEFT_MARGIN,
                           y * rectWidth + spacing + yReduce + Y_TOP_MARING,
                           x * rectWidth + spacing + xReduce + X_LEFT_MARGIN,
                           y * rectWidth + spacing + rectWidth - yReduce + Y_TOP_MARING);
        } else {
          QString txtToPaint = formatDoubleScientific(value);
          // Get text metrics
          QFontMetrics fontMetrics(painter.font());
          QRect textRect = fontMetrics.boundingRect(txtToPaint);
          int precision  = 2;
          while(textRect.width() >= rectWidth) {
            if(precision < 0) {
              txtToPaint = "...";
              textRect   = fontMetrics.boundingRect(txtToPaint);
              break;
            } else {
              txtToPaint = formatDoubleScientific(value, precision);
              textRect   = fontMetrics.boundingRect(txtToPaint);
            }
            precision--;
          }
          // Calculate center coordinates for text placement within the rectangle
          int textX = rect.center().x() - textRect.width() / 2;
          int textY = rect.center().y() + textRect.height() / 2 - fontMetrics.descent();    // Adjust for descent
          painter.drawText(textX, textY, txtToPaint);
        }
        idx++;
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
void ChartHeatMap::mouseMoveEvent(QMouseEvent *event)
{
  int32_t newHoveringIdx = -1;
  uint32_t width         = size().width() - (spacing + X_LEFT_MARGIN);
  uint32_t height        = size().height() - (spacing + Y_TOP_MARING);
  auto [min, max]        = mData.getMinMax();
  if(mRows > 0 && mCols > 0) {
    uint32_t rectWidth = std::min(width / mCols, height / mRows);
    uint32_t idx       = 0;
    for(uint32_t x = 0; x < mCols; x++) {
      for(uint32_t y = 0; y < mRows; y++) {
        uint32_t rectXPos = x * rectWidth + spacing + X_LEFT_MARGIN;
        uint32_t rectYPos = y * rectWidth + spacing + Y_TOP_MARING;
        QRectF rect(rectXPos, rectYPos, rectWidth, rectWidth);
        if(rect.contains(event->pos())) {
          newHoveringIdx = idx;
          break;
        }
        idx++;
      }
    }
  }
  // Update hovering index and trigger repaint if necessary
  if(mHoveredWell != newHoveringIdx) {
    mHoveredWell = newHoveringIdx;
    update();    // Trigger repaint to reflect hover state change
  }
}

QString ChartHeatMap::formatDoubleScientific(double value, int precision)
{
  QString formattedString = QString::number(value, 'e', precision);
  // Optional: Handle special cases like zero or infinity
  if(formattedString == "0.0e+00") {
    formattedString = "0";
  } else if(formattedString.contains("inf") || formattedString.contains("nan")) {
    formattedString = "N/A";    // Or any appropriate placeholder
  }
  return formattedString;
}

}    // namespace joda::ui::qt::reporting::plugin
