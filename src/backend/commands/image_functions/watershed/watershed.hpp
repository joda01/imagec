///
/// \file      watershed.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <cstdint>
#include "backend/commands/command.hpp"
#include "backend/commands/image_functions/watershed/watershed_settings.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "edm.hpp"
#include "maximum_finder.hpp"

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Base class for an image processing function
/// \ref https://github.com/imagej/ImageJ/blob/master/ij/process/ShortBlitter.java#L14
///
class Watershed : public Command
{
public:
  /////////////////////////////////////////////////////
  explicit Watershed(const settings::WatershedSettings &settings) : mSettings(settings)
  {
  }
  virtual ~Watershed() = default;
  void execute(processor::ProcessContext & /*context*/, cv::Mat &image, atom::ObjectList & /*result*/) override
  {
    if(mSettings.maximumFinderTolerance <= 0) {
      return;
    }
    image.convertTo(image, CV_8UC1, 1.0 / 257.0);
    auto floatEdm = joda::image::func::Edm::makeFloatEDM(image, 0, false);
    joda::image::func::MaximumFinder find;
    auto maxIp = find.findMaxima(floatEdm, static_cast<double>(mSettings.maximumFinderTolerance), joda::image::func::MaximumFinder::NO_THRESHOLD,
                                 joda::image::func::MaximumFinder::SEGMENTED, false, true);
    cv::bitwise_and(maxIp, image, image);
    image.convertTo(image, CV_16UC1, static_cast<double>(UINT16_MAX) / static_cast<double>(UINT8_MAX));
  }

private:
  /////////////////////////////////////////////////////
  const settings::WatershedSettings &mSettings;
};
}    // namespace joda::cmd
