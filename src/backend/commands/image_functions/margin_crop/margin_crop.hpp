///
/// \file      margin_crop.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "backend/commands/command.hpp"
#include <opencv2/core/mat.hpp>
#include "margin_crop_settings.hpp"

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Base class for an image processing function
///
class MarginCrop : public cmd::Command
{
public:
  /////////////////////////////////////////////////////
  explicit MarginCrop(const settings::MarginCropSettings &setting) : mMarginSize(setting.marginSize)
  {
  }
  void execute(processor::ProcessContext & /*context*/, cv::Mat &image, atom::ObjectList & /*result*/) override
  {
    // Calculate the new dimensions for the cropped image
    int newWidth  = image.cols - mMarginSize * 2;    // x pixels from each side
    int newHeight = image.rows - mMarginSize * 2;    // x pixels from each side

    // Create a rectangle to define the region of interest (ROI)
    cv::Rect roi(mMarginSize, mMarginSize, newWidth, newHeight);

    // Crop the image using the ROI
    image = image(roi);
  }

private:
  /////////////////////////////////////////////////////
  int mMarginSize;
};

}    // namespace joda::cmd
