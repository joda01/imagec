///
/// \file      pipeline_step.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include <memory>
#include "backend/commands/command.hpp"
#include "pipeline_factory.hpp"

namespace joda::settings {

void PipelineStep::operator()(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) const
{
  if(disabled) {
    return;
  }
  auto ret = PipelineFactory<joda::cmd::Command>::generate(*this);
  if(ret != nullptr) {
    ret->execute(context, image, result);
  }
}

void PipelineStep::check() const
{
}

}    // namespace joda::settings
