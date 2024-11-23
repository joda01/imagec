///
/// \file      image_loader.cüp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "pipeline_initializer.hpp"
#include <filesystem>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enum_memory_idx.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/fnv1a.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/processor/context/process_context.hpp"
#include "backend/settings/project_settings/project_image_setup.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>

namespace joda::processor {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
PipelineInitializer::PipelineInitializer(const settings::ProjectImageSetup &settings) : mSettings(settings)
{
}

void PipelineInitializer::init(ImageContext &imageContextOut)
{
  mImageContext      = &imageContextOut;
  mNrOfZStacks       = imageContextOut.imageMeta.getNrOfZStack();
  mTotalNrOfChannels = imageContextOut.imageMeta.getNrOfChannels();

  switch(mSettings.tStackHandling) {
    case settings::ProjectImageSetup::TStackHandling::EXACT_ONE:
      mTstackToLoad = 1;
      break;
    case settings::ProjectImageSetup::TStackHandling::EACH_ONE:
      mTstackToLoad = imageContextOut.imageMeta.getNrOfTStack();
      break;
  }

  switch(mSettings.zStackHandling) {
    case settings::ProjectImageSetup::ZStackHandling::EXACT_ONE:
    case settings::ProjectImageSetup::ZStackHandling::INTENSITY_PROJECTION:
      mZStackToLoad = 1;
      break;
    case settings::ProjectImageSetup::ZStackHandling::EACH_ONE:
      mZStackToLoad = imageContextOut.imageMeta.getNrOfZStack();
      break;
  }

  // Load image in tiles if too big
  const auto &imageInfo = imageContextOut.imageMeta.getImageInfo().resolutions.at(0);
  auto imageSize        = imageContextOut.imageMeta.getSize();

  if(std::get<0>(imageSize) > getCompositeTileSize().width || std::get<1>(imageSize) > getCompositeTileSize().height) {
    mNrOfTiles               = imageInfo.getNrOfTiles(getCompositeTileSize().width, getCompositeTileSize().height);
    imageContextOut.tileSize = {getCompositeTileSize().width, getCompositeTileSize().height};

    mLoadImageInTiles = true;
  } else {
    mNrOfTiles               = {1, 1};
    auto size                = imageSize;
    imageContextOut.tileSize = {static_cast<int32_t>(std::get<0>(size)), static_cast<int32_t>(std::get<1>(size))};
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PipelineInitializer::initPipeline(const joda::settings::PipelineSettings &pipelineSetup, const enums::tile_t &tile,
                                       const joda::enums::PlaneId &imagePartToLoad, joda::processor::ProcessContext &processContext) const
{
  int32_t c = pipelineSetup.cStackIndex;
  int32_t z = pipelineSetup.zStackIndex;
  int32_t t = pipelineSetup.tStackIndex;

  joda::atom::ImagePlane &imagePlaneOut = processContext.getActImage();
  imagePlaneOut.tile                    = tile;

  switch(mSettings.tStackHandling) {
    case settings::ProjectImageSetup::TStackHandling::EXACT_ONE:
      t = pipelineSetup.tStackIndex;
      break;
    case settings::ProjectImageSetup::TStackHandling::EACH_ONE:
      t = imagePartToLoad.tStack;
      break;
  }

  switch(mSettings.zStackHandling) {
    case settings::ProjectImageSetup::ZStackHandling::EXACT_ONE:
      z = pipelineSetup.zStackIndex;
      break;
    case settings::ProjectImageSetup::ZStackHandling::INTENSITY_PROJECTION:
      z = 0;
      break;
    case settings::ProjectImageSetup::ZStackHandling::EACH_ONE:
      z = imagePartToLoad.zStack;
      break;
  }

  enums::PlaneId planeToLoad{.tStack = t, .zStack = z, .cStack = c};

  auto zProjection = mSettings.zStackHandling == settings::ProjectImageSetup::ZStackHandling::INTENSITY_PROJECTION ? pipelineSetup.zProjection
                                                                                                                   : enums::ZProjection::NONE;
  //
  // Start with blank image
  //
  if(joda::settings::PipelineSettings::Source::BLANK == pipelineSetup.source || c < 0 || c >= mTotalNrOfChannels) {
    auto imageHeight = mImageContext->imageMeta.getImageInfo().resolutions.at(0).imageHeight;
    auto imageWidth  = mImageContext->imageMeta.getImageInfo().resolutions.at(0).imageWidth;

    if(mLoadImageInTiles) {
      int32_t offsetX         = std::get<0>(tile) * getCompositeTileSize().width;
      int32_t offsetY         = std::get<1>(tile) * getCompositeTileSize().height;
      int32_t tileWidthToLoad = getCompositeTileSize().width;
      ;
      int32_t tileHeightToLoad = getCompositeTileSize().height;
      if(offsetX + getCompositeTileSize().width > imageWidth) {
        tileWidthToLoad = getCompositeTileSize().width;
        -((offsetX + getCompositeTileSize().width) - imageWidth);
      }

      if(offsetY + getCompositeTileSize().height > imageHeight) {
        tileHeightToLoad = getCompositeTileSize().height - ((offsetY + getCompositeTileSize().height) - imageHeight);
      }
      imageWidth  = tileWidthToLoad;
      imageHeight = tileHeightToLoad;
    }

    imagePlaneOut.setId(joda::enums::ImageId{zProjection, planeToLoad}, tile);
    imagePlaneOut.image.create(imageHeight, imageWidth, CV_16UC1);
    imagePlaneOut.image.setTo(cv::Scalar::all(0));

    // Store original image to cache
    processContext.addImageToCache(imagePlaneOut.getId(), std::move(std::make_unique<joda::atom::ImagePlane>(imagePlaneOut)));
  } else if(joda::settings::PipelineSettings::Source::FROM_MEMORY == pipelineSetup.source) {
    //
    // Load from memory
    /// \todo Load from memory
    //
  } else if(joda::settings::PipelineSettings::Source::FROM_FILE == pipelineSetup.source) {
    processContext.setActImage(processContext.loadImageFromCache(
        loadImageToCache(planeToLoad,
                         mSettings.zStackHandling == settings::ProjectImageSetup::ZStackHandling::INTENSITY_PROJECTION ? pipelineSetup.zProjection
                                                                                                                       : enums::ZProjection::NONE,
                         tile, processContext)));
  }

  //
  // Write context
  //
  processContext.initDefaultSettings(pipelineSetup.defaultClusterId, pipelineSetup.defaultClassId, zProjection);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
enums::ImageId PipelineInitializer::loadImageToCache(const enums::PlaneId &planeToLoad, enums::ZProjection zProjection, const enums::tile_t &tile,
                                                     joda::processor::ProcessContext &processContext) const
{
  std::lock_guard<std::mutex> locked(mLoadMutex);
  joda::atom::ImagePlane imagePlaneOut;
  imagePlaneOut.tile = tile;

  int32_t c = planeToLoad.cStack;
  int32_t z = planeToLoad.zStack;
  int32_t t = planeToLoad.tStack;

  imagePlaneOut.setId(joda::enums::ImageId{zProjection, planeToLoad}, tile);

  if(processContext.doesImageInCacheExist(imagePlaneOut.getId())) {
    // Image still exist. No need to load -> Just use the cache cache
    return imagePlaneOut.getId();
  }

  //
  // Load from image file
  //

  auto loadEntireImage = [this, &planeToLoad](int32_t z, int32_t c, int32_t t) {
    return joda::image::reader::ImageReader::loadEntireImage(
        mImageContext->imagePath.string(), joda::image::reader::ImageReader::Plane{.z = z, .c = c, .t = t}, 0, 0, mImageContext->imageMeta);
  };

  auto loadImageTile = [this, &tile](int32_t z, int32_t c, int32_t t) {
    return joda::image::reader::ImageReader::loadImageTile(mImageContext->imagePath.string(),
                                                           joda::image::reader::ImageReader::Plane{.z = z, .c = c, .t = t}, 0, 0,
                                                           joda::ome::TileToLoad{.tileX      = std::get<0>(tile),
                                                                                 .tileY      = std::get<1>(tile),
                                                                                 .tileWidth  = getCompositeTileSize().width,
                                                                                 .tileHeight = getCompositeTileSize().height},
                                                           mImageContext->imageMeta);
  };

  std::function<cv::Mat(int32_t, int32_t, int32_t)> loadImage = loadEntireImage;
  if(mLoadImageInTiles) {
    loadImage = loadImageTile;
  }

  auto i = DurationCount::start("Load image " + mImageContext->imagePath.string());

  auto &image = imagePlaneOut.image;
  image       = loadImage(z, c, t);

  //
  // Do z -projection if activated
  //
  if(zProjection != enums::ZProjection::NONE) {
    auto max = [&loadImage, &image, c, t](int zIdx) { image = cv::max(image, loadImage(zIdx, c, t)); };
    auto min = [&loadImage, &image, c, t](int zIdx) { image = cv::min(image, loadImage(zIdx, c, t)); };
    auto avg = [&loadImage, &image, c, t](int zIdx) { image = cv::mean(image, loadImage(zIdx, c, t)); };

    std::function<void(int)> func;

    switch(zProjection) {
      case enums::ZProjection::MAX_INTENSITY:
        func = max;
        break;
      case enums::ZProjection::MIN_INTENSITY:
        func = min;
        break;
      case enums::ZProjection::AVG_INTENSITY:
        func = avg;
        break;
      case enums::ZProjection::NONE:
        break;
    }

    for(uint32_t zIdx = 1; zIdx < mNrOfZStacks; zIdx++) {
      func(zIdx);
    }
  }
  DurationCount::stop(i);

  // Store original image to cache
  processContext.addImageToCache(imagePlaneOut.getId(), std::move(std::make_unique<joda::atom::ImagePlane>(imagePlaneOut)));

  return imagePlaneOut.getId();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto PipelineInitializer::getCompositeTileSize() const -> TileSize const
{
  return {mSettings.imageTileSettings.tileWidth, mSettings.imageTileSettings.tileHeight};
}
}    // namespace joda::processor
