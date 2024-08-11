
///
/// \file      processor_memory.hpp
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

#pragma once

#include "backend/settings/anaylze_settings_enums.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::processor {

class ProcessStep;

class ProcessorMemory
{
public:
  ProcessorMemory()                   = default;
  ProcessorMemory(ProcessorMemory &)  = delete;
  ProcessorMemory(ProcessorMemory &&) = delete;

  /////////////////////////////////////////////////////
  void store(joda::settings::Slot, const ProcessStep &);
  auto load(joda::settings::Slot) -> ProcessStep &;
  void loadCopy(joda::settings::Slot slot, ProcessStep &toLoadIn);
  void erase(joda::settings::Slot);

private:
  /////////////////////////////////////////////////////
  std::map<joda::settings::Slot, ProcessStep> mMemory;
};
}    // namespace joda::processor
