
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

#include "backend/global_enums.hpp"
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
  void store(joda::enums::Slot, const ProcessStep &);
  auto load(joda::enums::Slot) -> ProcessStep &;
  void loadCopy(joda::enums::Slot slot, ProcessStep &toLoadIn);
  void erase(joda::enums::Slot);

private:
  /////////////////////////////////////////////////////
  std::map<joda::enums::Slot, ProcessStep> mMemory;
};
}    // namespace joda::processor
