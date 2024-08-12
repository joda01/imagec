///
/// \file      process_step.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "process_step.hpp"
#include "backend/commands/factory.hpp"

namespace joda::processor {

ProcessStep &ProcessStep::executeStep(ProcessorMemory &memory, joda::cmd::Command &pipelineStep)
{
  pipelineStep.execute(mContext, memory, mImages, mObjects);    // Execute the pipeline step
  return *this;
}

ProcessStep &ProcessStep::executeStep(ProcessorMemory &memory, const joda::settings::PipelineStep &pipelineStep)
{
  pipelineStep(mContext, memory, mImages, mObjects);    // Execute the pipeline step
  return *this;
}

}    // namespace joda::processor
