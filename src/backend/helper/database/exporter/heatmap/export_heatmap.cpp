///
/// \file      export_heatmap.cpp
/// \author    Joachim Danmayr
/// \date      2025-03-16
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "export_heatmap.hpp"
#include <QtSvg/qsvggenerator.h>
#include <qpainterpath.h>
#include <QPainter>
#include <random>
#include "backend/helper/database/exporter/heatmap/export_heatmap_settings.hpp"

namespace joda::db {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void HeatmapExporter::setData(const joda::table::Table &data)
{
  mData = data;
  mRows = mData.getRows();
  mCols = mData.getCols();
  // mSettings = settings;

  if(mSettings.heatmapRangeMode == joda::settings::DensityMapSettings::HeatMapRangeMode::AUTO) {
    auto [min, max]        = mData.getMinMax();
    mHeatMapMinMaxAuto.min = min;
    mHeatMapMinMaxAuto.max = max;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void HeatmapExporter::setMinMaxMode(joda::settings::DensityMapSettings::HeatMapRangeMode mode)
{
  mSettings.heatmapRangeMode = mode;
  if(mSettings.heatmapRangeMode == joda::settings::DensityMapSettings::HeatMapRangeMode::AUTO) {
    auto [min, max]        = mData.getMinMax();
    mHeatMapMinMaxAuto.min = min;
    mHeatMapMinMaxAuto.max = max;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void HeatmapExporter::setHeatMapMin(float min)
{
  mSettings.heatmapRangeMin = min;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void HeatmapExporter::setHeatMapMax(float max)
{
  mSettings.heatmapRangeMax = max;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
bool HeatmapExporter::isLegendMinSectionCLicked(const QPoint &pos) const
{
  return mLegendPosition.textMinPos.contains(pos);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
bool HeatmapExporter::isLegendMaxSectionCLicked(const QPoint &pos) const
{
  return mLegendPosition.textMaxPos.contains(pos);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
float HeatmapExporter::getHeatMapMin() const
{
  if(mSettings.heatmapRangeMode == joda::settings::DensityMapSettings::HeatMapRangeMode::AUTO) {
    return mHeatMapMinMaxAuto.min;
  } else {
    return mSettings.heatmapRangeMin;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
float HeatmapExporter::getHeatMapMax() const
{
  if(mSettings.heatmapRangeMode == joda::settings::DensityMapSettings::HeatMapRangeMode::AUTO) {
    return mHeatMapMinMaxAuto.max;
  } else {
    return mSettings.heatmapRangeMax;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void HeatmapExporter::setSelectedIndex(int32_t idx)
{
  mSelectedIndex = idx;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto HeatmapExporter::calcMargins(const QSize &size) const -> std::tuple<float, float, QRect, QFont>
{
  //
  // Define font
  //
  QFont fontHeader;
  fontHeader.setPixelSize(size.height() / 65);
  fontHeader.setBold(false);
  fontHeader.setFamily("Courier New");
  QFontMetrics fm(fontHeader);
  QRect headerMetrics = fm.boundingRect("A");
  QString newControlImagePath;

  //
  // Define spacings
  //
  float Y_TOP_MARING  = headerMetrics.height() + spacing * 2;
  float X_LEFT_MARGIN = headerMetrics.width() * 2 + spacing * 2;
  return {X_LEFT_MARGIN, Y_TOP_MARING, headerMetrics, fontHeader};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
HeatmapExporter::HeatMapMinMax HeatmapExporter::getHeatmapMinMax() const
{
  HeatMapMinMax heatmapMinMax = mHeatMapMinMaxAuto;
  if(mSettings.heatmapRangeMode != joda::settings::DensityMapSettings::HeatMapRangeMode::AUTO) {
    heatmapMinMax.min = mSettings.heatmapRangeMin;
    heatmapMinMax.max = mSettings.heatmapRangeMax;
  }
  return heatmapMinMax;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void HeatmapExporter::paint(QPainter &painter, const QSize &size, bool updatePosition) const
{
  // mData.print();
  //  Create a random device and use it to seed the random number generator
  std::random_device rd;
  std::mt19937 gen(rd());

  // Create a uniform real distribution to produce numbers in the range [0, 1)
  std::uniform_real_distribution<> dis(0.0, 1.0);
  double dividend = 1;

  auto [X_LEFT_MARGIN, Y_TOP_MARING, headerMetrics, font] = calcMargins(size);
  painter.setFont(font);
  float height = size.height() - (spacing + Y_TOP_MARING + 2 * (LEGEND_COLOR_ROW_HEIGHT + headerMetrics.height() + spacing));

  //
  // Canvas size
  //
  float width = size.width() / dividend - (spacing + X_LEFT_MARGIN);

  if(mRows > 0 && mCols > 0) {
    float rectWidth = std::min((float) width / (float) mCols, (float) height / (float) mRows);
    float xOffset   = (width / 2.0) - (rectWidth * mCols + spacing + X_LEFT_MARGIN - 4) / 2.0;
    painter.setRenderHint(QPainter::Antialiasing);    // Enable smooth edges
    // Define rectangle properties
    uint32_t idx = 0;
    for(uint32_t x = 0; x < mCols; x++) {
      float txtX = (x * rectWidth + spacing + X_LEFT_MARGIN + rectWidth / 2 - 6.0) + xOffset;
      painter.setPen(QPen(Qt::black, 1));
      painter.drawText(txtX, headerMetrics.height() + spacing, QString::number(x + 1));

      for(uint32_t y = 0; y < mRows; y++) {
        float rectXPos = (x * rectWidth + spacing + X_LEFT_MARGIN) + xOffset;
        float rectYPos = y * rectWidth + spacing + Y_TOP_MARING;
        painter.setPen(QPen(Qt::black, 1));
        // Draw header left
        if(x == 0) {
          float txtY   = y * rectWidth + rectWidth / 2 + spacing + Y_TOP_MARING;
          char toPrint = y + 'A';
          painter.drawText(rectXPos - (headerMetrics.width() + spacing), txtY, std::string(1, toPrint).data());
        }
        // Draw rect
        QRectF rect(rectXPos + 2, rectYPos + 2, rectWidth - 4, rectWidth - 4);
        int cornerRadius = 10;
        QPainterPath path;
        // path.addRoundedRect(rect, cornerRadius, cornerRadius);
        switch(mSettings.form) {
          case joda::settings::DensityMapSettings::ElementForm::CIRCLE:
            path.addEllipse(rect);
            break;
          case joda::settings::DensityMapSettings::ElementForm::RECTANGLE:
            path.addRect(rect);
            break;
        }

        // Generate a random number
        // double random_number = dis(gen);
        auto data = mData.data(y, x);
        auto ctrl = data.getControlImagePath().string();
        if(!ctrl.empty()) {
          auto newControlImagePath = ctrl.data();
        }

        auto heatmapMinMax = getHeatmapMinMax();
        double value       = data.getVal();
        double statVal     = (value - heatmapMinMax.min) / (heatmapMinMax.max - heatmapMinMax.min);
        auto iter          = findNearest(mColorMap, statVal);
        QColor color       = iter.second;

        if(data.isNAN()) {
          color = QColor(255, 255, 255);
        }
        painter.setBrush(color);    // Change color as desired
        painter.fillPath(path, painter.brush());
        if(data.isNAN() && !data.isValid()) {
          painter.setPen(QPen(Qt::lightGray, 1));
        } else if(idx == mSelectedIndex) {
          painter.setPen(QPen(Qt::blue, 2));

        } else if(false /*idx == mHoveredWell*/) {
          // painter.setPen(QPen(Qt::red, 1));
        } else {
          painter.setPen(QPen(Qt::black, 1));
        }

        painter.drawPath(path);

        const float xReduce = rectWidth / 3.0;
        const float yReduce = rectWidth / 3.0;
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
          float textX = (rect.center().x() - textRect.width() / 2.0);
          float textY = rect.center().y() + textRect.height() / 2.0 - fontMetrics.descent();    // Adjust for descent
          painter.drawText(textX, textY, txtToPaint);

          if(!data.isValid()) {
            painter.drawLine((x * rectWidth + spacing + xReduce + X_LEFT_MARGIN) + xOffset, y * rectWidth + spacing + yReduce + Y_TOP_MARING,
                             (x * rectWidth + spacing + rectWidth - xReduce + X_LEFT_MARGIN) + xOffset,
                             y * rectWidth + spacing + rectWidth - yReduce + Y_TOP_MARING);

            painter.drawLine((x * rectWidth + spacing + rectWidth - xReduce + X_LEFT_MARGIN) + xOffset,
                             y * rectWidth + spacing + yReduce + Y_TOP_MARING, (x * rectWidth + spacing + xReduce + X_LEFT_MARGIN) + xOffset,
                             y * rectWidth + spacing + rectWidth - yReduce + Y_TOP_MARING);
          }
        }
        idx++;
      }
    }

    //
    // Paint the legend
    //
    drawLegend(painter, rectWidth, xOffset, X_LEFT_MARGIN, Y_TOP_MARING, updatePosition, headerMetrics);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void HeatmapExporter::drawLegend(QPainter &painter, float rectWidth, float xOffset, float X_LEFT_MARGIN, float Y_TOP_MARING, bool updatePosition,
                                 const QRect &headerMetrics) const
{
  auto heatmapMinMax = getHeatmapMinMax();
  LegendPosition legendPosition;
  auto avg = (heatmapMinMax.min + heatmapMinMax.max) / 2.0;

  painter.setPen(QPen(Qt::black, 1));
  float xStart = (spacing + X_LEFT_MARGIN) + xOffset + 2;
  float yStart = mRows * rectWidth + spacing + Y_TOP_MARING + 3.0 * spacing;
  float length = (rectWidth * mCols);
  // painter.drawRect(xStart, yStart, length, LEGEND_HEIGHT);

  float partWith = length / static_cast<float>(mColorMap.size());
  int middle     = mColorMap.size() / 2.0;
  for(int n = 0; n < mColorMap.size(); n++) {
    float startX = xStart + (float) n * partWith;
    float val    = (float) n / (float) mColorMap.size();
    auto color   = mColorMap.upper_bound(val)->second;
    painter.setPen(QPen(Qt::black, 1));
    painter.setBrush(color);    // Change color as desired
    painter.drawRect(startX, yStart, partWith, LEGEND_COLOR_ROW_HEIGHT);

    if(n == 0) {
      if(mSettings.heatmapRangeMode == joda::settings::DensityMapSettings::HeatMapRangeMode::AUTO) {
        painter.setPen(QPen(Qt::black, 1));
      } else {
        painter.setPen(QPen(Qt::red, 1));
      }
      legendPosition.textMinPos =
          QRect(startX, yStart + LEGEND_COLOR_ROW_HEIGHT + spacing + headerMetrics.height(), partWith * 2, headerMetrics.height());
      painter.drawText(legendPosition.textMinPos, Qt::AlignLeft, formatDoubleScientific(heatmapMinMax.min));
    }
    if(n == middle) {
      painter.setPen(QPen(Qt::black, 1));
      auto rect = QRect(startX, yStart + LEGEND_COLOR_ROW_HEIGHT + spacing + headerMetrics.height(), partWith * 2, headerMetrics.height());
      painter.drawText(rect, Qt::AlignHCenter, formatDoubleScientific(avg));
    }

    if(n == mColorMap.size() - 1) {
      // This is the last element
      if(mSettings.heatmapRangeMode == joda::settings::DensityMapSettings::HeatMapRangeMode::AUTO) {
        painter.setPen(QPen(Qt::black, 1));
      } else {
        painter.setPen(QPen(Qt::red, 1));
      }
      legendPosition.textMaxPos =
          QRect(startX, yStart + LEGEND_COLOR_ROW_HEIGHT + spacing + headerMetrics.height(), partWith * 2, headerMetrics.height());

      auto rect =
          QRect(startX - 4 * partWith, yStart + LEGEND_COLOR_ROW_HEIGHT + spacing + headerMetrics.height(), 5 * partWith, headerMetrics.height());
      painter.drawText(rect, Qt::AlignRight, formatDoubleScientific(heatmapMinMax.max));
    }
  }
  painter.setPen(QPen(Qt::black, 1));
  drawGaussianCurve(painter, xStart, yStart + LEGEND_COLOR_ROW_HEIGHT + spacing + headerMetrics.height() + headerMetrics.height(),
                    LEGEND_COLOR_ROW_HEIGHT + spacing + headerMetrics.height() + headerMetrics.height() - 4, length);

  if(updatePosition) {
    mLegendPosition = legendPosition;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void HeatmapExporter::drawGaussianCurve(QPainter &painter, int startX, int startY, int height, int length) const
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
std::pair<float, QColor> HeatmapExporter::findNearest(const std::map<float, QColor> &myMap, double target) const
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
std::tuple<int32_t, QPoint> HeatmapExporter::getWellAtPosition(const QPoint &pos, const QSize &size) const
{
  auto [X_LEFT_MARGIN, Y_TOP_MARING, headerMetrics, font] = calcMargins(size);

  double dividend = 1;

  int32_t newSelectedWellId = -1;
  QPoint newSelectedWell;
  float width  = size.width() / dividend - (spacing + X_LEFT_MARGIN);
  float height = size.height() - (spacing + Y_TOP_MARING + 2.0 * (LEGEND_COLOR_ROW_HEIGHT + headerMetrics.height() + spacing));

  auto [min, max] = mData.getMinMax();
  if(mRows > 0 && mCols > 0) {
    float rectWidth = std::min((float) width / (float) mCols, (float) height / (float) mRows);
    float xOffset   = (width / 2.0) - (rectWidth * mCols + spacing + X_LEFT_MARGIN - 4) / 2.0;
    uint32_t idx    = 0;
    for(uint32_t col = 0; col < mCols; col++) {
      for(uint32_t row = 0; row < mRows; row++) {
        uint32_t rectXPos = col * rectWidth + spacing + X_LEFT_MARGIN;
        uint32_t rectYPos = row * rectWidth + spacing + Y_TOP_MARING;
        QRectF rect(rectXPos + xOffset, rectYPos, rectWidth, rectWidth);
        if(rect.contains(pos)) {
          newSelectedWellId = idx;
          newSelectedWell.setX(row);
          newSelectedWell.setY(col);
          break;
        }
        idx++;
      }
    }
  }
  return {newSelectedWellId, newSelectedWell};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
double HeatmapExporter::calcValueOnGaussianCurve(double x, double avg, double sttdev) const
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
QString HeatmapExporter::formatDoubleScientific(double value, int precision) const
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

///
/// \brief      Export to SVG
/// \author     Joachim Danmayr
///
void HeatmapExporter::exportToSVG(const QString &filePath) const
{
  QSize size{2048, 2048};
  QSvgGenerator generator;
  generator.setFileName(filePath);
  generator.setSize(size);    // Use widget size for SVG
  generator.setViewBox(QRect(0, 0, size.width(), size.height()));
  generator.setTitle("ImageC - Heatmap");
  generator.setDescription("ImageC - Heatmap export");
  QPainter svgPainter(&generator);
  paint(svgPainter, size, false);    // Call the common drawing function
}

///
/// \brief      Export to PNG
/// \author     Joachim Danmayr
///
void HeatmapExporter::exportToPNG(const QString &filePath) const
{
  QSize size{2048, 2048};
  QImage image(size, QImage::Format_ARGB32);
  image.fill(Qt::white);    // Set background color
  QPainter imagePainter(&image);
  paint(imagePainter, size, false);    // Call common drawing function
  image.save(filePath, "PNG");         // Save as PNG
}

}    // namespace joda::db
