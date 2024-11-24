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
#include "validator_noise_settings.hpp"

namespace joda::cmd {

class NoiseValidator : public Command
{
public:
  NoiseValidator(const settings::NoiseValidatorSettings &settings) : mSettings(settings)
  {
  }

  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override
  {
    uint64_t count = mSettings.maxObjects;

    for(const auto &classes : mSettings.inputClasses) {
      auto markAsInvalid = [this, &context, classes]() {
        switch(mSettings.mode) {
          case settings::NoiseValidatorSettings::FilterMode::UNKNOWN:
          case settings::NoiseValidatorSettings::FilterMode::INVALIDATE_IMAGE:
            context.setImageValidity(joda::enums::ChannelValidityEnum::POSSIBLE_WRONG_THRESHOLD);
            break;
          case settings::NoiseValidatorSettings::FilterMode::INVALIDATE_IMAGE_PLANE:
            context.setImagePlaneValidity(joda::enums::ChannelValidityEnum::POSSIBLE_WRONG_THRESHOLD);
            break;
          case settings::NoiseValidatorSettings::FilterMode::INVALIDATE_IAMGE_PLANE_CLASS:
            context.setImagePlaneClasssClasssValidity(classes, joda::enums::ChannelValidityEnum::POSSIBLE_WRONG_THRESHOLD);
            break;
        }
      };

      const auto *classs = result.at(context.getClassId(classes)).get();
      for(const auto &roi : *classs) {
        if(roi.getClassId() == context.getClassId(classes)) {
          if(count > 0) {
            count--;
          } else {
            // Too many objects
            markAsInvalid();
            break;
          }
        }
      }
    }
  }

  /////////////////////////////////////////////////////
  const settings::NoiseValidatorSettings &mSettings;
};

}    // namespace joda::cmd
