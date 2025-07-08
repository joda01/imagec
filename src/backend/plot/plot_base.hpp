///
/// \file      plot_base.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <vector>
#include "backend/helper/table/table.hpp"
#include <opencv2/opencv.hpp>

namespace joda::plot {

enum class ColormapName
{
  ACCENT,
  BLUES,
  BRBG,
  BUGN,
  BUPU,
  CHROMAJS,
  DARK2,
  GNBU,
  GNPU,
  GREENS,
  GREYS,
  INFERNO,
  JET,
  TURBO,
  MAGMA,
  ORANGES,
  ORRD,
  PAIRED,
  PARULA,
  PASTEL1,
  PASTEL2,
  PIYG,
  PLASMA,
  PRGN,
  PUBU,
  PUBUGN,
  PUOR,
  PURD,
  PURPLES,
  RDBU,
  BURD,
  RDGY,
  RDPU,
  RDYLBU,
  RDYLGN,
  REDS,
  SAND,
  SET1,
  SET2,
  SET3,
  SPECTRAL,
  VIRIDIS,
  WHYLRD,
  YLGN,
  YLGNBU,
  YLORBR,
  YLORRD,
  YLRD,
  HSV,
  HOT,
  COOL,
  SPRING,
  SUMMER,
  AUTUMN,
  WINTER,
  GRAY,
  BONE,
  COPPER,
  PINK,
  LINES,
  COLORCUBE,
  PRISM,
  FLAG,
  WHITE,
  RAINBOW,
  OCEAN,
  COLORMAP_CIVIDIS,
  COLORMAP_TWILIGHT,
  COLORMAP_TWILIGHT_SHIFTED,
  COLORMAP_DEEPGREEN,
  DEFAULT_MAP,
  DEFAULT_COLORS_MAP
};

///
/// \class      PlotBase
/// \author     Joachim Danmayr
/// \brief      Base class for plotting
///
class PlotBase
{
public:
  struct Size
  {
    int width  = 0;
    int height = 0;
  };

  virtual auto plot(const Size &size) -> cv::Mat        = 0;
  virtual void setData(const joda::table::Table &&data) = 0;

  static cv::Mat buildColorLUT(ColormapName colorMap);
  static cv::Vec3b mapValueToColor(double value, double vmin, double vmax, const cv::Mat &colorLUT);
  static void drawCenteredText(cv::Mat &image, const std::string &text, const cv::Rect &rect, int fontFace = cv::FONT_HERSHEY_SIMPLEX,
                               double fontScale = 1.0, int thickness = 2, const cv::Scalar &textColor = cv::Scalar(0, 0, 0));
  static void drawLeftAlignedText(cv::Mat &image, const std::string &text, const cv::Rect &rect, int fontFace = cv::FONT_HERSHEY_SIMPLEX,
                                  double fontScale = 1.0, int thickness = 2, const cv::Scalar &textColor = cv::Scalar(0, 0, 0));
};

}    // namespace joda::plot
