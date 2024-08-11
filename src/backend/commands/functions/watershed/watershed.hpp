///
/// \file      watershed.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <cstdint>
#include "backend/commands/command.hpp"
#include "backend/commands/functions/watershed/watershed_settings.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "edm.hpp"
#include "maximum_finder.hpp"

namespace joda::cmd::functions {

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
  explicit Watershed(const WatershedSettings &settings) : mSettings(settings)
  {
  }
  virtual ~Watershed() = default;
  void execute(processor::ProcessContext &context, cv::Mat &image, ObjectsListMap &result) override
  {
    auto idStart = DurationCount::start("Watershed");
    image.convertTo(image, CV_8UC1, 1.0F / 257.0F);
    auto floatEdm = joda::image::func::Edm::makeFloatEDM(image, 0, false);
    joda::image::func::MaximumFinder find;
    auto maxIp =
        find.findMaxima(floatEdm, mSettings.maximumFinderTolerance, joda::image::func::MaximumFinder::NO_THRESHOLD,
                        joda::image::func::MaximumFinder::SEGMENTED, false, true);
    cv::bitwise_and(maxIp, image, image);
    image.convertTo(image, CV_16UC1, (float) UINT16_MAX / (float) UINT8_MAX);
    DurationCount::stop(idStart);
  }

private:
  /////////////////////////////////////////////////////
  const WatershedSettings &mSettings;
};
}    // namespace joda::cmd::functions
