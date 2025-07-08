///
/// \file      plot_heatmap.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "plot_heatmap.hpp"
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::plot {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto Heatmap::plot(const Size &size) -> cv::Mat
{
  uint32_t nrRows = mData.getNrOfRows();
  uint32_t nrCols = mData.getNrOfCols();

  // =========================================
  // Calc rect with
  // =========================================
  mSize = size;
  if(mLegendPosition != LegendPosition::OFF) {
    mRectWidth = static_cast<double>(size.width - LEGEND_WIDTH) / nrCols;
  } else {
    mRectWidth = static_cast<double>(size.width) / nrCols;
  }
  mRectHeight   = static_cast<double>(size.height) / nrRows;
  auto rectSize = std::min(mRectWidth, mRectHeight);
  mRectWidth    = rectSize;
  mRectHeight   = rectSize;

  auto matSizRowsTmp = static_cast<int32_t>(rectSize * nrRows);
  auto matSizColsTmp = static_cast<int32_t>(rectSize * nrCols);
  if(matSizColsTmp <= 0 || matSizRowsTmp <= 0) {
    return {};
  }
  int32_t plotWidth = matSizColsTmp;
  if(mLegendPosition != LegendPosition::OFF) {
    plotWidth += LEGEND_WIDTH;
  }
  cv::Mat plotArea(matSizRowsTmp, plotWidth, CV_8UC3, mBackgroundColor);

  auto colorMapRangeAct = getColorMapRange();

  cv::Mat colorLUT = buildColorLUT(mColorMap);

  // =========================================
  // Plot data
  // =========================================
  for(int32_t col = 0; col < nrCols; col++) {
    for(int32_t row = 0; row < nrRows; row++) {
      const auto &tmpData = mData.data(row, col);
      double val          = std::numeric_limits<double>::quiet_NaN();
      bool isValid        = false;
      if(tmpData != nullptr) {
        val     = tmpData->getVal();
        isValid = tmpData->isValid();
      }

      auto x1 = static_cast<int32_t>(static_cast<double>(col) * mRectWidth);
      auto y1 = static_cast<int32_t>(static_cast<double>(row) * mRectHeight);
      auto x2 = static_cast<int32_t>(static_cast<double>(col) * mRectWidth + mRectWidth);
      auto y2 = static_cast<int32_t>(static_cast<double>(row) * mRectHeight + mRectHeight);

      int32_t borderPlace = 1;
      if(mRectWidth <= 25) {
        borderPlace = 0;    // No borders for small rects
      }

      // Plot area
      if(mShape == Shape::RECTANGLE) {
        cv::rectangle(plotArea, {x1 + borderPlace + mGap, y1 + borderPlace + mGap}, {x2 - borderPlace - mGap, y2 - borderPlace - mGap},
                      mapValueToColor(val, colorMapRangeAct.min, colorMapRangeAct.max, colorLUT), cv::FILLED);
      } else {
        cv::ellipse(plotArea, cv::Point((x1 + x2) / 2, (y1 + y2) / 2),                                   // Center
                    cv::Size((x2 - x1) / 2 - borderPlace - mGap, (y2 - y1) / 2 - borderPlace - mGap),    // Axes (radiusX, radiusY)
                    0,                                                                                   // Angle of rotation
                    0, 360,                                                                              // Start and end angle (full ellipse)
                    mapValueToColor(val, colorMapRangeAct.min, colorMapRangeAct.max, colorLUT),          // Color
                    cv::FILLED, cv::LINE_AA);
      }
      // Plot border
      if(mRectWidth > 25) {
        auto borderColor = cv::Scalar{0, 0, 0};
        if(mShape == Shape::RECTANGLE) {
          cv::rectangle(plotArea, {x1 + mGap, y1 + mGap}, {x2 - mGap, y2 - mGap}, borderColor, 1, cv::LINE_4);
        } else {
          cv::ellipse(plotArea, cv::Point((x1 + x2) / 2, (y1 + y2) / 2),       // Center
                      cv::Size((x2 - x1) / 2 - mGap, (y2 - y1) / 2 - mGap),    // Axes (radiusX, radiusY)
                      0,                                                       // Angle of rotation
                      0, 360,                                                  // Start and end angle (full ellipse)
                      borderColor,                                             // Color
                      1, cv::LINE_AA);
        }
      }

      // Plot labels
      if(mPlotLabels && mRectWidth > 50) {
        cv::Rect rect(x1, y1, mRectWidth, mRectHeight);
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(mPrecision) << val;
        PlotBase::drawCenteredText(plotArea, oss.str(), rect, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1);
      }

      // Cross out if not valid
      if(!isValid && val == val) {
        cv::line(plotArea, {x1, y1}, {x2, y2}, cv::Scalar(0, 0, 255), 2);    // thickness = 2
      }
    }
  }

  // =========================================
  // Plot highlight
  // =========================================
  if(mHighLight.has_value()) {
    if(mHighLight->row < nrRows && mHighLight->col < nrCols) {
      auto x1          = static_cast<int32_t>(static_cast<double>(mHighLight->col) * mRectWidth);
      auto y1          = static_cast<int32_t>(static_cast<double>(mHighLight->row) * mRectHeight);
      auto x2          = static_cast<int32_t>(static_cast<double>(mHighLight->col) * mRectWidth + mRectWidth);
      auto y2          = static_cast<int32_t>(static_cast<double>(mHighLight->row) * mRectHeight + mRectHeight);
      auto borderColor = cv::Scalar{0, 0, 255};
      if(mShape == Shape::RECTANGLE) {
        cv::rectangle(plotArea, {x1 + mGap, y1 + mGap}, {x2 - mGap, y2 - mGap}, borderColor, 1, cv::LINE_4);
      } else {
        cv::ellipse(plotArea, cv::Point((x1 + x2) / 2, (y1 + y2) / 2),       // Center
                    cv::Size((x2 - x1) / 2 - mGap, (y2 - y1) / 2 - mGap),    // Axes (radiusX, radiusY)
                    0,                                                       // Angle of rotation
                    0, 360,                                                  // Start and end angle (full ellipse)
                    borderColor,                                             // Color
                    1, cv::LINE_AA);
      }
    }
  }

  if(mLegendPosition != LegendPosition::OFF) {
    plotLegend(colorMapRangeAct, colorLUT, plotArea);
  }
  return plotArea;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Heatmap::plotLegend(const ColorMappingRange &range, const cv::Mat &colorLUT, cv::Mat &plotArea) const
{
  static const int32_t COLOR_STRIP_WITH = 50;
  int32_t startX                        = plotArea.cols - LEGEND_WIDTH + COLOR_STRIP_WITH;
  int32_t startY                        = 0;
  double rectHeight                     = (double) plotArea.rows / (double) UINT8_MAX;

  for(int32_t idx = 0; idx < UINT8_MAX; idx++) {
    auto color = colorLUT.at<cv::Vec3b>(0, idx);

    cv::rectangle(plotArea, {startX, (int32_t) (startY + idx * rectHeight)},
                  {startX + COLOR_STRIP_WITH, (int32_t) (startY + rectHeight + idx * rectHeight)}, color, cv::FILLED);

    if(idx == 0 || idx == UINT8_MAX - 1 || idx == UINT8_MAX / 2) {
      auto y = startY + idx * rectHeight;
      if(y + 30 >= plotArea.rows) {
        y = plotArea.rows - 45;
      }
      if(idx == UINT8_MAX - 1) {
        idx = UINT8_MAX;
      }
      double value = ((double) idx * ((double) range.max - (double) range.min)) / (double) UINT8_MAX + (double) range.min;
      cv::Rect rect(startX + COLOR_STRIP_WITH + 4, y, 100, 30);
      std::ostringstream oss;
      oss << std::fixed << std::setprecision(mPrecision) << value;
      PlotBase::drawLeftAlignedText(plotArea, oss.str(), rect, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1);
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
void Heatmap::setColorMappingMode(ColorMappingMode mode)
{
  mColorMapMode = mode;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Heatmap::setColorMappingRange(ColorMappingRange range)
{
  mColorMapRange = range;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto Heatmap::getColorMapRange() const -> const ColorMappingRange
{
  if(mColorMapMode == ColorMappingMode::AUTO) {
    auto [min, max] = mData.getMinMax();
    return {min, max};
  } else {
    return mColorMapRange;
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Heatmap::setBackgroundColor(const cv::Vec3b &bg)
{
  mBackgroundColor = bg;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Heatmap::setLegendPosition(LegendPosition position)
{
  mLegendPosition = position;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Heatmap::setColorMap(ColormapName colorMap)
{
  mColorMap = colorMap;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Heatmap::setHighlightCell(const Cell &cell)
{
  mHighLight = cell;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Heatmap::resetHighlight()
{
  mHighLight.reset();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Heatmap::setShape(Shape shape)
{
  mShape = shape;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Heatmap::setGapsBetweenBoxes(int32_t gaps)
{
  mGap = gaps;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto Heatmap::getCellFromCoordinates(double x, double y) const -> std::optional<std::tuple<Cell, joda::table::TableCell>>
{
  uint32_t nrRows = mData.getNrOfRows();
  uint32_t nrCols = mData.getNrOfCols();
  auto row        = static_cast<int32_t>(y / mRectHeight);
  auto col        = static_cast<int32_t>(x / mRectWidth);
  if(row >= 0 && row < nrRows && col >= 0 && col < nrCols) {
    auto data = mData.data(row, col);
    if(data != nullptr) {
      return std::tuple<Cell, joda::table::TableCell>{Cell{col, row}, *mData.data(row, col)};
    } else {
      return std::tuple<Cell, joda::table::TableCell>{Cell{col, row}, {}};
    }
  }

  return std::nullopt;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Heatmap::setData(const joda::table::Table &&data)
{
  mData = data;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto Heatmap::getData() const -> const joda::table::Table &
{
  return mData;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Heatmap::setPlotLabels(bool label)
{
  mPlotLabels = label;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Heatmap::setPrecision(int32_t prec)
{
  mPrecision = prec;
}

}    // namespace joda::plot
