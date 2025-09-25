///
/// \file      object_math.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include "backend/commands/command.hpp"
#include "backend/processor/context/process_context.hpp"
#include "enhance_contrast_settings.hpp"

namespace joda::cmd {

///
/// \class      EnhanceContrast
/// \author     Joachim Danmayr
/// \brief
///
class EnhanceContrast : public Command
{
public:
  /////////////////////////////////////////////////////
  EnhanceContrast(const settings::EnhanceContrastSettings &);
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override;
  static auto equalize(cv::Mat &histogram) -> std::array<int32_t, UINT16_MAX + 1>;
  static auto findContrastStretchBounds(const cv::Mat &hist, double percentage = 0.01) -> std::pair<int, int>;
  static void stretchHistogram(cv::Mat &ip, double saturated, cv::Mat &histogram, bool doNormalize);

private:
  /////////////////////////////////////////////////////
  const settings::EnhanceContrastSettings &mSettings;
};

}    // namespace joda::cmd
