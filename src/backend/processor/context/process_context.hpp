
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
///

#pragma once

#include <atomic>
#include <filesystem>
#include <map>
#include <memory>
#include "backend/artifacts/image/image.hpp"
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/enums/types.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include "image_context.hpp"
#include "iteration_context.hpp"
#include "pipeline_context.hpp"

namespace joda::processor {

using objectCache_t = std::map<enums::ObjectStoreId, std::unique_ptr<joda::atom::ObjectList>>;

struct GlobalContext
{
  friend class ProcessContext;

  std::filesystem::path resultsOutputFolder;
  uint64_t nextObjectId()
  {
    return globalObjectIdCount++;
  }
  uint64_t actObjectId()
  {
    return globalObjectIdCount;
  }

  db::Database database;

private:
  objectCache_t objectCache;
  std::atomic<uint64_t> globalObjectIdCount;
};

struct ProcessContext
{
  GlobalContext &globalContext;
  ImageContext &imageContext;
  IterationContext &iterationContext;
  PipelineContext pipelineContext;
  [[nodiscard]] joda::atom::ImagePlane &getActImage()
  {
    return pipelineContext.actImagePlane;
  }

  void setActImage(const joda::atom::ImagePlane *image)
  {
    pipelineContext.actImagePlane = *image;
  }

  [[nodiscard]] joda::atom::ObjectList &getActObjects()
  {
    return iterationContext.actObjects;
  }

  [[nodiscard]] const joda::enums::PlaneId &getActIterator() const
  {
    return pipelineContext.actImagePlane.getId().imagePlane;
  }

  bool doesImageInCacheExist(joda::enums::ImageId cacheId) const
  {
    getCorrectIteration(cacheId.imagePlane);
    return imageContext.imageCache.contains(cacheId);
  }

  joda::atom::ImagePlane *addImageToCache(joda::enums::ImageId cacheId, std::unique_ptr<joda::atom::ImagePlane> img)
  {
    getCorrectIteration(cacheId.imagePlane);
    return imageContext.imageCache.try_emplace(cacheId, std::move(img)).first->second.get();
  }

  [[nodiscard]] const joda::atom::ImagePlane *loadImageFromCache(joda::enums::ImageId cacheId);
  void storeImageToCache(joda::enums::ImageId cacheId, const joda::atom::ImagePlane &image) const
  {
    getCorrectIteration(cacheId.imagePlane);
    imageContext.imageCache.try_emplace(cacheId, ::std::make_unique<joda::atom::ImagePlane>(image));
  }

  [[nodiscard]] joda::atom::ObjectList *loadObjectsFromCache(joda::enums::ObjectStoreId cacheId) const
  {
    if(cacheId.storeIdx == enums::MemoryIdx::M0) {
      return &iterationContext.actObjects;
    }
    getCorrectObjectId(cacheId);
    return globalContext.objectCache.at(cacheId).get();
  }

  // void storeObjectsToCache(joda::enums::ObjectStoreId cacheId, const joda::atom::ObjectList &object) const
  //{
  // #warning "IF cluster ID still exists. Merge or override!?"
  //   getCorrectObjectId(cacheId);
  //
  //  auto oby = ::std::make_unique<joda::atom::ObjectList>();
  //  for(const auto &[key, element] : object) {
  //    oby->operator[](key).cloneFromOther(element);
  //  }
  //
  //  globalContext.objectCache.try_emplace(cacheId, std::move(oby));
  //}

  [[nodiscard]] cv::Size getImageSize() const
  {
    return pipelineContext.actImagePlane.image.size();
  }

  [[nodiscard]] uint64_t acquireNextObjectId() const
  {
    return globalContext.nextObjectId();
  }

  [[nodiscard]] enums::ClusterId getClusterId(enums::ClusterIdIn in) const
  {
    return in != enums::ClusterIdIn::$ ? static_cast<enums::ClusterId>(in) : pipelineContext.defaultClusterId;
  }

  [[nodiscard]] enums::ClassId getClassId(enums::ClassId in) const
  {
    return static_cast<enums::ClassId>(in);
  }

  ///
  /// \brief      Returns a corrected iterator. Every value < 0 is interpreted as THIS and
  ///             is replaced by the actual index of the process step
  /// \author     Joachim Danmayr
  ///
  void getCorrectObjectId(joda::enums::ObjectStoreId &cacheId) const
  {
    getCorrectIteration(cacheId.imagePlane);
  }

  ///
  /// \brief      Returns a corrected iterator. Every value < 0 is interpreted as THIS and
  ///             is replaced by the actual index of the process step
  /// \author     Joachim Danmayr
  ///
  void getCorrectIteration(joda::enums::PlaneId &imagePlane) const
  {
    const auto &actImageId = getActIterator();
    if(imagePlane.cStack < 0) {
      imagePlane.cStack = actImageId.cStack;
    }
    if(imagePlane.tStack < 0) {
      imagePlane.tStack = actImageId.tStack;
    }
    if(imagePlane.zStack < 0) {
      imagePlane.zStack = actImageId.zStack;
    }
  }
};

}    // namespace joda::processor
