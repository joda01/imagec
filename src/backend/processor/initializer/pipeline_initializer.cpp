///
/// \file      image_loader.cüp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
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
PipelineInitializer::PipelineInitializer(const settings::ProjectImageSetup &settings, const settings::ProjectPipelineSetup &pipelineSetup,
                                         const std::filesystem::path &imagePath, const std::filesystem::path &imagesBasePath) :
    mSettings(settings),
    mSettingsPipeline(pipelineSetup), mImageRead(imagePath)
{
  ome::PhyiscalSize phys = {};
  if(settings.imagePixelSizeSettings.mode == enums::PhysicalSizeMode::Manual) {
    phys =
        joda::ome::PhyiscalSize{static_cast<double>(settings.imagePixelSizeSettings.pixelWidth),
                                static_cast<double>(settings.imagePixelSizeSettings.pixelHeight), 0, settings.imagePixelSizeSettings.pixelSizeUnit};
  }
  mImageMeta = mImageRead.getOmeInformation(phys);
  mImageId   = joda::helper::generateImageIdFromPath(imagePath.string(), imagesBasePath);

  auto series = settings.series;
  if(settings.series >= static_cast<int32_t>(mImageMeta.getNrOfSeries())) {
    series = static_cast<int32_t>(mImageMeta.getNrOfSeries()) - 1;
  }
  mSelectedSeries = series;

  nrOfZStacks = static_cast<uint32_t>(mImageMeta.getNrOfZStack(series));

  mTotalNrOfZChannels = mImageMeta.getNrOfZStack(series);
  mTotalNrOfTChannels = mImageMeta.getNrOfTStack(series);
  mTotalNrOfChannels  = mImageMeta.getNrOfChannels(series);

  switch(mSettings.tStackHandling) {
    case settings::ProjectImageSetup::TStackHandling::EXACT_ONE:
      mTstackToLoad = 1;
      break;
    case settings::ProjectImageSetup::TStackHandling::EACH_ONE:
      mTstackToLoad = static_cast<uint32_t>(mImageMeta.getNrOfTStack(series));
      break;
  }

  switch(mSettings.zStackHandling) {
    case settings::ProjectImageSetup::ZStackHandling::EXACT_ONE:
      mZStackToLoad = 1;
      break;
    case settings::ProjectImageSetup::ZStackHandling::EACH_ONE:
      mZStackToLoad = static_cast<uint32_t>(mImageMeta.getNrOfZStack(series));
      break;
  }

  // Load image in tiles if too big
  const auto &imageInfo = mImageMeta.getImageInfo(series).resolutions.at(0);
  auto imageSize        = mImageMeta.getSize(series);

  if(std::get<0>(imageSize) > getCompositeTileSize().width || std::get<1>(imageSize) > getCompositeTileSize().height) {
    mNrOfTiles = imageInfo.getNrOfTiles(getCompositeTileSize().width, getCompositeTileSize().height);
    tileSize   = {getCompositeTileSize().width, getCompositeTileSize().height};

    loadImageInTiles = true;
  } else {
    loadImageInTiles = false;
    mNrOfTiles       = {1, 1};
    auto size        = imageSize;
    tileSize         = {static_cast<int32_t>(std::get<0>(size)), static_cast<int32_t>(std::get<1>(size))};
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
                                       const joda::enums::PlaneId &imagePartToLoad, joda::processor::ProcessContext &processContext,
                                       int32_t pipelineIndex) const
{
  int32_t c = pipelineSetup.cStackIndex;
  int32_t z = pipelineSetup.zStackIndex;
  int32_t t = pipelineSetup.tStackIndex;

  joda::atom::ImagePlane &imagePlaneOut = processContext.getActImage();
  imagePlaneOut.tile                    = tile;
  imagePlaneOut.series                  = mSelectedSeries;

  auto zProjection =
      mSettings.zStackHandling == settings::ProjectImageSetup::ZStackHandling::EACH_ONE ? enums::ZProjection::NONE : pipelineSetup.zProjection;

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
    case settings::ProjectImageSetup::ZStackHandling::EACH_ONE:
      z = imagePartToLoad.zStack;
      break;
  }

  // If we do a z-projection start with zero
  if(zProjection != enums::ZProjection::NONE) {
    z = 0;
  }
  z = limitChannel(z, mTotalNrOfZChannels);
  t = limitChannel(t, mTotalNrOfTChannels);
  c = limitChannel(c, mTotalNrOfChannels);

  enums::PlaneId planeToLoad{.tStack = t, .zStack = z, .cStack = c};

  //
  // Start with blank image
  //
  if(joda::settings::PipelineSettings::Source::BLANK == pipelineSetup.source || c < 0 || c >= mTotalNrOfChannels) {
    auto imageHeight = mImageMeta.getImageInfo(imagePlaneOut.series).resolutions.at(0).imageHeight;
    auto imageWidth  = mImageMeta.getImageInfo(imagePlaneOut.series).resolutions.at(0).imageWidth;

    if(loadImageInTiles) {
      int32_t offsetX          = std::get<0>(tile) * getCompositeTileSize().width;
      int32_t offsetY          = std::get<1>(tile) * getCompositeTileSize().height;
      int32_t tileWidthToLoad  = getCompositeTileSize().width;
      int32_t tileHeightToLoad = getCompositeTileSize().height;
      if(offsetX + getCompositeTileSize().width > imageWidth) {
        tileWidthToLoad = getCompositeTileSize().width - ((offsetX + getCompositeTileSize().width) - imageWidth);
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
    processContext.addImageToCache(enums::MemoryScope::ITERATION, imagePlaneOut.getId(), std::make_unique<joda::atom::ImagePlane>(imagePlaneOut));
  } else if(joda::settings::PipelineSettings::Source::FROM_MEMORY == pipelineSetup.source) {
    //
    // Load from memory
    /// \todo Load from memory
    //
  } else if(joda::settings::PipelineSettings::Source::FROM_FILE == pipelineSetup.source) {
    processContext.setActImage(processContext.loadImageFromCache(
        enums::MemoryScope::ITERATION,
        loadImageAndStoreToCache(enums::MemoryScope::ITERATION, planeToLoad,
                                 mSettings.zStackHandling == settings::ProjectImageSetup::ZStackHandling::EACH_ONE ? enums::ZProjection::NONE
                                                                                                                   : pipelineSetup.zProjection,
                                 tile, processContext)));
  }

  //
  // Write context
  //
  processContext.initDefaultSettings(pipelineSetup.defaultClassId, zProjection, pipelineIndex, mSettingsPipeline.realSizesUnit);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
enums::ImageId PipelineInitializer::loadImageAndStoreToCache(enums::MemoryScope scope, const enums::PlaneId &planeToLoad,
                                                             enums::ZProjection zProjection, const enums::tile_t &tile,
                                                             joda::processor::ProcessContext &processContext) const
{
  static std::mutex mLoadMutex;

  std::lock_guard<std::mutex> locked(mLoadMutex);
  joda::atom::ImagePlane imagePlaneOut;
  imagePlaneOut.tile = tile;

  int32_t c = planeToLoad.cStack;
  int32_t z = planeToLoad.zStack;
  int32_t t = planeToLoad.tStack;

  if(zProjection == enums::ZProjection::TAKE_MIDDLE) {
    z                      = static_cast<int32_t>(nrOfZStacks / 2);
    auto &planeToLoadEdit  = const_cast<enums::PlaneId &>(planeToLoad);
    planeToLoadEdit.zStack = z;
  } else if(zProjection != enums::ZProjection::NONE) {
    z = 0;
  }

  imagePlaneOut.setId(joda::enums::ImageId{zProjection, planeToLoad}, tile);

  if(processContext.doesImageInCacheExist(scope, imagePlaneOut.getId())) {
    // Image still exist. No need to load -> Just use the cache cache
    return imagePlaneOut.getId();
  }

  //
  // Load from image file
  //
  auto loadEntireImage = [this, series = mSelectedSeries](int32_t zIn, int32_t cIn, int32_t tIn) {
    return mImageRead.loadEntireImage(joda::enums::PlaneId{.tStack = tIn, .zStack = zIn, .cStack = cIn}, static_cast<uint16_t>(series), 0,
                                      mImageMeta);
  };

  auto loadImageTile = [this, &tile, series = mSelectedSeries](int32_t zIn, int32_t cIn, int32_t tIn) {
    return mImageRead.loadImageTile(
        joda::enums::PlaneId{.tStack = tIn, .zStack = zIn, .cStack = cIn}, static_cast<uint16_t>(series), 0,
        joda::ome::TileToLoad{.tileX = std::get<0>(tile), .tileY = std::get<1>(tile), .tileWidth = tileSize.width, .tileHeight = tileSize.height},
        mImageMeta);
  };

  std::function<cv::Mat(int32_t, int32_t, int32_t)> loadImage = loadEntireImage;
  if(loadImageInTiles) {
    loadImage = loadImageTile;
  }

  DurationCount durationCount("Load image");

  auto &image = imagePlaneOut.image;
  image       = loadImage(z, c, t);

  //
  // Do z -projection if activated
  //
  if(zProjection != enums::ZProjection::NONE && zProjection != enums::ZProjection::TAKE_MIDDLE) {
    auto max = [&loadImage, &image, c, t](int zIdx) { image = cv::max(image, loadImage(zIdx, c, t)); };
    auto min = [&loadImage, &image, c, t](int zIdx) { image = cv::min(image, loadImage(zIdx, c, t)); };
    auto avg = [&loadImage, &image, c, t](int zIdx) {
      auto tmp = loadImage(zIdx, c, t);
      tmp.convertTo(tmp, CV_32SC1);
      image = image + tmp;
    };

    std::function<void(int)> func;
    auto imageType = image.type();

    switch(zProjection) {
      case enums::ZProjection::MAX_INTENSITY:
        func = max;
        break;
      case enums::ZProjection::MIN_INTENSITY:
        func = min;
        break;
      case enums::ZProjection::AVG_INTENSITY:
        image.convertTo(image, CV_32SC1);    // Need to scale up because we are adding a lot of images to avoid overflow
        func = avg;
        break;
      case enums::ZProjection::NONE:
      case enums::ZProjection::$:
      case enums::ZProjection::UNDEFINED:
      case enums::ZProjection::TAKE_MIDDLE:
        break;
    }

    for(uint32_t zIdx = 1; zIdx < nrOfZStacks; zIdx++) {
      func(static_cast<int32_t>(zIdx));
    }
    // Avg intensity projection
    if(enums::ZProjection::AVG_INTENSITY == zProjection) {
      image = image / nrOfZStacks;
      image.convertTo(image, imageType);    // no scaling
    }
  }

  // Store original image to cache
  processContext.addImageToCache(scope, imagePlaneOut.getId(), std::make_unique<joda::atom::ImagePlane>(imagePlaneOut));

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

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t PipelineInitializer::limitChannel(int32_t wantedIndex, int32_t maxIndex)
{
  if(wantedIndex >= maxIndex) {
    return maxIndex - 1;
  }
  if(wantedIndex < 0) {
    return 0;
  }
  return wantedIndex;
}

}    // namespace joda::processor
