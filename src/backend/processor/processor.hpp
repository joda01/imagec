
///
/// \file      processor.hpp
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

#include "backend/settings/analze_settings.hpp"
#include "backend/settings/anaylze_settings_enums.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include <opencv2/core/mat.hpp>
#include "process_step.hpp"
#include "processor_memory.hpp"

namespace joda::processor {

class Processor
{
public:
  /////////////////////////////////////////////////////
  Processor();
  void execute(const joda::settings::AnalyzeSettings &program);

private:
  /////////////////////////////////////////////////////
  ProcessorMemory mMemory;
};
}    // namespace joda::processor
