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
#include <utility>
#include "backend/enums/enum_images.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/processor/process_context.hpp"
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
PipelineInitializer::PipelineInitializer(const settings::PipelineInitializerSettings &settings,
                                         const std::filesystem::path &imagePath,
                                         processor::ImageContext &imageContextOut,
                                         processor::GlobalContext &globalContextOut) :
    mSettings(settings),
    mImageContext(imageContextOut)
{
  imageContextOut.imageMeta            = joda::image::reader::ImageReader::getOmeInformation(imagePath.string());
  imageContextOut.imagePath            = imagePath;
  globalContextOut.resultsOutputFolder = std::filesystem::path(settings.resultsOutputFolder);

  switch(settings.tStackHandling) {
    case settings::PipelineInitializerSettings::TStackHandling::EXACT_ONE:
      mTstackToLoad = 1;
      break;
    case settings::PipelineInitializerSettings::TStackHandling::EACH_ONE:
      mTstackToLoad = imageContextOut.imageMeta.getNrOfTStack();
      break;
  }

  switch(mSettings.zStackHandling) {
    case settings::PipelineInitializerSettings::ZStackHandling::EXACT_ONE:
    case settings::PipelineInitializerSettings::ZStackHandling::INTENSITY_PROJECTION:
      mZStackToLoad = 1;
      break;
    case settings::PipelineInitializerSettings::ZStackHandling::EACH_ONE:
      mZStackToLoad = imageContextOut.imageMeta.getNrOfZStack();
      break;
  }

  switch(mSettings.cStackHandling) {
    case settings::PipelineInitializerSettings::CStackHandling::EXACT_ONE:
      mCStackToLoad = 1;
      break;
    case settings::PipelineInitializerSettings::CStackHandling::EACH_ONE:
      mCStackToLoad = imageContextOut.imageMeta.getNrOfChannels();
      break;
  }

  // Load image in tiles if too big
  const auto &imageInfo = imageContextOut.imageMeta.getImageInfo().resolutions.at(0);
  if(imageInfo.imageMemoryUsage > MAX_IMAGE_SIZE_BYTES_TO_LOAD_AT_ONCE) {
    mNrOfTiles        = imageInfo.getNrOfTiles(COMPOSITE_TILE_WIDTH, COMPOSITE_TILE_HEIGHT);
    mLoadImageInTiles = true;
  } else {
    mNrOfTiles = {1, 1};
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PipelineInitializer::initPipeline(const joda::settings::PipelineInputImageLoaderSettings &pipelineSettings,
                                       const enums::tile_t &tile, const joda::enums::IteratorId &imagePartToLoad,
                                       joda::processor::ProcessContext &processStepOut)
{
  int32_t zStacksToLoad = 1;
  int32_t c             = pipelineSettings.cStackIndex;
  int32_t z             = pipelineSettings.zStackIndex;
  int32_t t             = pipelineSettings.tStackIndex;

  switch(mSettings.tStackHandling) {
    case settings::PipelineInitializerSettings::TStackHandling::EXACT_ONE:
      t = pipelineSettings.tStackIndex;
      break;
    case settings::PipelineInitializerSettings::TStackHandling::EACH_ONE:
      t = imagePartToLoad.tStack;
      break;
  }

  switch(mSettings.cStackHandling) {
    case settings::PipelineInitializerSettings::CStackHandling::EXACT_ONE:
      c = pipelineSettings.cStackIndex;
      break;
    case settings::PipelineInitializerSettings::CStackHandling::EACH_ONE:
      c = imagePartToLoad.cStack;
      break;
  }

  switch(mSettings.zStackHandling) {
    case settings::PipelineInitializerSettings::ZStackHandling::EXACT_ONE:
      z = pipelineSettings.zStackIndex;
      break;
    case settings::PipelineInitializerSettings::ZStackHandling::INTENSITY_PROJECTION:
      z             = 0;
      zStacksToLoad = mZStackToLoad;
      break;
    case settings::PipelineInitializerSettings::ZStackHandling::EACH_ONE:
      z = imagePartToLoad.zStack;
      break;
  }

  //
  // Write context
  //
  processStepOut.pipelineContext.defaultClusterId = pipelineSettings.defaultClusterId;
  processStepOut.pipelineContext.actImage.setId(
      {.imageIdx = joda::enums::ImageIdx::I0, .iteration{.tStack = t, .zStack = z, .cStack = c}}, tile);

  //
  // Load from image file
  //
  if(joda::settings::PipelineInputImageLoaderSettings::Source::FROM_FILE == pipelineSettings.source) {
    auto loadEntireImage = [this, &imagePartToLoad](int32_t z, int32_t c, int32_t t) {
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

    cv::Mat &contextImage = processStepOut.getActImage().image;

    contextImage = loadImage(z, c, t);

    //
    // Do z -projection if activated
    //
    if(pipelineSettings.zProjection != joda::settings::PipelineInputImageLoaderSettings::ZProjection::NONE) {
      auto max = [&loadImage, &contextImage, c, t](int zIdx) { cv::max(contextImage, loadImage(zIdx, c, t)); };
      auto min = [&loadImage, &contextImage, c, t](int zIdx) { cv::min(contextImage, loadImage(zIdx, c, t)); };
      auto avg = [&loadImage, &contextImage, c, t](int zIdx) { cv::mean(contextImage, loadImage(zIdx, c, t)); };

      std::function<void(int)> func;

      switch(pipelineSettings.zProjection) {
        case joda::settings::PipelineInputImageLoaderSettings::ZProjection::MAX_INTENSITY:
          func = max;
          break;
        case joda::settings::PipelineInputImageLoaderSettings::ZProjection::MIN_INTENSITY:
          func = min;
          break;
        case joda::settings::PipelineInputImageLoaderSettings::ZProjection::AVG_INTENSITY:
          func = avg;
          break;
        case joda::settings::PipelineInputImageLoaderSettings::ZProjection::NONE:
          break;
      }

      for(int zIdx = 1; zIdx < zStacksToLoad; zIdx++) {
        func(zIdx);
      }
    }
  }

  //
  // Start with blank image
  //
  if(joda::settings::PipelineInputImageLoaderSettings::Source::BLANK == pipelineSettings.source) {
    auto rows = mImageContext.imageMeta.getImageInfo().resolutions.at(0).imageHeight;
    auto cols = mImageContext.imageMeta.getImageInfo().resolutions.at(0).imageWidth;

    joda::atom::Image &contextImage = processStepOut.getActImage();
    contextImage.image.create(rows, cols, CV_16UC1);
    contextImage.image.setTo(cv::Scalar::all(0));
  }

  //
  // Load from memory
  /// \todo Load from memory
  //
  if(joda::settings::PipelineInputImageLoaderSettings::Source::FROM_MEMORY == pipelineSettings.source) {
  }

  // Store original image to cache
  processStepOut.addImageToCache(processStepOut.getActImage().getId(),
                                 std::move(std::make_unique<joda::atom::Image>(processStepOut.getActImage())));
}

}    // namespace joda::processor
