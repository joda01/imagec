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
#include <qgridlayout.h>
#include <qwidget.h>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <iostream>
#include <string>

namespace joda::ui::qt::reporting::plugin {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
PanelHeatmap::PanelHeatmap(QWidget *parent) : QWidget(parent)
{
  // Create and set up the grid layout
  QGridLayout *gridLayout = new QGridLayout(this);
  gridLayout->setSpacing(2);

  mHeatmap01 = new ChartHeatMap(this);
  gridLayout->addWidget(mHeatmap01, 0, 0);
  mHeatmap01->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  gridLayout->addWidget(new ChartHeatMap(this), 0, 1);
  gridLayout->addWidget(new ChartHeatMap(this), 1, 0);
  gridLayout->addWidget(new ChartHeatMap(this), 0, 1);

  // Add spacers for resizing
  gridLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 4);
  gridLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 4, 0);

  setLayout(gridLayout);
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelHeatmap::setData(const joda::results::Table &data)
{
  mHeatmap01->setData(data);
  update();
  mHeatmap01->update();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
ChartHeatMap::ChartHeatMap(QWidget *parent) : QWidget(parent)
{
  setMinimumSize(parent->size());
}

void ChartHeatMap::setData(const joda::results::Table &data)
{
  mData = data;
  std::cout << "Set data " << std::to_string(mData.getRows()) << std::endl;

  update();
}

///
/// \brief      Painter
/// \author     Joachim Danmayr
///
void ChartHeatMap::paintEvent(QPaintEvent *event)
{
  const uint32_t spacing = 4;
  uint32_t rows          = mData.getRows();
  uint32_t cols          = mData.getCols();
  uint32_t width         = size().width() - spacing;
  uint32_t height        = size().height() - spacing;

  if(rows > 0 && cols > 0) {
    uint32_t rectWidth = std::min(width / cols, height / rows);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);    // Enable smooth edges

    // Define rectangle properties
    for(uint32_t x = 0; x < cols; x++) {
      for(uint32_t y = 0; y < rows; y++) {
        QRectF rect(x * rectWidth + spacing, y * rectWidth + spacing, rectWidth, rectWidth);
        int cornerRadius = 10;
        QPainterPath path;
        path.addRoundedRect(rect, cornerRadius, cornerRadius);
        painter.setBrush(Qt::red);    // Change color as desired
        painter.fillPath(path, painter.brush());
        painter.setPen(QPen(Qt::black, 1));
        painter.drawPath(path);
      }
    }
  }
}

}    // namespace joda::ui::qt::reporting::plugin
