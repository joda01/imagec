///
/// \file      image_processor.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Does image preprocessing, object detection and filtering
///

#pragma once

#include <filesystem>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include "../duration_count/duration_count.h"
#include "../helper/helper.hpp"
#include "../image_reader/image_reader.hpp"
#include "../image_reader/jpg/image_loader_jpg.hpp"
#include "../image_reader/tif/image_loader_tif.hpp"
#include "../reporting/reporting.h"
#include "../settings/analze_settings_parser.hpp"
#include "../settings/preprocessing_settings.hpp"
#include "object_detection/detection.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::algo {

static constexpr int64_t MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE = 71680768;
static constexpr int64_t TILES_TO_LOAD_PER_RUN          = 36;
static constexpr int32_t TIME_FRAME                     = 0;

struct ChannelProperties
{
  ImageProperties props;
  bool isJpg;
  std::set<uint32_t> tifDirs;
};

class TiffLoaderEntireWrapper
{
public:
  static cv::Mat loadImage(const std::string &filename, uint16_t directory, int offset, int nrOfTilesToRead)
  {
    return TiffLoader::loadEntireImage(filename, directory);
  }
};

class TiffLoaderTileWrapper
{
public:
  static cv::Mat loadImage(const std::string &filename, uint16_t directory, int offset, int nrOfTilesToRead)
  {
    return TiffLoader::loadImageTile(filename, directory, offset, nrOfTilesToRead);
  }
};

class JpgLoaderEntireWrapper
{
public:
  static cv::Mat loadImage(const std::string &filename, uint16_t directory, int offset, int nrOfTilesToRead)
  {
    return JpgLoader::loadEntireImage(filename);
  }
};

//
// Concept detection classes
//
template <class T>
concept detection_t = std::is_base_of<joda::algo::Detection, T>::value;

//
// Concept for image loader classes
//
template <class T>
concept image_loader_t =
    std::is_base_of<TiffLoaderEntireWrapper, T>::value || std::is_base_of<TiffLoaderTileWrapper, T>::value ||
    std::is_base_of<JpgLoaderEntireWrapper, T>::value;

///< Processing result. Key is the image tile index, value os the detection result of this tile

template <detection_t ALGORITHM>
class ImageProcessor
{
public:
  ///
  /// \brief      Executed the algorithm and generates reporting
  /// \author     Joachim Danmayr
  ///
  static auto executeAlgorithm(const std::string &imagePath,
                               const joda::settings::json::ChannelSettings &channelSetting, uint64_t tileIndex)
  {
    //
    // Execute the algorithms
    //
    ChannelProperties chProps = loadChannelProperties(imagePath, channelSetting);
    if(chProps.props.imageSize > MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE) {
      return processImage<TiffLoaderTileWrapper>(imagePath, channelSetting, chProps.tifDirs, tileIndex);
    }
    if(chProps.isJpg) {
      return processImage<JpgLoaderEntireWrapper>(imagePath, channelSetting, chProps.tifDirs, 0);
    }
    return processImage<TiffLoaderEntireWrapper>(imagePath, channelSetting, chProps.tifDirs, 0);
  }

private:
  ///
  /// \brief      Does z-projection, image preprocessing, object detection
  ///             and object filtering for the given image
  /// \author     Joachim Danmayr
  /// \param[in]  imagePath   Path of the original image
  /// \param[in]  tiffDirectories TIFF directories of the Z-Stack
  /// \param[in]  idx tile index [0... nrOfTiles]
  /// \return     Processed image
  ///
  template <image_loader_t TIFFLOADER>
  static func::DetectionResponse processImage(const std::string &imagePath,
                                              const joda::settings::json::ChannelSettings &channelSetting,
                                              const std::set<uint32_t> &tiffDirectories, int64_t idx)
  {
    auto id             = DurationCount::start("z-projection");
    cv::Mat image       = doZProjection<TIFFLOADER>(imagePath, channelSetting, tiffDirectories, idx);
    cv::Mat originalImg = image.clone();
    DurationCount::stop(id);

    id = DurationCount::start("preprocessing");
    doPreprocessing(image, channelSetting);
    DurationCount::stop(id);

    id                   = DurationCount::start("detection");
    auto detectionResult = doDetection(image, originalImg, channelSetting);
    DurationCount::stop(id);

    id = DurationCount::start("filtering");
    doFiltering(detectionResult, channelSetting);
    detectionResult.originalImage = std::move(originalImg);
    DurationCount::stop(id);

    return detectionResult;
  }

  ///,
  /// \brief      Does a Z-Projection of the image
  ///             This template function allows to load tiled and entire images
  ///             using the TiffLoaderEntireWrapper or TiffLoaderTileWrapper class
  /// \author     Joachim Danmayr
  /// \param[in]  imagePath   Path of the original image
  /// \param[in]  tiffDirectories TIFF directories of the Z-Stack
  /// \param[in]  idx tile index [0... nrOfTiles]
  /// \return     Processed image
  ///
  template <class TIFFLOADER>
  static cv::Mat doZProjection(const std::string &imagePath,
                               const joda::settings::json::ChannelSettings &channelSetting,
                               const std::set<uint32_t> &tiffDirectories, int64_t idx)
  {
    auto id           = DurationCount::start("load");
    auto actDirectory = tiffDirectories.begin();
    cv::Mat tilePart  = TIFFLOADER::loadImage(imagePath, *actDirectory, idx, TILES_TO_LOAD_PER_RUN);
    DurationCount::stop(id);

    if(channelSetting.getZProjectionSetting() == PreprocessingZStack::MAX_INTENSITY) {
      //
      // Do maximum intensity projection
      //
      while(actDirectory != tiffDirectories.end()) {
        actDirectory = std::next(actDirectory);
        if(actDirectory == tiffDirectories.end()) {
          break;
        }
        cv::max(tilePart, TIFFLOADER::loadImage(imagePath, *actDirectory, idx, TILES_TO_LOAD_PER_RUN), tilePart);
      }
    }
    return tilePart;
  }

  ///
  /// \brief      Does some preprocessing with the image
  /// \author     Joachim Danmayr
  /// \param[in,out]  image  Image to preprocess and returns the preprocessed image
  ///
  static void doPreprocessing(cv::Mat &image, const joda::settings::json::ChannelSettings &channelSetting)
  {
    auto preprocessing = channelSetting.getPreprocessingFunctions();
    for(auto const &func : preprocessing) {
      func.execute(image);
    }
  }

  ///
  /// \brief      Executes the detection algorithm
  /// \author     Joachim Danmayr
  /// \param[in]  originalImage  Image the detection should be executed on
  /// \return     Detection results with control image
  ///
  static func::DetectionResponse doDetection(const cv::Mat &image, const cv::Mat &originalImg,
                                             const joda::settings::json::ChannelSettings &channelSetting)
  {
    ALGORITHM algo;
    auto ret = algo.execute(image, originalImg, channelSetting);
    return ret;
  }

  ///
  /// \brief      Does some filtering and returns the newly detection response
  /// \author     Joachim Danmayr
  /// \param[in,out]  detectionResult  Detection result and removes the filtered objects from the detection results
  ///
  static void doFiltering(func::DetectionResponse &detectionResult,
                          const joda::settings::json::ChannelSettings &channelSetting)
  {
  }

  ///
  /// \brief      Load channel properties
  /// \author     Joachim Danmayr
  ///
  static ChannelProperties loadChannelProperties(const std::string &imagePath,
                                                 const joda::settings::json::ChannelSettings &channelSetting)
  {
    //
    // Load image properties
    //
    uint32_t channel = channelSetting.getChannelInfo().getChannelIndex();
    std::filesystem::path path_obj(imagePath);
    std::string filename = path_obj.filename().stem().string();
    ImageProperties imgProperties;
    bool isJpg = imagePath.ends_with(".jpg");
    auto id    = DurationCount::start("load img properties");
    std::set<uint32_t> tiffDirectories;
    if(isJpg) {
      imgProperties = JpgLoader::getImageProperties(imagePath);
    } else {
      auto omeInfo    = TiffLoader::getOmeInformation(imagePath);
      tiffDirectories = omeInfo.getDirectoryForChannel(channel, TIME_FRAME);
      if(tiffDirectories.empty()) {
        throw std::runtime_error("Selected channel does not contain images!");
      }
      imgProperties = TiffLoader::getImageProperties(imagePath, *tiffDirectories.begin());
    }
    DurationCount::stop(id);

    return ChannelProperties{.props = imgProperties, .isJpg = isJpg, .tifDirs = tiffDirectories};
  }
};
}    // namespace joda::algo
