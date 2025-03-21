///
/// \file      object_math.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
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

private:
  /////////////////////////////////////////////////////
  const settings::EnhanceContrastSettings &mSettings;
};

}    // namespace joda::cmd
