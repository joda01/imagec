///
/// \file      pipeline_step.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include <memory>
#include "backend/commands/command.hpp"
#include "pipeline_factory.hpp"

namespace joda::settings {

void PipelineStep::operator()(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) const
{
  auto ret = PipelineFactory<joda::cmd::Command>::generate(*this);
  if(ret != nullptr) {
    ret->execute(context, image, result);
  }
}

}    // namespace joda::settings
