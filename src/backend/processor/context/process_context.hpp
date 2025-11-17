
///
/// \file      process_step.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <atomic>
#include <filesystem>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include "backend/artifacts/image/image.hpp"
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/artifacts/roi/roi.hpp"
#include "backend/database/database.hpp"
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enum_memory_idx.hpp"
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enum_validity.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_units.hpp"
#include "backend/enums/types.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include "backend/processor/context/plate_context.hpp"
#include "backend/settings/project_settings/project_class.hpp"
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
  std::filesystem::path workingDirectory;
  std::unique_ptr<db::DatabaseInterface> database;
  std::map<enums::ClassId, joda::settings::Class> classes;

private:
  objectCache_t objectCache;
};

class ProcessContext
{
public:
  ProcessContext(GlobalContext &globalContext, PlateContext &plateContext, ImageContext &imageContext, IterationContext &iterationContext);

  void initDefaultSettings(enums::ClassId classId, enums::ZProjection zProjection, int32_t pipelineIndex, enums::Units realSizesUnit)
  {
    pipelineContext.defaultClassId     = classId;
    pipelineContext.defaultZProjection = zProjection;
    pipelineContext.pipelineIndex      = pipelineIndex;
    pipelineContext.realSizesUnit      = realSizesUnit;
  }

  enums::Units getPipelineRealValuesUnit() const
  {
    return pipelineContext.realSizesUnit;
  }

  void setBinaryImage(uint16_t thresholdMin, uint16_t thresholdMax)
  {
    pipelineContext.actImagePlane.setToBinary();
    pipelineContext.actImagePlane.appliedMinThreshold = thresholdMin;
    pipelineContext.actImagePlane.appliedMaxThreshold = thresholdMax;
  }

  bool isRgb() const
  {
    return pipelineContext.actImagePlane.isRgb();
  }

  void setToGrayscaleImage()
  {
    pipelineContext.actImagePlane.setToGrayscale();
  }

  [[nodiscard]] joda::atom::ImagePlane &getActImage()
  {
    return pipelineContext.actImagePlane;
  }

  [[nodiscard]] enums::PlaneId getActImagePlaneId() const
  {
    return pipelineContext.actImagePlane.getId().imagePlane;
  }
  [[nodiscard]] const std::filesystem::path &getActImagePath() const;

  [[nodiscard]] const std::filesystem::path &getWorkingDirectory() const
  {
    return globalContext.workingDirectory;
  }

  [[nodiscard]] const std::filesystem::path &getOutputFolder() const
  {
    return globalContext.resultsOutputFolder;
  }

  [[nodiscard]] enums::tile_t getActTile() const
  {
    return pipelineContext.actImagePlane.tile;
  }

  [[nodiscard]] joda::enums::TileInfo getTileInfo() const
  {
    return {getActTile(), getTileSize()};
  }

  [[nodiscard]] cv::Size getTileSize() const
  {
    return imageContext.tileSize;
  }

  ome::PhyiscalSize getPhysicalPixelSIzeOfImage() const
  {
    return imageContext.imageMeta.getPhyiscalSize(imageContext.series);
  }

  void setActImage(const joda::atom::ImagePlane *image)
  {
    pipelineContext.actImagePlane.image = image->image.clone();
    pipelineContext.actImagePlane.tile  = image->tile;
    pipelineContext.actImagePlane.mId   = image->mId;
  }

  [[nodiscard]] joda::atom::ObjectList &getActObjects()
  {
    return iterationContext.getObjects();
  }

  [[nodiscard]] const joda::enums::PlaneId &getActIterator() const
  {
    return pipelineContext.actImagePlane.getId().imagePlane;
  }

  [[nodiscard]] bool doesImageInCacheExist(enums::MemoryScope scope, joda::enums::ImageId cacheId) const
  {
    getCorrectIteration(cacheId.imagePlane);
    if(scope == enums::MemoryScope::ITERATION) {
      return iterationContext.imageCache.contains(getMemoryIdx(cacheId));
    } else {
      return pipelineContext.imageCache.contains(getMemoryIdx(cacheId));
    }
  }

  joda::atom::ImagePlane *addImageToCache(enums::MemoryScope scope, joda::enums::ImageId cacheId, std::unique_ptr<joda::atom::ImagePlane> img)
  {
    getCorrectIteration(cacheId.imagePlane);
    if(scope == enums::MemoryScope::ITERATION) {
      return iterationContext.imageCache.try_emplace(getMemoryIdx(cacheId), std::move(img)).first->second.get();
    } else {
      return pipelineContext.imageCache.try_emplace(getMemoryIdx(cacheId), std::move(img)).first->second.get();
    }
  }

  [[nodiscard]] const joda::atom::ImagePlane *loadImageFromCache(enums::MemoryScope scope, joda::enums::ImageId cacheId);
  void storeImageToCache(enums::MemoryScope scope, joda::enums::ImageId cacheId, const joda::atom::ImagePlane &image) const
  {
    getCorrectIteration(cacheId.imagePlane);
    if(scope == enums::MemoryScope::ITERATION) {
      iterationContext.imageCache.try_emplace(getMemoryIdx(cacheId), ::std::make_unique<joda::atom::ImagePlane>(image));
    } else {
      pipelineContext.imageCache.try_emplace(getMemoryIdx(cacheId), ::std::make_unique<joda::atom::ImagePlane>(image));
    }
  }

  [[nodiscard]] joda::atom::ObjectList *loadObjectsFromCache(joda::enums::ObjectStoreId cacheId = {}) const
  {
    if(cacheId.storeIdx == enums::MemoryIdx::M0) {
      return &iterationContext.getObjects();
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
    globalContext.database->setImageValidity(imageContext.imageId, validity);
  }

  void setImagePlaneValidity(enums::ChannelValidityEnum validityIn)
  {
    enums::ChannelValidity validity;
    validity.set(validityIn);
    globalContext.database->setImagePlaneValidity(imageContext.imageId, getActIterator(), validity);
  }

  void setImagePlaneClasssClasssValidity(enums::ClassIdIn classIn, enums::ChannelValidityEnum validityIn)
  {
    enums::ChannelValidity validity;
    validity.set(validityIn);
    globalContext.database->setImagePlaneClasssClasssValidity(imageContext.imageId, getActIterator(), getClassId(classIn), validity);
  }

  // void storeObjectsToCache(joda::enums::ObjectStoreId cacheId, const joda::atom::ObjectList &object) const
  //{
  // #warning "IF classs ID still exists. Merge or override!?"
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

  [[nodiscard]] cv::Size getOriginalImageSize() const
  {
    return {imageContext.imageMeta.getImageWidth(imageContext.series, 0), imageContext.imageMeta.getImageHeight(imageContext.series, 0)};
  }

  [[nodiscard]] enums::ClassId getClassId(enums::ClassIdIn in) const
  {
    if(in >= enums::ClassIdIn::TEMP_01 && in <= enums::ClassIdIn::TEMP_LAST) {
      return getTemporaryClassId(in);
    }

    return in != enums::ClassIdIn::$ ? static_cast<enums::ClassId>(in) : pipelineContext.defaultClassId;
  }

  enums::ClassId getTemporaryClassId(enums::ClassIdIn in) const
  {
    if(in >= enums::ClassIdIn::TEMP_01 && in <= enums::ClassIdIn::TEMP_LAST) {
      return static_cast<enums::ClassId>(static_cast<int32_t>(enums::ClassId::RESERVED_FOR_TEMP_START) +
                                         (static_cast<int32_t>(in) - static_cast<int32_t>(enums::ClassIdIn::TEMP_01)) +
                                         (pipelineContext.pipelineIndex * static_cast<int32_t>(enums::ClassIdIn::TEMP_LAST)));
    }
    throw std::invalid_argument("This is not a temporary class!");
  }

  [[nodiscard]] std::set<enums::ClassId> getClassId(std::set<enums::ClassIdIn> in) const
  {
    std::set<enums::ClassId> out;
    for(const auto &d : in) {
      out.emplace(getClassId(d));
    }
    return out;
  }

  enums::MemoryIdx getMemoryIdx(enums::ImageId imgId) const
  {
    getCorrectIteration(imgId.imagePlane);

    if(imgId.memoryId == enums::MemoryIdx::NONE) {
      if(imgId.zProjection == enums::ZProjection::$) {
        imgId.zProjection = pipelineContext.defaultZProjection;
      }
      enums::MemoryIdx xx(imgId.imagePlane, imgId.zProjection, enums::MemoryIdx::Enum::RESERVED_01);
      return enums::MemoryIdx(imgId.imagePlane, imgId.zProjection, enums::MemoryIdx::Enum::RESERVED_01);
    }
    return imgId.memoryId;
  }

  std::string getColorClass(enums::ClassIdIn in) const
  {
    auto classs = getClassId(in);
    if(globalContext.classes.contains(classs)) {
      return globalContext.classes.at(classs).color;
    }
    return "#ADD8E6";
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
