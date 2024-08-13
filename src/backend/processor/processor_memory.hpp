
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

///

#pragma once

#include "backend/enums/enum_memory.hpp"
#include "backend/enums/enums_clusters.hpp"
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
  void store(joda::enums::MemoryId, const ProcessStep &);
  auto load(joda::enums::MemoryId) -> ProcessStep &;
  void loadCopy(joda::enums::MemoryId slot, ProcessStep &toLoadIn);
  void erase(joda::enums::MemoryId);

  void store(joda::enums::ClusterId, const ProcessStep &);
  auto load(joda::enums::ClusterId) -> ProcessStep &;
  void loadCopy(joda::enums::ClusterId slot, ProcessStep &toLoadIn);
  void erase(joda::enums::ClusterId);

private:
  /////////////////////////////////////////////////////
  std::map<joda::enums::MemoryId, ProcessStep> mMemory;
  std::map<joda::enums::ClusterId, ProcessStep> mChannels;
};
}    // namespace joda::processor
