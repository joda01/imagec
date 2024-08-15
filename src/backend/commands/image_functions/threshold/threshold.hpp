///
/// \file      threshold.hpp
/// \author    Joachim Danmayr
/// \date      2023-11-01
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <opencv2/core/hal/interface.h>
#include <algorithm>
#include <cstdint>
#include "backend/commands/command.hpp"
#include "backend/commands/image_functions/threshold/threshold_settings.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "threshold_li.hpp"
#include "threshold_min_error.hpp"
#include "threshold_moments.hpp"
#include "threshold_otsu.hpp"
#include "threshold_triangel.hpp"

namespace joda::cmd {

///
/// \class      Threshold
/// \author     Joachim Danmayr
/// \brief      Base class for thershold calculation
///
class Threshold : public Command
{
public:
  explicit Threshold(const settings::ThresholdSettings &settings) : mSettings(settings)
  {
  }
  virtual ~Threshold() = default;

  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override
  {
    auto [thresholdValMin, thresholdValMax] = autoThreshold(image);
    cv::Mat thresholdImg(image.size(), CV_16UC1);
    cv::threshold(image, thresholdImg, thresholdValMin, UINT16_MAX, cv::THRESH_BINARY);
    cv::Mat thresholdTmp;
    cv::threshold(image, thresholdTmp, thresholdValMax, UINT16_MAX, cv::THRESH_BINARY_INV);
    cv::bitwise_and(thresholdImg, thresholdTmp, thresholdImg);
    image                                                     = std::move(thresholdImg);
    context.pipelineContext.actImagePlane.appliedMinThreshold = thresholdValMin;
    context.pipelineContext.actImagePlane.appliedMaxThreshold = thresholdValMax;
  }

protected:
  /////////////////////////////////////////////////////
  [[nodiscard]] uint16_t getMinThreshold() const
  {
    return mSettings.thresholdMin;
  }

  [[nodiscard]] uint16_t getMaxThreshold() const
  {
    return mSettings.thresholdMax;
  }

private:
  /////////////////////////////////////////////////////
  [[nodiscard]] uint16_t calcThresholdValue(cv::Mat &histogram) const
  {
    switch(mSettings.mode) {
      case settings::ThresholdSettings::Mode::NONE:
      case settings::ThresholdSettings::Mode::MANUAL:
        return getMinThreshold();
      case settings::ThresholdSettings::Mode::LI:
        return ThresholdLi::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Mode::MIN_ERROR:
        return ThresholdMinError::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Mode::TRIANGLE:
        return ThresholdTriangle::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Mode::MOMENTS:
        return ThresholdMoments::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Mode::OTSU:
        return ThresholdOtsu::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Mode::HUANG:
      case settings::ThresholdSettings::Mode::INTERMODES:
      case settings::ThresholdSettings::Mode::ISODATA:
      case settings::ThresholdSettings::Mode::MAX_ENTROPY:
      case settings::ThresholdSettings::Mode::MEAN:
      case settings::ThresholdSettings::Mode::MINIMUM:
      case settings::ThresholdSettings::Mode::PERCENTILE:
      case settings::ThresholdSettings::Mode::RENYI_ENTROPY:
      case settings::ThresholdSettings::Mode::SHANBHAG:
      case settings::ThresholdSettings::Mode::YEN:
        joda::log::logWarning("Selected threshold not supported!");
        break;
    }
    return getMinThreshold();
  }

  [[nodiscard]] virtual std::tuple<uint16_t, uint16_t> autoThreshold(const cv::Mat &srcImg) const
  {
    //
    // Scale image
    //
    double min;
    double max;
    cv::Point min_loc;
    cv::Point max_loc;
    // Find the minimum and maximum values and their locations
    cv::minMaxLoc(srcImg, &min, &max, &min_loc, &max_loc);
    double scale    = 256.0 / (max - min + 1);
    cv::Mat charImg = cv::Mat::zeros(srcImg.rows, srcImg.cols, CV_8UC1);
    // srcImg.convertTo(charImg, CV_8UC1, scale);
    for(int i = 0; i < srcImg.total(); i++) {
      auto value = (srcImg.at<uint16_t>(i) & 0xffff) - min;
      if(value < 0) {
        value = 0;
      }
      value = (int) (value * scale + 0.5);
      if(value > 255) {
        value = 255;
      }
      charImg.at<uint8_t>(i) = (uint8_t) value;
    }

    // Calculate the histogram of the image
    int histSize           = UINT8_MAX + 1;    // Number of bins
    float range[]          = {0, 256};         // Pixel value range
    const float *histRange = {range};
    cv::Mat histogram;
    cv::calcHist(&charImg, 1, 0, cv::Mat(), histogram, 1, &histSize, &histRange);
    // histogram.at<float>(0) = 0;

    auto thresholdTempMin = scaleAndSetThreshold(0, calcThresholdValue(histogram) + 1, min, max);

    return {std::min(std::max(getMinThreshold(), thresholdTempMin), getMaxThreshold()), getMaxThreshold()};
  }

  /////////////////////////////////////////////////////

  ///
  /// \ref https://imagej.net/ij/developer/source/ij/process/ImageProcessor.java.html
  ///
  [[nodiscard]] uint16_t scaleAndSetThreshold(double lower, double upper, double min, double max) const
  {
    if(max > min) {
      if(lower == 0.0) {
      } else {
        lower = min + (lower / 255.0) * (max - min);
      }
      if(upper == 255.0) {
        upper = 65535;
      } else {
        upper = min + (upper / 255.0) * (max - min);
      }
    } else {
      lower = upper = min;
    }
    return upper;
  }

  /////////////////////////////////////////////////////
  const settings::ThresholdSettings &mSettings;
};

}    // namespace joda::cmd
