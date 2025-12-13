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
#include "backend/helper/fnv1a.hpp"

namespace joda::processor {

class PipelineInitializer;

class IterationContext
{
  friend class ProcessContext;

public:
  explicit IterationContext(std::shared_ptr<joda::atom::ObjectList> &objectList, const std::filesystem::path &projectPath,
                            const std::filesystem::path &imagePath) :
      actObjects(objectList)
  {
    auto storagePath = joda::helper::generateImageMetaDataStoragePathFromImagePath(imagePath, projectPath,
                                                                                   joda::fs::FILE_NAME_ANNOTATIONS + joda::fs::EXT_ANNOTATION);
    objectList->deserialize(storagePath);
  }
  joda::atom::ObjectList &getObjects()
  {
    return *actObjects;
  }

  void removeTemporaryObjects(ProcessContext *context);

private:
  /////////////////////////////////////////////////////
  enums::imageCache_t imageCache;
  std::shared_ptr<joda::atom::ObjectList> actObjects;
};

}    // namespace joda::processor
