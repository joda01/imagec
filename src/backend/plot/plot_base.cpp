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
std::string PlotBase::doubleToString(double value, int32_t precision)
{
  std::ostringstream oss;

  // Absolute value for comparison
  double abs_val = std::abs(value);

  // Use scientific notation if |value| >= 1e5 or |value| < 1e-4 but not zero
  if((abs_val >= 1e4 || (abs_val > 0 && abs_val < 1e-4))) {
    oss << std::scientific << std::setprecision(precision);
  } else {
    oss << std::fixed << std::setprecision(precision);
  }

  oss << value;
  return oss.str();
}

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
void PlotBase::drawLeftAlignedText(cv::Mat &image, const std::string &text, const cv::Rect &rect, int fontFace, double fontScale, int thickness,
                                   const cv::Scalar &textColor)
{
  int baseline      = 0;
  cv::Size textSize = cv::getTextSize(text, fontFace, fontScale, thickness, &baseline);

  // Compute centered origin
  int x = rect.x;
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
cv::Mat PlotBase::buildColorLUT(ColormapName colorMap)
{
  int colormapType = cv::COLORMAP_VIRIDIS;
  cv::Mat customLUT(1, 256, CV_8UC3);
  bool useCustom = false;

  switch(colorMap) {
    case ColormapName::INFERNO:
      colormapType = cv::COLORMAP_INFERNO;
      break;
    case ColormapName::JET:
      colormapType = cv::COLORMAP_JET;
      break;
    case ColormapName::TURBO:
      colormapType = cv::COLORMAP_TURBO;
      break;
    case ColormapName::MAGMA:
      colormapType = cv::COLORMAP_MAGMA;
      break;
    case ColormapName::PARULA:
      colormapType = cv::COLORMAP_PARULA;
      break;
    case ColormapName::PLASMA:
      colormapType = cv::COLORMAP_PLASMA;
      break;
    case ColormapName::VIRIDIS:
      colormapType = cv::COLORMAP_VIRIDIS;
      break;
    case ColormapName::HSV:
      colormapType = cv::COLORMAP_HSV;
      break;
    case ColormapName::HOT:
      colormapType = cv::COLORMAP_HOT;
      break;
    case ColormapName::COOL:
      colormapType = cv::COLORMAP_COOL;
      break;
    case ColormapName::SPRING:
      colormapType = cv::COLORMAP_SPRING;
      break;
    case ColormapName::SUMMER:
      colormapType = cv::COLORMAP_SUMMER;
      break;
    case ColormapName::AUTUMN:
      colormapType = cv::COLORMAP_AUTUMN;
      break;
    case ColormapName::WINTER:
      colormapType = cv::COLORMAP_WINTER;
      break;
    case ColormapName::BONE:
      colormapType = cv::COLORMAP_BONE;
      break;
    case ColormapName::PINK:
      colormapType = cv::COLORMAP_PINK;
      break;
    case ColormapName::RAINBOW:
      colormapType = cv::COLORMAP_RAINBOW;
      break;
    case ColormapName::OCEAN:
      colormapType = cv::COLORMAP_OCEAN;
      break;
    case ColormapName::COLORMAP_CIVIDIS:
      colormapType = cv::COLORMAP_CIVIDIS;
      break;
    case ColormapName::COLORMAP_TWILIGHT:
      colormapType = cv::COLORMAP_TWILIGHT;
      break;
    case ColormapName::COLORMAP_TWILIGHT_SHIFTED:
      colormapType = cv::COLORMAP_TWILIGHT_SHIFTED;
      break;
    case ColormapName::COLORMAP_DEEPGREEN:
      colormapType = cv::COLORMAP_DEEPGREEN;
      break;
    case ColormapName::ACCENT:
    case ColormapName::BLUES:
    case ColormapName::BRBG:
    case ColormapName::BUGN:
    case ColormapName::BUPU:
    case ColormapName::CHROMAJS:
    case ColormapName::DARK2:
    case ColormapName::GNBU:
    case ColormapName::GNPU:
    case ColormapName::GREENS:
    case ColormapName::GREYS:
    case ColormapName::ORANGES:
    case ColormapName::ORRD:
    case ColormapName::PAIRED:
    case ColormapName::PASTEL1:
    case ColormapName::PASTEL2:
    case ColormapName::PIYG:
    case ColormapName::PRGN:
    case ColormapName::PUBU:
    case ColormapName::PUBUGN:
    case ColormapName::PUOR:
    case ColormapName::PURD:
    case ColormapName::PURPLES:
    case ColormapName::RDBU:
    case ColormapName::BURD:
    case ColormapName::RDGY:
    case ColormapName::RDPU:
    case ColormapName::RDYLBU:
    case ColormapName::RDYLGN:
    case ColormapName::REDS:
    case ColormapName::SAND:
    case ColormapName::SET1:
    case ColormapName::SET2:
    case ColormapName::SET3:
    case ColormapName::SPECTRAL:
    case ColormapName::WHYLRD:
    case ColormapName::YLGN:
    case ColormapName::YLGNBU:
    case ColormapName::YLORBR:
    case ColormapName::YLORRD:
    case ColormapName::YLRD:
    case ColormapName::GRAY:
    case ColormapName::COPPER:
    case ColormapName::LINES:
    case ColormapName::COLORCUBE:
    case ColormapName::PRISM:
    case ColormapName::FLAG:
    case ColormapName::WHITE:
    case ColormapName::DEFAULT_MAP:
    case ColormapName::DEFAULT_COLORS_MAP:
      break;

    case ColormapName::IMAGEC_GREEN_RED: {
      useCustom = true;
      for(int i = 0; i < 256; ++i) {
        cv::Vec3b color;
        if(i < 128) {
          color = cv::Vec3b(0, 255, static_cast<uchar>(2 * i));    // BGR order: Blue=0, Green=255, Red=0→255
        } else {
          color = cv::Vec3b(0, static_cast<uchar>(255 - 2 * (i - 128)), 255);
        }
        customLUT.at<cv::Vec3b>(0, i) = color;
      }
    }
  }

  cv::Mat lut(1, 256, CV_8UC1);    // grayscale ramp 0..255
  for(int i = 0; i < 256; ++i) {
    lut.at<uchar>(0, i) = static_cast<uchar>(i);
  }

  cv::Mat colorLUT;
  if(useCustom) {
    cv::Mat lutT = customLUT.t();    // Make it 256x1 instead of 1x256
    cv::applyColorMap(lut, colorLUT, lutT);
  } else {
    cv::applyColorMap(lut, colorLUT, colormapType);
  }

  return colorLUT;    // Returns a 1x256 CV_8UC3 BGR colormap
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Vec3b PlotBase::mapValueToColor(double value, double vmin, double vmax, const cv::Mat &colorLUT)
{
  if(value != value) {
    return {255, 255, 255};
  }
  // Normalize value to 0–255 index range
  double clamped = std::max(vmin, std::min(vmax, value));
  if(vmax - vmin == 0) {
    return {255, 255, 255};
  }
  int idx = static_cast<int>(255.0 * (clamped - vmin) / (vmax - vmin));
  if(colorLUT.cols > idx) {
    return colorLUT.at<cv::Vec3b>(0, idx);    // Return BGR color
  }

  return {255, 255, 255};
}

}    // namespace joda::plot
