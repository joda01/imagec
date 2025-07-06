///
/// \file      plot_base.cpp
/// \author    Joachim Danmayr
/// \date      2025-07-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "plot_base.hpp"

namespace joda::plot {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PlotBase::drawCenteredText(cv::Mat &image, const std::string &text, const cv::Rect &rect, int fontFace, double fontScale, int thickness,
                                const cv::Scalar &textColor)
{
  int baseline      = 0;
  cv::Size textSize = cv::getTextSize(text, fontFace, fontScale, thickness, &baseline);

  // Compute centered origin
  int x = rect.x + (rect.width - textSize.width) / 2;
  int y = rect.y + (rect.height + textSize.height) / 2;

  // Draw text
  cv::putText(image, text, cv::Point(x, y), fontFace, fontScale, textColor, thickness, cv::LINE_AA);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Mat PlotBase::buildColorLUT(int colormapType)
{
  cv::Mat lut(1, 256, CV_8UC1);    // grayscale ramp 0..255
  for(int i = 0; i < 256; ++i)
    lut.at<uchar>(0, i) = static_cast<uchar>(i);

  cv::Mat colorLUT;
  cv::applyColorMap(lut, colorLUT, colormapType);    // Apply chosen OpenCV colormap

  return colorLUT;    // Returns a 1x256 CV_8UC3 BGR colormap
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Vec3b PlotBase::mapValueToColor(float value, float vmin, float vmax, const cv::Mat &colorLUT)
{
  if(value != value) {
    return {255, 255, 255};
  }
  // Normalize value to 0–255 index range
  float clamped = std::max(vmin, std::min(vmax, value));
  if(vmax - vmin == 0) {
    return {255, 255, 255};
  }
  int idx = static_cast<int>(255.0f * (clamped - vmin) / (vmax - vmin));
  if(colorLUT.cols > idx) {
    return colorLUT.at<cv::Vec3b>(0, idx);    // Return BGR color
  }

  return {255, 255, 255};
}

}    // namespace joda::plot
