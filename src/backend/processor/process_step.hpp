
///
/// \file      process_step.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "backend/commands/command.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include <opencv2/core/mat.hpp>
#include "process_context.hpp"
#include "processor_memory.hpp"

namespace joda::processor {

class ProcessStep
{
public:
  /////////////////////////////////////////////////////
  ProcessStep(const ProcessContext &ctx) : mContext(ctx)
  {
  }
  ProcessStep &executeStep(ProcessorMemory &memory, const joda::settings::PipelineStep &pipelineStep);
  ProcessStep &executeStep(ProcessorMemory &memory, joda::cmd::Command &pipelineStep);
  void cloneFrom(const ProcessStep &)
  {
  }

  auto mutableContext() -> ProcessContext &
  {
    return mContext;
  }

  [[nodiscard]] auto context() const -> const ProcessContext &
  {
    return mContext;
  }

private:
  // RESULTS /////////////////////////////////
  cv::Mat mImages;
  joda::cmd::ObjectsListMap mResults;
  ProcessContext mContext;
};

}    // namespace joda::processor