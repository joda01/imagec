
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
#include "backend/enums/enum_validity.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/enums/types.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include "backend/processor/context/plate_context.hpp"
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
  db::Database database;

private:
  objectCache_t objectCache;
};

class ProcessContext
{
public:
  ProcessContext(GlobalContext &globalContext, PlateContext &plateContext, ImageContext &imageContext, IterationContext &iterationContext);

  void initDefaultSettings(enums::ClusterId cluster, enums::ZProjection zProjection)
  {
    pipelineContext.defaultClusterId   = cluster;
    pipelineContext.defaultZProjection = zProjection;
  }

  void setBinaryImage(uint16_t thresholdMin, uint16_t thresholdMax)
  {
    pipelineContext.actImagePlane.isBinary            = true;
    pipelineContext.actImagePlane.appliedMinThreshold = thresholdMin;
    pipelineContext.actImagePlane.appliedMaxThreshold = thresholdMax;
  }

  [[nodiscard]] joda::atom::ImagePlane &getActImage()
  {
    return pipelineContext.actImagePlane;
  }

  [[nodiscard]] enums::PlaneId getActImagePlaneId() const
  {
    return pipelineContext.actImagePlane.getId().imagePlane;
  }
  [[nodiscard]] const std::filesystem::path &getActImagePath() const
  {
    return imageContext.imagePath;
  }

  [[nodiscard]] const std::filesystem::path &getOutputFolder() const
  {
    return globalContext.resultsOutputFolder;
  }

  [[nodiscard]] enums::tile_t getActTile() const
  {
    return pipelineContext.actImagePlane.tile;
  }

  [[nodiscard]] cv::Size getTileSize() const
  {
    return imageContext.tileSize;
  }

  void setActImage(const joda::atom::ImagePlane *image)
  {
    pipelineContext.actImagePlane.image = image->image.clone();
    pipelineContext.actImagePlane.tile  = image->tile;
    pipelineContext.actImagePlane.mId   = image->mId;
  }

  [[nodiscard]] joda::atom::ObjectList &getActObjects()
  {
    return iterationContext.actObjects;
  }

  [[nodiscard]] const joda::enums::PlaneId &getActIterator() const
  {
    return pipelineContext.actImagePlane.getId().imagePlane;
  }

  [[nodiscard]] bool doesImageInCacheExist(joda::enums::ImageId cacheId) const
  {
    getCorrectIteration(cacheId.imagePlane);
    return iterationContext.imageCache.contains(cacheId);
  }

  joda::atom::ImagePlane *addImageToCache(joda::enums::ImageId cacheId, std::unique_ptr<joda::atom::ImagePlane> img)
  {
    getCorrectIteration(cacheId.imagePlane);
    return iterationContext.imageCache.try_emplace(cacheId, std::move(img)).first->second.get();
  }

  [[nodiscard]] const joda::atom::ImagePlane *loadImageFromCache(joda::enums::ImageId cacheId);
  void storeImageToCache(joda::enums::ImageId cacheId, const joda::atom::ImagePlane &image) const
  {
    getCorrectIteration(cacheId.imagePlane);
    iterationContext.imageCache.try_emplace(cacheId, ::std::make_unique<joda::atom::ImagePlane>(image));
  }

  [[nodiscard]] joda::atom::ObjectList *loadObjectsFromCache(joda::enums::ObjectStoreId cacheId = {}) const
  {
    if(cacheId.storeIdx == enums::MemoryIdx::M0) {
      return &iterationContext.actObjects;
    }
    getCorrectObjectId(cacheId);
    return globalContext.objectCache.at(cacheId).get();
  }

  [[nodiscard]] uint16_t getAppliedMinThreshold() const
  {
    return pipelineContext.actImagePlane.appliedMinThreshold;
  }

  void setImageValidity(enums::ChannelValidityEnum validityIn)
  {
    enums::ChannelValidity validity;
    validity.set(validityIn);
    globalContext.database.setImageValidity(imageContext.imageId, validity);
  }

  void setImagePlaneValidity(enums::ChannelValidityEnum validityIn)
  {
    enums::ChannelValidity validity;
    validity.set(validityIn);
    globalContext.database.setImagePlaneValidity(imageContext.imageId, getActIterator(), validity);
  }

  void setImagePlaneClusterClusterValidity(enums::ClusterIdIn clusterIn, enums::ChannelValidityEnum validityIn)
  {
    enums::ChannelValidity validity;
    validity.set(validityIn);
    globalContext.database.setImagePlaneClusterClusterValidity(imageContext.imageId, getActIterator(), getClusterId(clusterIn), validity);
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

  [[nodiscard]] enums::ClusterId getClusterId(enums::ClusterIdIn in) const
  {
    return in != enums::ClusterIdIn::$ ? static_cast<enums::ClusterId>(in) : pipelineContext.defaultClusterId;
  }

  [[nodiscard]] enums::ClassId getClassId(enums::ClassIdIn in) const
  {
    return in != enums::ClassIdIn::$ ? static_cast<enums::ClassId>(in) : pipelineContext.defaultClassId;
  }

  [[nodiscard]] std::set<enums::ClassId> getClassId(std::set<enums::ClassIdIn> in) const
  {
    std::set<enums::ClassId> out;
    for(const auto &d : in) {
      out.emplace(getClassId(d));
    }
    return out;
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

private:
  /////////////////////////////////////////////////////
  GlobalContext &globalContext;
  PlateContext &plateContext;
  ImageContext &imageContext;
  IterationContext &iterationContext;
  PipelineContext pipelineContext;
};

}    // namespace joda::processor
