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
  mSize         = size;
  mRectWidth    = static_cast<double>(size.width) / nrCols;
  mRectHeight   = static_cast<double>(size.height) / nrRows;
  auto rectSize = std::min(mRectWidth, mRectHeight);
  mRectWidth    = rectSize;
  mRectHeight   = rectSize;

  auto matSizRowsTmp = static_cast<int32_t>(rectSize * nrRows);
  auto matSizColsTmp = static_cast<int32_t>(rectSize * nrCols);
  if(matSizColsTmp <= 0 || matSizRowsTmp <= 0) {
    return {};
  }
  cv::Mat plotArea(matSizRowsTmp, matSizColsTmp, CV_8UC3, mBackgroundColor);

  auto [min, max] = mData.getMinMax();

  cv::Mat colorLUT = buildColorLUT(cv::COLORMAP_VIRIDIS);

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

      auto x1      = static_cast<int32_t>(static_cast<double>(col) * mRectWidth);
      auto y1      = static_cast<int32_t>(static_cast<double>(row) * mRectHeight);
      auto x2      = static_cast<int32_t>(static_cast<double>(col) * mRectWidth + mRectWidth);
      auto y2      = static_cast<int32_t>(static_cast<double>(row) * mRectHeight + mRectHeight);
      double color = ((val - min) * 255) / max;

      int32_t borderPlace = 1;
      if(mRectWidth <= 25) {
        borderPlace = 0;    // No borders for small rects
      }

      // Plot area
      if(mShape == Shape::RECTANGLE) {
        cv::rectangle(plotArea, {x1 + borderPlace + mGap, y1 + borderPlace + mGap}, {x2 - borderPlace - mGap, y2 - borderPlace - mGap},
                      mapValueToColor(val, min, max, colorLUT), cv::FILLED);
      } else {
        cv::ellipse(plotArea, cv::Point((x1 + x2) / 2, (y1 + y2) / 2),                                   // Center
                    cv::Size((x2 - x1) / 2 - borderPlace - mGap, (y2 - y1) / 2 - borderPlace - mGap),    // Axes (radiusX, radiusY)
                    0,                                                                                   // Angle of rotation
                    0, 360,                                                                              // Start and end angle (full ellipse)
                    mapValueToColor(val, min, max, colorLUT),                                            // Color
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

  return plotArea;
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
