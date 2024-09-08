///
/// \file      validator.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include "backend/commands/command.hpp"
#include "backend/enums/enum_validity.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "validator_threshold_settings.hpp"

namespace joda::cmd {

class ThresholdValidator : public Command
{
public:
  ThresholdValidator(const settings::ThresholdValidatorSettings &settings) : mSettings(settings)
  {
  }

  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override
  {
  }

private:
  ///
  /// \brief If the min threshold is lower than the value at the maximum
  ///        of the histogram multiplied with this factor the filter
  ///        will be applied.
  ///        Example: Min Threshold = 50
  ///                 Hist. Max is at 40
  ///                 histMinThresholdFilterFactor = 1.3  --> HistogramFilterThreshold = 65 --> Filter will be applied
  ///
  ///        Example: Min Threshold = 50
  ///                 Hist. Max is at 10
  ///                 histMinThresholdFilterFactor = 1.3  --> HistogramFilterThreshold = 13 --> Filter will not be
  ///                 applied
  /// \author Joachim Danmayr
  ///
  inline void applyHistogramFilter(processor::ProcessContext &context, cv::Mat & /*image*/, atom::ObjectList &result)
  {
    const auto &imageOriginal  = *context.loadImageFromCache(mSettings.imageIn);
    const auto &imageThreshold = context.getActImage();

    if(!imageThreshold.isBinary) {
      THROW_ERROR("Histogram filter can only be applied on threshold (binary) image!");
    }

    if(mSettings.histMinThresholdFilterFactor > 0) {
      int histSize           = UINT16_MAX + 1;         // Number of bins
      float range[]          = {0, UINT16_MAX + 1};    // Pixel value range
      const float *histRange = {range};
      cv::Mat histogram;
      cv::calcHist(&imageOriginal.image, 1, 0, cv::Mat(), histogram, 1, &histSize, &histRange);

      double maxVal = 0;
      int maxIdx    = -1;
      cv::minMaxIdx(histogram, NULL, &maxVal, NULL, &maxIdx);

      float filterThreshold = static_cast<float>(maxIdx) * mSettings.histMinThresholdFilterFactor;
      if(static_cast<float>(imageThreshold.appliedMinThreshold) < filterThreshold) {
        // Mark image or cluster as invalid
        enums::ChannelValidity validity;
        validity.set(joda::enums::ChannelValidityEnum::POSSIBLE_WRONG_THRESHOLD);
        switch(mSettings.mode) {
          case settings::ThresholdValidatorSettings::FilterMode::UNKNOWN:
          case settings::ThresholdValidatorSettings::FilterMode::INVALIDATE_IMAGE:
            context.setImageValidity(joda::enums::ChannelValidityEnum::POSSIBLE_WRONG_THRESHOLD);
            break;
          case settings::ThresholdValidatorSettings::FilterMode::INVALIDATE_IMAGE_PLANE:
            context.setImagePlaneValidity(joda::enums::ChannelValidityEnum::POSSIBLE_WRONG_THRESHOLD);
            break;
          case settings::ThresholdValidatorSettings::FilterMode::INVALIDATE_IAMGE_PLANE_CLUSTER:
            context.setImagePlaneClusterClusterValidity(mSettings.inputCluster,
                                                        joda::enums::ChannelValidityEnum::POSSIBLE_WRONG_THRESHOLD);
            break;
        }
      }

      TRACE("Hist idx: " + std::to_string(maxIdx) + " | " + std::to_string(filterThreshold));
    }
  }

  /////////////////////////////////////////////////////
  const settings::ThresholdValidatorSettings &mSettings;
};

}    // namespace joda::cmd
