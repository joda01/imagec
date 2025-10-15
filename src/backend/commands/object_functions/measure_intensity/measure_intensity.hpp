///
/// \file      measure.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "backend/commands/command.hpp"
#include "measure_intensity_settings.hpp"

namespace joda::cmd {

class MeasureIntensity : public cmd::Command
{
public:
  explicit MeasureIntensity(const settings::MeasureIntensitySettings &settings) : mSettings(settings)
  {
  }

  void execute(processor::ProcessContext &context, cv::Mat & /*image*/, atom::ObjectList & /*result*/) override
  {
    auto &store = *context.loadObjectsFromCache();
    for(auto imageId : mSettings.planesIn) {
      joda::atom::ImagePlane rgbPlane;
      auto const &image = *context.loadImageFromCache(enums::MemoryScope::ITERATION, imageId);
      if(image.isRgb()) {
        /// \todo Allow to specify which grayscale calculation mode should be used
        rgbPlane       = image;
        rgbPlane.image = cv::Mat::zeros(image.image.size(), CV_16UC1);
        for(int i = 0; i < image.image.rows; ++i) {
          for(int j = 0; j < image.image.cols; ++j) {
            cv::Vec3b pixel                   = image.image.at<cv::Vec3b>(i, j);
            uint16_t grayValue                = static_cast<uint16_t>(((pixel[2] + pixel[1] + pixel[0]) * 65535.0) / (3 * 255.0));    // BGR format
            rgbPlane.image.at<uint16_t>(i, j) = grayValue;
          }
        }
      }

      for(const auto &classIdIn : mSettings.inputClasses) {
        if(!store.contains(context.getClassId(classIdIn))) {
          continue;
        }
        auto *classsObjects = store.at(context.getClassId(classIdIn)).get();
        for(auto &object : *classsObjects) {
          if(context.getClassId(classIdIn) == object.getClassId()) {
            if(image.isRgb()) {
              object.measureIntensityAndAdd(rgbPlane.getId(), rgbPlane.image, context.getTileInfo());
            } else {
              object.measureIntensityAndAdd(image.getId(), rgbPlane.image, context.getTileInfo());
            }
          }
        }
      }
    }
  }

private:
  const settings::MeasureIntensitySettings &mSettings;
};
}    // namespace joda::cmd
