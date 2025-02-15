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
#include "hough_transform_settings.hpp"

namespace joda::cmd {

///
/// \class      HoughTransform
/// \author     Joachim Danmayr
/// \brief
///
class HoughTransform : public Command
{
public:
  /////////////////////////////////////////////////////
  HoughTransform(const settings::HoughTransformSettings &);
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override;

private:
  /////////////////////////////////////////////////////
  const settings::HoughTransformSettings &mSettings;
};

}    // namespace joda::cmd
