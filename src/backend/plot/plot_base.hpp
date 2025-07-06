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

  static cv::Mat buildColorLUT(int colormapType = cv::COLORMAP_JET);
  static cv::Vec3b mapValueToColor(float value, float vmin, float vmax, const cv::Mat &colorLUT);
  static void drawCenteredText(cv::Mat &image, const std::string &text, const cv::Rect &rect, int fontFace = cv::FONT_HERSHEY_SIMPLEX,
                               double fontScale = 1.0, int thickness = 2, const cv::Scalar &textColor = cv::Scalar(0, 0, 0));
};

}    // namespace joda::plot
