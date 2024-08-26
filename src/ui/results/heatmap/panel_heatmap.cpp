///
/// \file      panel_heatmap.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "panel_heatmap.hpp"
#include <qaction.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qgridlayout.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qsize.h>
#include <qwidget.h>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include "../panel_results.hpp"
#include "backend/helper/database/plugins/control_image.hpp"
#include "backend/helper/database/plugins/heatmap_for_image.hpp"
#include "backend/helper/database/plugins/heatmap_for_plate.hpp"
#include "backend/helper/database/plugins/heatmap_for_well.hpp"
#include "ui/container/container_button.hpp"
#include "ui/container/container_label.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/panel_preview.hpp"
#include "ui/results/dialog_export_data.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
ChartHeatMap::ChartHeatMap(PanelResults *parent) : QWidget(parent), mParent(parent)
{
  setMinimumSize(parent->size());
  setMouseTracking(true);
}

void ChartHeatMap::setData(const joda::table::Table &data, MatrixForm form, PaintControlImage paint,
                           int32_t newHierarchy)
{
  if(mActHierarchy > newHierarchy) {
    // We navigate back
    mSelection[mActHierarchy].mSelectedWell = -1;
    mHoveredWell                            = -1;
  }
  mActHierarchy = newHierarchy;
  mData         = data;
  mRows         = mData.getRows();
  mCols         = mData.getCols();

  mForm           = form;
  mPaintCtrlImage = PaintControlImage::NO;
  update();
  if(mSelection[mActHierarchy].mSelectedWell >= 0) {
    auto x = mSelection[mActHierarchy].mSelectedPoint.x;
    auto y = mSelection[mActHierarchy].mSelectedPoint.y;
    emit onElementClick(x, y, mData.data(x, y));
  }
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
  double dividend = 1;
  if(mPaintCtrlImage == PaintControlImage::YES) {
    dividend = 2;
  }

  uint32_t width  = size().width() / dividend - (spacing + X_LEFT_MARGIN);
  uint32_t height = size().height() - (spacing + Y_TOP_MARING + 2 * LEGEND_HEIGHT);

  if(mMinMaxMode == HeatmapMinMax::AUTO) {
    auto [min, max]    = mData.getMinMax();
    mHeatMapMinMax.min = min;
    mHeatMapMinMax.max = max;
  }

  auto avg = (mHeatMapMinMax.min + mHeatMapMinMax.max) / 2.0;
  //  auto avg        = mData.getAvg();
  //   auto stddev     = mData.getStddev();

  if(mRows > 0 && mCols > 0) {
    uint32_t rectWidth = std::min(width / mCols, height / mRows);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);    // Enable smooth edges

    QFont fontHeader;
    fontHeader.setPixelSize(HEATMAP_FONT_SIZE);
    fontHeader.setBold(false);
    fontHeader.setFamily("Courier New");
    painter.setFont(fontHeader);
    QFontMetrics fm(fontHeader);

    QString newControlImagePath;

    // Define rectangle properties
    uint32_t idx = 0;
    for(uint32_t x = 0; x < mCols; x++) {
      uint32_t txtX = x * rectWidth + spacing + X_LEFT_MARGIN + rectWidth / 2 - 6;
      painter.setPen(QPen(Qt::black, 1));
      painter.drawText(txtX, spacing * 4, QString::number(x + 1));

      for(uint32_t y = 0; y < mRows; y++) {
        uint32_t txtY = y * rectWidth + rectWidth / 2 + spacing + Y_TOP_MARING;
        painter.setPen(QPen(Qt::black, 1));
        char toPrint = y + 'A';
        painter.drawText(spacing, txtY, std::string(1, toPrint).data());

        uint32_t rectXPos = x * rectWidth + spacing + X_LEFT_MARGIN;
        uint32_t rectYPos = y * rectWidth + spacing + Y_TOP_MARING;
        QRectF rect(rectXPos + 2, rectYPos + 2, rectWidth - 4, rectWidth - 4);
        int cornerRadius = 10;
        QPainterPath path;
        // path.addRoundedRect(rect, cornerRadius, cornerRadius);
        switch(mForm) {
          case MatrixForm::CIRCLE:
            path.addEllipse(rect);
            break;
          case MatrixForm::RECTANGLE:
            path.addRect(rect);
            break;
        }

        // Generate a random number
        // double random_number = dis(gen);
        auto data = mData.data(y, x);

        auto ctrl = data.getControlImagePath().string();
        if(!ctrl.empty()) {
          newControlImagePath = ctrl.data();
        }

        double value   = data.getVal();
        double statVal = (value - mHeatMapMinMax.min) / (mHeatMapMinMax.max - mHeatMapMinMax.min);
        // double statVal = (value - avg) / stddev;    // Standadisierung
        // statVal        = calcValueOnGaussianCurve(statVal, avg, stddev);
        // std::cout << "----\n"
        //          << std::to_string(avg) << " | " << std::to_string(stddev) << " | " << std::to_string(statVal) << " |
        //          "
        //          << std::to_string(value) << std::endl;
        auto iter    = findNearest(mColorMap, statVal);
        QColor color = iter.second;

        if(data.isNAN()) {
          color = QColor(255, 255, 255);
        }
        painter.setBrush(color);    // Change color as desired
        painter.fillPath(path, painter.brush());
        if(data.isNAN() && !data.isValid()) {
          painter.setPen(QPen(Qt::lightGray, 1));
        } else if(idx == mSelection[mActHierarchy].mSelectedWell) {
          painter.setPen(QPen(Qt::blue, 2));

        } else if(idx == mHoveredWell) {
          // painter.setPen(QPen(Qt::red, 1));
        } else {
          painter.setPen(QPen(Qt::black, 1));
        }

        painter.drawPath(path);

        const int32_t xReduce = rectWidth / 3;
        const int32_t yReduce = rectWidth / 3;
        if(data.isNAN()) {
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

          if(!data.isValid()) {
            painter.drawLine(x * rectWidth + spacing + xReduce + X_LEFT_MARGIN,
                             y * rectWidth + spacing + yReduce + Y_TOP_MARING,
                             x * rectWidth + spacing + rectWidth - xReduce + X_LEFT_MARGIN,
                             y * rectWidth + spacing + rectWidth - yReduce + Y_TOP_MARING);

            painter.drawLine(x * rectWidth + spacing + rectWidth - xReduce + X_LEFT_MARGIN,
                             y * rectWidth + spacing + yReduce + Y_TOP_MARING,
                             x * rectWidth + spacing + xReduce + X_LEFT_MARGIN,
                             y * rectWidth + spacing + rectWidth - yReduce + Y_TOP_MARING);
          }
        }
        idx++;
      }
    }

    //
    // Paint the legend
    //
    {
      painter.setPen(QPen(Qt::black, 1));
      uint32_t xStart = spacing + X_LEFT_MARGIN;
      uint32_t yStart = mRows * rectWidth + spacing + Y_TOP_MARING + 3 * spacing;
      float length    = (mCols * rectWidth + spacing + X_LEFT_MARGIN) - xStart;
      // painter.drawRect(xStart, yStart, length, LEGEND_HEIGHT);

      uint32_t partWith = std::floor(length / static_cast<float>(mColorMap.size()));
      int middle        = mColorMap.size() / 2;
      for(int n = 0; n < mColorMap.size(); n++) {
        uint32_t startX = xStart + n * partWith;
        float val       = (float) n / (float) mColorMap.size();
        auto color      = mColorMap.upper_bound(val)->second;
        painter.setPen(QPen(Qt::black, 1));
        painter.setBrush(color);    // Change color as desired
        painter.drawRect(startX, yStart, partWith, LEGEND_COLOR_ROW_HEIGHT);

        if(n == 0) {
          if(mMinMaxMode == HeatmapMinMax::AUTO) {
            painter.setPen(QPen(Qt::black, 1));
          } else {
            painter.setPen(QPen(Qt::red, 1));
          }
          mHeatMapMinMax.textMinPos = QRect(startX, yStart + LEGEND_COLOR_ROW_HEIGHT + spacing + HEATMAP_FONT_SIZE,
                                            partWith * 2, HEATMAP_COLOR_ROW_TEXT_HEIGHT);
          painter.drawText(mHeatMapMinMax.textMinPos, Qt::AlignLeft, formatDoubleScientific(mHeatMapMinMax.min));
        }
        if(n == middle) {
          painter.setPen(QPen(Qt::black, 1));
          painter.drawText(startX, yStart + LEGEND_COLOR_ROW_HEIGHT + spacing + HEATMAP_FONT_SIZE,
                           formatDoubleScientific(avg));
        }

        if(n == mColorMap.size() - 1) {
          if(mMinMaxMode == HeatmapMinMax::AUTO) {
            painter.setPen(QPen(Qt::black, 1));
          } else {
            painter.setPen(QPen(Qt::red, 1));
          }
          mHeatMapMinMax.textMaxPos = QRect(startX, yStart + LEGEND_COLOR_ROW_HEIGHT + spacing + HEATMAP_FONT_SIZE,
                                            partWith * 2, HEATMAP_COLOR_ROW_TEXT_HEIGHT);
          painter.drawText(mHeatMapMinMax.textMaxPos, Qt::AlignRight, formatDoubleScientific(mHeatMapMinMax.max));
        }
      }
      painter.setPen(QPen(Qt::black, 1));
      drawGaussianCurve(painter, xStart,
                        yStart + LEGEND_COLOR_ROW_HEIGHT + spacing + HEATMAP_FONT_SIZE + HEATMAP_FONT_SIZE,
                        LEGEND_COLOR_ROW_HEIGHT + spacing + HEATMAP_FONT_SIZE + HEATMAP_FONT_SIZE - 4, length);
    }

    //
    // Paint control image
    //
    /*
    if(mPaintCtrlImage == PaintControlImage::YES) {
      if(newControlImagePath != mActControlImagePath) {
        mActControlImagePath = newControlImagePath;
        auto path            = mAnalyzer.lock()->getAbsolutePathToControlImage(mActControlImagePath.toStdString());

        mActControlImage.load(path.string().data());
        mActControlImage = mActControlImage.scaled(QSize(size().width() / 2, size().height()), Qt::KeepAspectRatio,
                                                   Qt::SmoothTransformation);
      }
      uint32_t xStart = spacing + X_LEFT_MARGIN + (mCols * rectWidth + spacing + Y_TOP_MARING);
      uint32_t yStart = spacing + Y_TOP_MARING;

      uint32_t width  = rectWidth * mCols;
      uint32_t height = rectWidth * mRows;
      painter.drawImage(xStart, yStart, mActControlImage);
    }*/
  }
}

std::pair<float, QColor> ChartHeatMap::findNearest(std::map<float, QColor> &myMap, double target)
{
  // Handle empty map case
  if(myMap.empty()) {
    return {};    // Or throw an appropriate exception
  }

  // Find the middle key using iterators
  auto it = myMap.lower_bound(0.5);

  // Initialize variables to track nearest key and difference
  double nearestKey = it->first;
  double minDiff    = abs(it->first - target);

  // Iterate left and right from the middle, comparing differences
  while(it != myMap.begin()) {
    auto prev       = std::prev(it);
    double prevDiff = abs(prev->first - target);
    if(prevDiff < minDiff) {
      nearestKey = prev->first;
      minDiff    = prevDiff;
    }
    --it;
  }

  it = myMap.begin();
  while(it != myMap.end()) {
    auto next = std::next(it);
    if(next != myMap.end()) {
      double nextDiff = abs(next->first - target);
      if(nextDiff < minDiff) {
        nearestKey = next->first;
        minDiff    = nextDiff;
      }
    }
    ++it;
  }

  // Return the nearest key-value pair
  return std::make_pair(nearestKey, myMap.at(nearestKey));
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ChartHeatMap::drawGaussianCurve(QPainter &painter, int startX, int startY, int height, int length)
{
  // Define Gaussian function parameters
  double sigma = length / 6.0;    // Standard deviation
  double mu    = length / 2.0;    // Mean

  // Calculate points on the curve
  QVector<QPointF> points;
  for(int x = 0; x <= length; ++x) {
    double normalizedX = (x - mu) / sigma;
    double y           = height * exp(-0.5 * normalizedX * normalizedX);
    points.append(QPointF(startX + x, startY - y));
  }

  // Draw the curve
  painter.drawPolyline(points.constData(), points.size());
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
double ChartHeatMap::calcValueOnGaussianCurve(double x, double avg, double sttdev)
{
  double ex  = std::exp(-(1 / 2) * std::pow((x - avg) / sttdev, 2.0));
  double bef = sttdev * std::sqrt(2 * M_PI);
  return ex / bef;
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
  auto [newHoveredWellId, _] = getWellUnderMouse(event);
  if(newHoveredWellId >= 0) {
    if(!mIsHovering) {
      mIsHovering = true;
      // QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
    }
  } else {
    if(mIsHovering) {
      mIsHovering = false;
      // QApplication::restoreOverrideCursor();
    }
  }
  // Update hovering index and trigger repaint if necessary
  if(newHoveredWellId >= 0 && mHoveredWell != newHoveredWellId) {
    mHoveredWell = newHoveredWellId;
    update();    // Trigger repaint to reflect hover state change
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ChartHeatMap::mousePressEvent(QMouseEvent *event)
{
  auto [newSelectedWellId, selectedPoint] = getWellUnderMouse(event);
  // Update hovering index and trigger repaint if necessary
  auto selectedData = mData.data(selectedPoint.x, selectedPoint.y);
  if(!selectedData.isNAN()) {
    if(newSelectedWellId >= 0 && mSelection[mActHierarchy].mSelectedWell != newSelectedWellId) {
      mSelection[mActHierarchy].mSelectedWell  = newSelectedWellId;
      mSelection[mActHierarchy].mSelectedPoint = selectedPoint;
      update();    // Trigger repaint to reflect hover state change
    }

    emit onElementClick(mSelection[mActHierarchy].mSelectedPoint.x, mSelection[mActHierarchy].mSelectedPoint.y,
                        selectedData);
  }

  if(mHeatMapMinMax.textMinPos.contains(event->pos())) {
    // Text min pos clicked
    mHeatMapMinMax.min = showInputDialog(mHeatMapMinMax.min);
    update();
  }

  if(mHeatMapMinMax.textMaxPos.contains(event->pos())) {
    // Text min pos clicked
    mHeatMapMinMax.max = showInputDialog(mHeatMapMinMax.max);
    update();
  }
}

double ChartHeatMap::showInputDialog(double defaultVal)
{
  QInputDialog inputDialog;
  inputDialog.setLabelText("Enter value:");
  inputDialog.setInputMode(QInputDialog::DoubleInput);
  inputDialog.setDoubleDecimals(3);    // Allow up to 3 decimal places
  inputDialog.setCancelButtonText("Reset");
  inputDialog.setDoubleMinimum(std::numeric_limits<double>::min());
  inputDialog.setDoubleMaximum(std::numeric_limits<double>::max());
  inputDialog.setDoubleValue(defaultVal);

  auto ret = inputDialog.exec();
  if(QInputDialog::Accepted == ret) {
    mMinMaxMode = HeatmapMinMax::MANUAL;
    return inputDialog.doubleValue();
  } else {
    mMinMaxMode = HeatmapMinMax::AUTO;
    return defaultVal;
  }
}

///
/// \brief
/// \authors
/// \param[in]
/// \param[out]
/// \return
///
void ChartHeatMap::mouseDoubleClickEvent(QMouseEvent *event)
{
  auto [newSelectedWellId, selectedPoint] = getWellUnderMouse(event);
  // Update hovering index and trigger repaint if necessary
  auto selectedData = mData.data(selectedPoint.x, selectedPoint.y);
  if(!selectedData.isNAN()) {
    if(newSelectedWellId >= 0 && mSelection[mActHierarchy].mSelectedWell != newSelectedWellId) {
      mSelection[mActHierarchy].mSelectedWell  = newSelectedWellId;
      mSelection[mActHierarchy].mSelectedPoint = selectedPoint;
      update();    // Trigger repaint to reflect hover state change
    }
    emit onDoubleClicked(mSelection[mActHierarchy].mSelectedPoint.x, mSelection[mActHierarchy].mSelectedPoint.y,
                         selectedData);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::tuple<int32_t, Point> ChartHeatMap::getWellUnderMouse(QMouseEvent *event)
{
  double dividend = 1;
  if(mPaintCtrlImage == PaintControlImage::YES) {
    dividend = 2;
  }

  int32_t newSelectedWellId = -1;
  Point newSelectedWell;
  uint32_t width  = size().width() / dividend - (spacing + X_LEFT_MARGIN);
  uint32_t height = size().height() - (spacing + Y_TOP_MARING + 2 * LEGEND_HEIGHT);
  auto [min, max] = mData.getMinMax();
  if(mRows > 0 && mCols > 0) {
    uint32_t rectWidth = std::min(width / mCols, height / mRows);
    uint32_t idx       = 0;
    for(uint32_t col = 0; col < mCols; col++) {
      for(uint32_t row = 0; row < mRows; row++) {
        uint32_t rectXPos = col * rectWidth + spacing + X_LEFT_MARGIN;
        uint32_t rectYPos = row * rectWidth + spacing + Y_TOP_MARING;
        QRectF rect(rectXPos, rectYPos, rectWidth, rectWidth);
        if(rect.contains(event->pos())) {
          newSelectedWellId = idx;
          newSelectedWell.x = row;
          newSelectedWell.y = col;
          break;
        }
        idx++;
      }
    }
  }
  return {newSelectedWellId, newSelectedWell};
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

}    // namespace joda::ui
