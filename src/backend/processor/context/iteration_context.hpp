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
#include <memory>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/enums/enum_image_cache.hpp"

namespace joda::processor {

class PipelineInitializer;

class IterationContext
{
  friend class ProcessContext;

public:
  explicit IterationContext(std::shared_ptr<joda::atom::ObjectList> &objectList) : actObjects(objectList)
  {
  }
  joda::atom::ObjectList &getObjects()
  {
    return *actObjects;
  }

private:
  /////////////////////////////////////////////////////
  enums::imageCache_t imageCache;
  std::shared_ptr<joda::atom::ObjectList> actObjects;
};

}    // namespace joda::processor
