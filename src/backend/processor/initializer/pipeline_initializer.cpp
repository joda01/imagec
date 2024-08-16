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
#include <stdexcept>
#include <string>
#include <utility>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enum_memory_idx.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/fnv1a.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/processor/context/process_context.hpp"
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
PipelineInitializer::PipelineInitializer(const settings::ProjectSettings &settings,
                                         const std::filesystem::path &imagePath,
                                         processor::ImageContext &imageContextOut,
                                         processor::GlobalContext &globalContextOut) :
    mSettings(settings.imageSetup),
    mImageContext(imageContextOut)
{
  imageContextOut.imageMeta            = joda::image::reader::ImageReader::getOmeInformation(imagePath.string());
  imageContextOut.imagePath            = imagePath;
  imageContextOut.imageId              = joda::helper::fnv1a(imagePath.string());
  globalContextOut.resultsOutputFolder = std::filesystem::path(settings.workingDirectory);

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

  switch(mSettings.cStackHandling) {
    case settings::ProjectImageSetup::CStackHandling::EXACT_ONE:
      mCStackToLoad = 1;
      break;
    case settings::ProjectImageSetup::CStackHandling::EACH_ONE:
      mCStackToLoad = imageContextOut.imageMeta.getNrOfChannels();
      break;
  }

  // Load image in tiles if too big
  const auto &imageInfo = imageContextOut.imageMeta.getImageInfo().resolutions.at(0);
  if(imageInfo.imageMemoryUsage > MAX_IMAGE_SIZE_BYTES_TO_LOAD_AT_ONCE) {
    mNrOfTiles               = imageInfo.getNrOfTiles(COMPOSITE_TILE_WIDTH, COMPOSITE_TILE_HEIGHT);
    imageContextOut.tileSize = {COMPOSITE_TILE_WIDTH, COMPOSITE_TILE_HEIGHT};

    mLoadImageInTiles = true;
  } else {
    mNrOfTiles               = {1, 1};
    auto size                = mImageContext.imageMeta.getSize();
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
                                       const joda::enums::PlaneId &imagePartToLoad,
                                       joda::processor::ProcessContext &processContext)
{
  int32_t zStacksToLoad = 1;
  int32_t c             = pipelineSetup.cStackIndex;
  int32_t z             = pipelineSetup.zStackIndex;
  int32_t t             = pipelineSetup.tStackIndex;

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

  switch(mSettings.cStackHandling) {
    case settings::ProjectImageSetup::CStackHandling::EXACT_ONE:
      c = pipelineSetup.cStackIndex;
      break;
    case settings::ProjectImageSetup::CStackHandling::EACH_ONE:
      c = imagePartToLoad.cStack;
      break;
  }

  switch(mSettings.zStackHandling) {
    case settings::ProjectImageSetup::ZStackHandling::EXACT_ONE:
      z = pipelineSetup.zStackIndex;
      break;
    case settings::ProjectImageSetup::ZStackHandling::INTENSITY_PROJECTION:
      z             = 0;
      zStacksToLoad = mZStackToLoad;
      break;
    case settings::ProjectImageSetup::ZStackHandling::EACH_ONE:
      z = imagePartToLoad.zStack;
      break;
  }

  enums::PlaneId planeToLoad{.tStack = t, .zStack = z, .cStack = c};

  //
  // Start with blank image
  //
  if(joda::settings::PipelineSettings::Source::BLANK == pipelineSetup.source) {
    auto rows = mImageContext.imageMeta.getImageInfo().resolutions.at(0).imageHeight;
    auto cols = mImageContext.imageMeta.getImageInfo().resolutions.at(0).imageWidth;

    imagePlaneOut.setId(joda::enums::ImageId{mSettings.zStackHandling ==
                                                     settings::ProjectImageSetup::ZStackHandling::INTENSITY_PROJECTION
                                                 ? pipelineSetup.zProjection
                                                 : enums::ZProjection::NONE,
                                             planeToLoad},
                        tile);

    imagePlaneOut.image.create(rows, cols, CV_16UC1);
    imagePlaneOut.image.setTo(cv::Scalar::all(0));

    // Store original image to cache
    processContext.addImageToCache(imagePlaneOut.getId(),
                                   std::move(std::make_unique<joda::atom::ImagePlane>(imagePlaneOut)));
  }

  //
  // Load from memory
  /// \todo Load from memory
  //
  if(joda::settings::PipelineSettings::Source::FROM_MEMORY == pipelineSetup.source) {
  }

  if(joda::settings::PipelineSettings::Source::FROM_FILE == pipelineSetup.source) {
    std::cout << "Load image In: " << std::to_string(planeToLoad.cStack) << " " << std::to_string(planeToLoad.tStack)
              << " " << std::to_string(planeToLoad.zStack) << std::endl;
    processContext.setActImage(processContext.loadImageFromCache(
        loadImageToCache(planeToLoad,
                         mSettings.zStackHandling == settings::ProjectImageSetup::ZStackHandling::INTENSITY_PROJECTION
                             ? pipelineSetup.zProjection
                             : enums::ZProjection::NONE,
                         tile, processContext)));
  }

  //
  // Write context
  //
  if(pipelineSetup.defaultClusterId == enums::ClusterIdIn::$) {
    throw std::invalid_argument("Default cluster ID must not be >$<.");
  }
  processContext.pipelineContext.defaultClusterId = static_cast<enums::ClusterId>(pipelineSetup.defaultClusterId);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
enums::ImageId PipelineInitializer::loadImageToCache(const enums::PlaneId &planeToLoad, enums::ZProjection zProjection,
                                                     const enums::tile_t &tile,
                                                     joda::processor::ProcessContext &processContext)
{
  joda::atom::ImagePlane imagePlaneOut;
  imagePlaneOut.tile = tile;

  int32_t c = planeToLoad.cStack;
  int32_t z = planeToLoad.zStack;
  int32_t t = planeToLoad.tStack;

  imagePlaneOut.setId(joda::enums::ImageId{zProjection, planeToLoad}, tile);

  std::cout << "Load image " << std::to_string(planeToLoad.cStack) << " " << std::to_string(planeToLoad.tStack) << " "
            << std::to_string(planeToLoad.zStack) << std::endl;

  if(processContext.doesImageInCacheExist(imagePlaneOut.getId())) {
    // Image still exist. No need to load -> Just use the cache cache
    std::cout << "Exist" << std::endl;
    return imagePlaneOut.getId();
  }

  //
  // Load from image file
  //

  auto loadEntireImage = [this, &planeToLoad](int32_t z, int32_t c, int32_t t) {
    return joda::image::reader::ImageReader::loadEntireImage(
        mImageContext.imagePath.string(), joda::image::reader::ImageReader::Plane{.z = z, .c = c, .t = t}, 0, 0);
  };

  auto loadImageTile = [this, &tile](int32_t z, int32_t c, int32_t t) {
    return joda::image::reader::ImageReader::loadImageTile(
        mImageContext.imagePath.string(), joda::image::reader::ImageReader::Plane{.z = z, .c = c, .t = t}, 0, 0,
        joda::ome::TileToLoad{.tileX      = std::get<0>(tile),
                              .tileY      = std::get<1>(tile),
                              .tileWidth  = COMPOSITE_TILE_WIDTH,
                              .tileHeight = COMPOSITE_TILE_HEIGHT});
  };

  std::function<cv::Mat(int32_t, int32_t, int32_t)> loadImage = loadEntireImage;
  if(mLoadImageInTiles) {
    loadImage = loadImageTile;
  }

  auto &image = imagePlaneOut.image;
  image       = loadImage(z, c, t);

  //
  // Do z -projection if activated
  //
  if(zProjection != enums::ZProjection::NONE) {
    int32_t zStacksToLoad = mZStackToLoad;

    auto max = [&loadImage, &image, c, t](int zIdx) { cv::max(image, loadImage(zIdx, c, t)); };
    auto min = [&loadImage, &image, c, t](int zIdx) { cv::min(image, loadImage(zIdx, c, t)); };
    auto avg = [&loadImage, &image, c, t](int zIdx) { cv::mean(image, loadImage(zIdx, c, t)); };

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

    for(int zIdx = 1; zIdx < zStacksToLoad; zIdx++) {
      func(zIdx);
    }
  }

  // Store original image to cache
  processContext.addImageToCache(imagePlaneOut.getId(),
                                 std::move(std::make_unique<joda::atom::ImagePlane>(imagePlaneOut)));

  return imagePlaneOut.getId();
}

}    // namespace joda::processor
