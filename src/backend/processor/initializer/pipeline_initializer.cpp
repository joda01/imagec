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
#include <utility>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_clusters.hpp"
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
PipelineInitializer::PipelineInitializer(const settings::ProjectImageSetup &settings,
                                         const std::filesystem::path &imagePath,
                                         processor::ImageContext &imageContextOut,
                                         processor::GlobalContext &globalContextOut) :
    mSettings(settings),
    mImageContext(imageContextOut)
{
  imageContextOut.imageMeta            = joda::image::reader::ImageReader::getOmeInformation(imagePath.string());
  imageContextOut.imagePath            = imagePath;
  imageContextOut.imageId              = joda::helper::fnv1a(imagePath.string());
  globalContextOut.resultsOutputFolder = std::filesystem::path(settings.resultsOutputFolder);

  switch(settings.tStackHandling) {
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
void PipelineInitializer::initPipeline(const joda::settings::PipelineSettings &pipelineSetup, const enums::tile_t &tile,
                                       const joda::enums::IteratorId &imagePartToLoad,
                                       joda::processor::ProcessContext &processStepOut)
{
  int32_t zStacksToLoad = 1;
  int32_t c             = pipelineSetup.cStackIndex;
  int32_t z             = pipelineSetup.zStackIndex;
  int32_t t             = pipelineSetup.tStackIndex;

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

  //
  // Write context
  //
  if(pipelineSetup.defaultClusterId == enums::ClusterIdIn::$) {
    throw std::invalid_argument("Default cluster ID must not be >$<.");
  }
  processStepOut.pipelineContext.defaultClusterId = static_cast<enums::ClusterId>(pipelineSetup.defaultClusterId);
  processStepOut.pipelineContext.actImagePlane.setId(
      joda::enums::ImageId{.imageIdx = joda::enums::MemoryIdxIn::M0, .iteration{.tStack = t, .zStack = z, .cStack = c}},
      tile);

  //
  // Load from image file
  //
  if(joda::settings::PipelineSettings::Source::FROM_FILE == pipelineSetup.source) {
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
    if(pipelineSetup.zProjection != joda::settings::PipelineSettings::ZProjection::NONE) {
      auto max = [&loadImage, &contextImage, c, t](int zIdx) { cv::max(contextImage, loadImage(zIdx, c, t)); };
      auto min = [&loadImage, &contextImage, c, t](int zIdx) { cv::min(contextImage, loadImage(zIdx, c, t)); };
      auto avg = [&loadImage, &contextImage, c, t](int zIdx) { cv::mean(contextImage, loadImage(zIdx, c, t)); };

      std::function<void(int)> func;

      switch(pipelineSetup.zProjection) {
        case joda::settings::PipelineSettings::ZProjection::MAX_INTENSITY:
          func = max;
          break;
        case joda::settings::PipelineSettings::ZProjection::MIN_INTENSITY:
          func = min;
          break;
        case joda::settings::PipelineSettings::ZProjection::AVG_INTENSITY:
          func = avg;
          break;
        case joda::settings::PipelineSettings::ZProjection::NONE:
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
  if(joda::settings::PipelineSettings::Source::BLANK == pipelineSetup.source) {
    auto rows = mImageContext.imageMeta.getImageInfo().resolutions.at(0).imageHeight;
    auto cols = mImageContext.imageMeta.getImageInfo().resolutions.at(0).imageWidth;

    joda::atom::ImagePlane &contextImage = processStepOut.getActImage();
    contextImage.image.create(rows, cols, CV_16UC1);
    contextImage.image.setTo(cv::Scalar::all(0));
  }

  //
  // Load from memory
  /// \todo Load from memory
  //
  if(joda::settings::PipelineSettings::Source::FROM_MEMORY == pipelineSetup.source) {
  }

  // Store original image to cache
  processStepOut.addImageToCache(processStepOut.getActImage().getId(),
                                 std::move(std::make_unique<joda::atom::ImagePlane>(processStepOut.getActImage())));
}

}    // namespace joda::processor
