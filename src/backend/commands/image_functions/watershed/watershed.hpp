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
#include <opencv2/imgcodecs.hpp>
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
  void execute(processor::ProcessContext & /*context*/, cv::Mat &imageIn, atom::ObjectList & /*result*/) override
  {
    if(mSettings.maximumFinderTolerance <= 0) {
      return;
    }

    // ==========================
    // Prepare input image
    // ===========================
    cv::Mat binary8(imageIn.size(), CV_8UC1);
    binary8 = (imageIn > 0);
    binary8 *= 255;

    // ==========================
    // Apply watershed
    // ===========================
    auto floatEdm = joda::image::func::Edm::makeFloatEDM(binary8, 0, false);
    joda::image::func::MaximumFinder find;
    auto maxIp = find.findMaxima(floatEdm, static_cast<double>(mSettings.maximumFinderTolerance), joda::image::func::MaximumFinder::NO_THRESHOLD,
                                 joda::image::func::MaximumFinder::SEGMENTED, false, true);

    // ==========================
    // Apply mask
    // ===========================
    for(int y = 0; y < maxIp.rows; y++) {
      const uint8_t *p1 = maxIp.ptr<uint8_t>(y);       // or uchar*, depends on type
      uint16_t *p2      = imageIn.ptr<uint16_t>(y);    // adjust type as needed
      for(int x = 0; x < maxIp.cols; x++) {
        if(p1[x] == 0) {
          p2[x] = 0;
        }
      }
    }
  }

private:
  /////////////////////////////////////////////////////
  const settings::WatershedSettings &mSettings;
};
}    // namespace joda::cmd
