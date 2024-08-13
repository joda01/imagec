
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
#include "backend/helper/ome_parser/ome_info.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

namespace joda::processor {

using imageCache_t  = std::map<enums::ImageId, std::unique_ptr<joda::atom::Image>>;
using objectCache_t = std::map<enums::ObjectId, std::unique_ptr<joda::atom::ObjectList>>;

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

private:
  imageCache_t imageCache;
  objectCache_t objectCache;
  std::atomic<uint64_t> globalObjectIdCount;
  // std::shared_ptr<joda::db::Database> database;
};

struct ImageContext
{
  std::filesystem::path imagePath;
  joda::ome::OmeInfo imageMeta;
};

struct PipelineContext
{
  joda::atom::Image actImage;
  joda::atom::ObjectList actObjects{};
  enums::ClusterId defaultClusterId;
};

struct ProcessContext
{
  GlobalContext &globalContext;
  ImageContext &imageContext;
  PipelineContext pipelineContext;
  [[nodiscard]] atom::Image &getActImage()
  {
    return pipelineContext.actImage;
  }

  [[nodiscard]] joda::atom::ObjectList &getActObjects()
  {
    return pipelineContext.actObjects;
  }

  [[nodiscard]] const joda::enums::IteratorId &getActIterator() const
  {
    return pipelineContext.actImage.getId().iteration;
  }

#warning "Handle this imageIdx"
  joda::atom::Image *addImageToCache(joda::enums::ImageId cacheId, std::unique_ptr<joda::atom::Image> img)
  {
    getCorrectIteration(cacheId.iteration);
    return globalContext.imageCache.try_emplace(cacheId, std::move(img)).first->second.get();
  }

  [[nodiscard]] const joda::atom::Image *loadImageFromCache(joda::enums::ImageId cacheId) const
  {
    getCorrectIteration(cacheId.iteration);
    return globalContext.imageCache.at(cacheId).get();
  }

  [[nodiscard]] const joda::atom::ObjectList *loadObjectsFromCache(joda::enums::ObjectId cacheId) const
  {
    getCorrectObjectId(cacheId);
    return globalContext.objectCache.at(cacheId).get();
  }

  void storeImageToCache(joda::enums::ImageId cacheId, const joda::atom::Image &image) const
  {
    getCorrectIteration(cacheId.iteration);
    globalContext.imageCache.try_emplace(cacheId, ::std::make_unique<joda::atom::Image>(image));
  }

  void storeObjectsToCache(joda::enums::ObjectId cacheId, const joda::atom::ObjectList &object) const
  {
    getCorrectObjectId(cacheId);
    globalContext.objectCache.try_emplace(cacheId, ::std::make_unique<joda::atom::ObjectList>());
    globalContext.objectCache.at(cacheId)->cloneFromOther(object);
  }

  [[nodiscard]] cv::Size getImageSize() const
  {
    return pipelineContext.actImage.image.size();
  }

  [[nodiscard]] uint64_t acquireNextObjectId() const
  {
    return globalContext.nextObjectId();
  }

  [[nodiscard]] enums::ClusterId getClusterId(enums::ClusterId in) const
  {
    return in != enums::ClusterId::$ ? in : pipelineContext.defaultClusterId;
  }

  [[nodiscard]] enums::ClassId getClassId(enums::ClassId in) const
  {
    return in != enums::ClassId::$ ? in : enums::ClassId::NONE;
  }

  ///
  /// \brief      Returns a corrected iterator. Every value < 0 is interpreted as THIS and
  ///             is replaced by the actual index of the process step
  /// \author     Joachim Danmayr
  ///
  void getCorrectObjectId(joda::enums::ObjectId &cacheId) const
  {
    if(cacheId.clusterId == enums::ClusterId::$) {
      cacheId.clusterId = pipelineContext.defaultClusterId;
    }
    getCorrectIteration(cacheId.iteration);
  }

  ///
  /// \brief      Returns a corrected iterator. Every value < 0 is interpreted as THIS and
  ///             is replaced by the actual index of the process step
  /// \author     Joachim Danmayr
  ///
  void getCorrectIteration(joda::enums::IteratorId &iteration) const
  {
    const auto &actImageId = getActIterator();
    if(iteration.cStack < 0) {
      iteration.cStack = actImageId.cStack;
    }
    if(iteration.tStack < 0) {
      iteration.tStack = actImageId.tStack;
    }
    if(iteration.zStack < 0) {
      iteration.zStack = actImageId.zStack;
    }
  }
};

}    // namespace joda::processor
