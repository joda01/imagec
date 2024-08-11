

///
/// \file      processor_memory.cpp
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

#include "processor_memory.hpp"
#include "process_step.hpp"

namespace joda::processor {

void ProcessorMemory::store(joda::enums::Slot slot, const ProcessStep &element)
{
  mMemory.try_emplace(slot, element.context());
  mMemory.at(slot).cloneFrom(element);
}
auto ProcessorMemory::load(joda::enums::Slot slot) -> ProcessStep &
{
  return mMemory.at(slot);
}
void ProcessorMemory::loadCopy(joda::enums::Slot slot, ProcessStep &toLoadIn)
{
  toLoadIn.cloneFrom(mMemory.at(slot));
}
void ProcessorMemory::erase(joda::enums::Slot slot)
{
  mMemory.erase(slot);
}

}    // namespace joda::processor
