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
#include "measure_distance_settings.hpp"

namespace joda::cmd {

class MeasureDistance : public cmd::Command
{
public:
  explicit MeasureDistance(const settings::MeasureDistanceSettings &settings) : mSettings(settings)
  {
  }
  void execute(processor::ProcessContext &context, cv::Mat & /*image*/, atom::ObjectList &result) override;

private:
  const settings::MeasureDistanceSettings &mSettings;
};
}    // namespace joda::cmd
