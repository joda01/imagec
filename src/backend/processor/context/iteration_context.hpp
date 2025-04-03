///
/// \file      interation_context.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <filesystem>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/enums/enum_image_cache.hpp"

namespace joda::processor {

class PipelineInitializer;

struct IterationContext
{
  friend class ProcessContext;

  enums::imageCache_t imageCache;

public:
  joda::atom::ObjectList &getObjects()
  {
    return actObjects;
  }

private:
  joda::atom::ObjectList actObjects{};
};

}    // namespace joda::processor
