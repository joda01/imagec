///
/// \file      threshold.hpp
/// \author    Joachim Danmayr
/// \date      2023-11-01
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <opencv2/core/hal/interface.h>
#include <algorithm>
#include <cstdint>
#include "backend/commands/command.hpp"
#include "backend/commands/image_functions/threshold/threshold_huang.hpp"
#include "backend/commands/image_functions/threshold/threshold_intermodes.hpp"
#include "backend/commands/image_functions/threshold/threshold_isodata.hpp"
#include "backend/commands/image_functions/threshold/threshold_max_entropy.hpp"
#include "backend/commands/image_functions/threshold/threshold_mean.hpp"
#include "backend/commands/image_functions/threshold/threshold_minimum.hpp"
#include "backend/commands/image_functions/threshold/threshold_percentile.hpp"
#include "backend/commands/image_functions/threshold/threshold_renyi_entropy.hpp"
#include "backend/commands/image_functions/threshold/threshold_settings.hpp"
#include "backend/commands/image_functions/threshold/threshold_shanbhag.hpp"
#include "backend/commands/image_functions/threshold/threshold_yen.hpp"
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
    cv::Mat outputImage = cv::Mat::zeros(image.size(), CV_16UC1);

    for(const auto &threshold : mSettings.modelClasses) {
      auto [thresholdValMin, thresholdValMax] = autoThreshold(threshold, image);
      cv::Mat thresholdImg(image.size(), CV_16UC1);
      cv::threshold(image, thresholdImg, thresholdValMin, UINT16_MAX, cv::THRESH_BINARY);
      cv::Mat thresholdTmp;
      cv::threshold(image, thresholdTmp, thresholdValMax, UINT16_MAX, cv::THRESH_BINARY_INV);
      cv::bitwise_and(thresholdImg, thresholdTmp, thresholdImg);
      thresholdImg.setTo(threshold.modelClassId, thresholdImg > 0);
      context.setBinaryImage(thresholdValMin, thresholdValMax);
      outputImage = cv::max(outputImage, thresholdImg);
    }

    image = std::move(outputImage);
  }

private:
  /////////////////////////////////////////////////////
  [[nodiscard]] uint16_t calcThresholdValue(const settings::ThresholdSettings::Threshold &settings, cv::Mat &histogram) const
  {
    switch(settings.method) {
      case settings::ThresholdSettings::Methods::NONE:
      case settings::ThresholdSettings::Methods::MANUAL:
        return settings.thresholdMin;
      case settings::ThresholdSettings::Methods::LI:
        return ThresholdLi::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Methods::MIN_ERROR:
        return ThresholdMinError::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Methods::TRIANGLE:
        return ThresholdTriangle::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Methods::MOMENTS:
        return ThresholdMoments::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Methods::OTSU:
        return ThresholdOtsu::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Methods::MEAN:
        return ThresholdMean::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Methods::SHANBHAG:
        return ThresholdShanbhag::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Methods::HUANG:
        return ThresholdHuang::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Methods::INTERMODES:
        return ThresholdIntermodes::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Methods::ISODATA:
        return ThresholdIsoData::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Methods::MAX_ENTROPY:
        return ThresholdMaxEntropy::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Methods::MINIMUM:
        return ThresholdMinimum::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Methods::PERCENTILE:
        return ThresholdPercentile::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Methods::RENYI_ENTROPY:
        return ThresholdRenyiEntropy::calcThresholdValue(histogram);
      case settings::ThresholdSettings::Methods::YEN:
        return ThresholdYen::calcThresholdValue(histogram);
    }
    return settings.thresholdMin;
  }

  [[nodiscard]] virtual std::tuple<uint16_t, uint16_t> autoThreshold(const settings::ThresholdSettings::Threshold &settings,
                                                                     const cv::Mat &srcImg) const
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
    for(int i = 0; i < static_cast<int32_t>(srcImg.total()); i++) {
      auto value = (srcImg.at<uint16_t>(i) & 0xffff) - min;
      if(value < 0) {
        value = 0;
      }
      value = static_cast<int>(std::lround(static_cast<double>(value) * scale));    // static_cast<int>(static_cast<double>(value) * scale + 0.5);
      if(value > 255) {
        value = 255;
      }
      charImg.at<uint8_t>(i) = static_cast<uint8_t>(value);
    }

    // Calculate the histogram of the image
    int histSize           = UINT8_MAX + 1;     // Number of bins
    float range[]          = {0.0F, 256.0F};    // Pixel value range
    const float *histRange = {range};
    cv::Mat histogram;
    cv::calcHist(&charImg, 1, nullptr, cv::Mat(), histogram, 1, &histSize, &histRange);
    // histogram.at<float>(0) = 0;

    uint16_t thresholdTempMin = settings.thresholdMin;
    if(settings.method != settings::ThresholdSettings::Methods::MANUAL && settings.method != settings::ThresholdSettings::Methods::NONE) {
      thresholdTempMin = scaleAndSetThreshold(0, calcThresholdValue(settings, histogram) + 1 + settings.cValue, min, max);
    }

    return {std::min(std::max(settings.thresholdMin, thresholdTempMin), settings.thresholdMax), settings.thresholdMax};
  }

  /////////////////////////////////////////////////////

  ///
  /// \ref https://imagej.net/ij/developer/source/ij/process/ImageProcessor.java.html
  ///
  [[nodiscard]] static uint16_t scaleAndSetThreshold(double lower, double upper, double min, double max)
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
    return static_cast<uint16_t>(upper);
  }

  /////////////////////////////////////////////////////
  const settings::ThresholdSettings &mSettings;
};

}    // namespace joda::cmd
