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
/// \brief     A short description what happens here.
///

#include "image_loader.hpp"
#include <filesystem>
#include <functional>
#include "backend/commands/image_functions/image_loader/image_loader_settings.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include <opencv2/opencv.hpp>

namespace joda::cmd::functions {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
ImageLoader::ImageLoader(const ImageLoaderSettings &settings, const std::filesystem::path &imagePath) :
    mSettings(settings)
{
  mOmeInfo = joda::image::reader::ImageReader::getOmeInformation(imagePath.string());

  switch(settings.tStackHandling) {
    case ImageLoaderSettings::TStackHandling::EXACT_ONE:
      mTstackToLoad = 1;
      break;
    case ImageLoaderSettings::TStackHandling::EACH_INDIVIDUAL:
      mTstackToLoad = mOmeInfo.getNrOfTStack();
      break;
  }

  switch(mSettings.zStackHandling) {
    case ImageLoaderSettings::ZStackHandling::EXACT_ONE:
    case ImageLoaderSettings::ZStackHandling::INTENSITY_PROJECTION:
      mZStackToLoad = 1;
      break;
    case ImageLoaderSettings::ZStackHandling::EACH_INDIVIDUAL:
      mZStackToLoad = mOmeInfo.getNrOfZStack();
      break;
  }

  // Load image in tiles if too big
  const auto &imageInfo = mOmeInfo.getImageInfo().resolutions.at(0);
  if(imageInfo.imageMemoryUsage > MAX_IMAGE_SIZE_BYTES_TO_LOAD_AT_ONCE) {
    mNrOfTiles = mOmeInfo.getImageInfo().resolutions.at(0).getNrOfTiles(COMPOSITE_TILE_WIDTH, COMPOSITE_TILE_HEIGHT);
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
void ImageLoader::execute(processor::ProcessContext &context, processor::ProcessorMemory &memory, cv::Mat &image,
                          ObjectsListMap &result)
{
  auto loadEntireImage = [&context](int32_t z, int32_t c, int32_t t) {
    return joda::image::reader::ImageReader::loadEntireImage(
        context.imagePath.string(), joda::image::reader::ImageReader::Plane{.z = z, .c = c, .t = t}, 0, 0);
  };

  auto loadImageTile = [&context](int32_t z, int32_t c, int32_t t) {
    return joda::image::reader::ImageReader::loadImageTile(
        context.imagePath.string(), joda::image::reader::ImageReader::Plane{.z = z, .c = c, .t = t}, 0, 0,
        joda::ome::TileToLoad{.tileX      = std::get<0>(context.tile),
                              .tileY      = std::get<1>(context.tile),
                              .tileWidth  = COMPOSITE_TILE_WIDTH,
                              .tileHeight = COMPOSITE_TILE_HEIGHT});
  };

  std::function<cv::Mat(int32_t, int32_t, int32_t)> loadImage = loadEntireImage;
  if(mLoadImageInTiles) {
    loadImage = loadImageTile;
  }

  int32_t z             = context.zStack;
  int32_t c             = static_cast<int32_t>(context.channel);
  int32_t t             = context.tStack;
  int32_t zStacksToLoad = 1;

  switch(mSettings.tStackHandling) {
    case ImageLoaderSettings::TStackHandling::EXACT_ONE:
      t = context.loader.tStackIndex;
      break;
    case ImageLoaderSettings::TStackHandling::EACH_INDIVIDUAL:
      t = context.tStack;
      break;
  }

  switch(mSettings.zStackHandling) {
    case ImageLoaderSettings::ZStackHandling::EXACT_ONE:
      z = context.loader.zStackIndex;
      break;
    case ImageLoaderSettings::ZStackHandling::INTENSITY_PROJECTION:
      z             = 0;
      zStacksToLoad = mOmeInfo.getNrOfZStack();
      break;
    case ImageLoaderSettings::ZStackHandling::EACH_INDIVIDUAL:
      z = context.zStack;
      break;
  }

  //
  // Load image
  //
  image = loadImage(z, c, t);

  //
  // Do z -projection if activated
  //
  if(context.loader.zProjection != ChannelLoaderSettings::ZProjection::NONE) {
    auto max = [&loadImage, &image, &context, c, t](int zIdx) { cv::max(image, loadImage(zIdx, c, t)); };
    auto min = [&loadImage, &image, &context, c, t](int zIdx) { cv::min(image, loadImage(zIdx, c, t)); };
    auto avg = [&loadImage, &image, &context, c, t](int zIdx) { cv::mean(image, loadImage(zIdx, c, t)); };

    std::function<void(int)> func;

    switch(context.loader.zProjection) {
      case ChannelLoaderSettings::ZProjection::MAX_INTENSITY:
        func = max;
        break;
      case ChannelLoaderSettings::ZProjection::MIN_INTENSITY:
        func = min;
        break;
      case ChannelLoaderSettings::ZProjection::AVG_INTENSITY:
        func = avg;
        break;
      case ChannelLoaderSettings::ZProjection::NONE:
        break;
    }

    for(int zIdx = 1; zIdx < zStacksToLoad; zIdx++) {
      func(zIdx);
    }
  }
}

}    // namespace joda::cmd::functions
