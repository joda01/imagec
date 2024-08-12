

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

///

#include "processor_memory.hpp"
#include "process_step.hpp"

namespace joda::processor {

void ProcessorMemory::store(joda::enums::MemoryId slot, const ProcessStep &element)
{
  mMemory.try_emplace(slot, element.context());
  mMemory.at(slot).cloneFrom(element);
}
auto ProcessorMemory::load(joda::enums::MemoryId slot) -> ProcessStep &
{
  return mMemory.at(slot);
}
void ProcessorMemory::loadCopy(joda::enums::MemoryId slot, ProcessStep &toLoadIn)
{
  toLoadIn.cloneFrom(mMemory.at(slot));
}
void ProcessorMemory::erase(joda::enums::MemoryId slot)
{
  mMemory.erase(slot);
}

void ProcessorMemory::store(joda::enums::ChannelId slot, const ProcessStep &element)
{
  mChannels.try_emplace(slot, element.context());
  mChannels.at(slot).cloneFrom(element);
}
auto ProcessorMemory::load(joda::enums::ChannelId slot) -> ProcessStep &
{
  return mChannels.at(slot);
}
void ProcessorMemory::loadCopy(joda::enums::ChannelId slot, ProcessStep &toLoadIn)
{
  toLoadIn.cloneFrom(mChannels.at(slot));
}
void ProcessorMemory::erase(joda::enums::ChannelId slot)
{
  mChannels.erase(slot);
}

}    // namespace joda::processor
