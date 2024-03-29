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

#include <cmath>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include "backend/duration_count/duration_count.h"
#include "backend/helper/file_info.hpp"
#include "backend/helper/helper.hpp"
#include "backend/image_processing/detection/detection.hpp"
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/image_processing/functions/watershed/watershed.hpp"
#include "backend/image_reader/bioformats/bioformats_loader.hpp"
#include "backend/image_reader/image_reader.hpp"
#include "backend/image_reader/jpg/image_loader_jpg.hpp"
#include "backend/image_reader/tif/image_loader_tif.hpp"
#include "backend/logger/console_logger.hpp"
#include "backend/pipelines/pipeline_detection/pipeline_detection.hpp"
#include "backend/settings/channel_settings.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::algo {

static constexpr int64_t MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE = 71680768;
static constexpr int64_t TILES_TO_LOAD_PER_RUN          = 36;
static constexpr int32_t TIME_FRAME                     = 0;

struct ChannelProperties
{
  ImageProperties props;
  std::set<uint32_t> tifDirs;
};

class TiffLoaderEntireWrapper
{
public:
  static cv::Mat loadImage(const std::string &filename, uint16_t directory, uint16_t series, int offset,
                           int nrOfTilesToRead)
  {
    return TiffLoader::loadEntireImage(filename, directory);
  }
};

class TiffLoaderTileWrapper
{
public:
  static cv::Mat loadImage(const std::string &filename, uint16_t directory, uint16_t series, int offset,
                           int nrOfTilesToRead)
  {
    return TiffLoader::loadImageTile(filename, directory, offset, nrOfTilesToRead);
  }
};

class JpgLoaderEntireWrapper
{
public:
  static cv::Mat loadImage(const std::string &filename, uint16_t directory, uint16_t series, int offset,
                           int nrOfTilesToRead)
  {
    return JpgLoader::loadEntireImage(filename);
  }
};

class BioformatsEntireWrapper
{
public:
  static cv::Mat loadImage(const std::string &filename, uint16_t directory, uint16_t series, int offset,
                           int nrOfTilesToRead)
  {
    return BioformatsLoader::loadEntireImage(filename, directory, series);
  }
};

//
// Concept detection classes
//
template <class T>
concept detection_t = std::is_base_of<joda::pipeline::detection::Detection, T>::value;

//
// Concept for image loader classes
//
template <class T>
concept image_loader_t =
    std::is_base_of<TiffLoaderEntireWrapper, T>::value || std::is_base_of<TiffLoaderTileWrapper, T>::value ||
    std::is_base_of<JpgLoaderEntireWrapper, T>::value || std::is_base_of<BioformatsEntireWrapper, T>::value;

///< Processing result. Key is the image tile index, value os the detection result of this tile

template <detection_t ALGORITHM>
class ImageProcessor
{
public:
  ///
  /// \brief      Executed the algorithm and generates reporting
  /// \author     Joachim Danmayr
  ///
  static auto
  executeAlgorithm(const FileInfo &imagePath, const joda::settings::json::ChannelSettings &channelSetting,
                   uint64_t tileIndex,
                   const std::map<int32_t, joda::func::DetectionResponse> *const referenceChannelResults = nullptr)
  {
    //
    // Execute the algorithms
    //
    ChannelProperties chProps = loadChannelProperties(imagePath, channelSetting.getChannelInfo().getChannelIndex(),
                                                      channelSetting.getChannelInfo().getChannelSeries());
    if(chProps.props.imageSize > MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE && imagePath.getDecoder() == FileInfo::Decoder::TIFF) {
      return processImage<TiffLoaderTileWrapper>(imagePath, channelSetting, chProps.tifDirs, tileIndex,
                                                 referenceChannelResults);
    }
    if(imagePath.getDecoder() == FileInfo::Decoder::JPG) {
      return processImage<JpgLoaderEntireWrapper>(imagePath, channelSetting, chProps.tifDirs, 0,
                                                  referenceChannelResults);
    }
    if(imagePath.getDecoder() == FileInfo::Decoder::TIFF) {
      return processImage<TiffLoaderEntireWrapper>(imagePath, channelSetting, chProps.tifDirs, 0,
                                                   referenceChannelResults);
    }
    return processImage<BioformatsEntireWrapper>(imagePath, channelSetting, chProps.tifDirs, 0,
                                                 referenceChannelResults);
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
  static func::DetectionResponse
  processImage(const FileInfo &imagePath, const joda::settings::json::ChannelSettings &channelSetting,
               const std::set<uint32_t> &tiffDirectories, int64_t idx,
               const std::map<int32_t, joda::func::DetectionResponse> *const referenceChannelResults)
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
    doFiltering(detectionResult, channelSetting, referenceChannelResults);
    detectionResult.originalImage = std::move(originalImg);
    DurationCount::stop(id);

    generateControlImage(detectionResult, channelSetting.getChannelInfo().getColor());

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
  static cv::Mat doZProjection(const FileInfo &imagePath, const joda::settings::json::ChannelSettings &channelSetting,
                               const std::set<uint32_t> &tifDirs, int64_t idx)
  {
    auto id         = DurationCount::start("load");
    uint16_t series = channelSetting.getChannelInfo().getChannelSeries();

    auto loadTileAndToIntensityProjectionIfEnabled =
        [&imagePath, idx, &series, &channelSetting](const std::set<uint32_t> &tiffDirectories) -> cv::Mat {
      auto actDirectory = tiffDirectories.begin();
      cv::Mat tilePart  = TIFFLOADER::loadImage(imagePath.getPath(), *actDirectory, series, idx, TILES_TO_LOAD_PER_RUN);
      if(channelSetting.getZProjectionSetting() == PreprocessingZStack::MAX_INTENSITY) {
        //
        // Do maximum intensity projection
        //
        while(actDirectory != tiffDirectories.end()) {
          actDirectory = std::next(actDirectory);
          if(actDirectory == tiffDirectories.end()) {
            break;
          }
          cv::max(tilePart,
                  TIFFLOADER::loadImage(imagePath.getPath(), *actDirectory, series, idx, TILES_TO_LOAD_PER_RUN),
                  tilePart);
        }
      }
      return tilePart;
    };

    cv::Mat tilePart = loadTileAndToIntensityProjectionIfEnabled(tifDirs);

    //
    // If channel subtraction is enabled, subtract the channel from the other one
    //
    if(channelSetting.getPreprocessingSubtractChannel() >= 0) {
      ChannelProperties chPropsToSubtract =
          loadChannelProperties(imagePath.getPath(), channelSetting.getPreprocessingSubtractChannel(), series);
      cv::Mat tileToSubtract = loadTileAndToIntensityProjectionIfEnabled(chPropsToSubtract.tifDirs);

      tilePart = tilePart - tileToSubtract;
    }
    DurationCount::stop(id);

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
      auto id = DurationCount::start("preprocessing step");

      func.execute(image);
      DurationCount::stop(id);
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
                          const joda::settings::json::ChannelSettings &channelSetting,
                          const std::map<int32_t, joda::func::DetectionResponse> *const referenceChannelResults)
  {
    if(nullptr != referenceChannelResults) {
      int32_t referenceSpotChannelIndex = channelSetting.getFilter().getReferenceSpotChannelIndex();
      if(referenceSpotChannelIndex >= 0) {
        auto referenceSpotChannel = referenceChannelResults->find(referenceSpotChannelIndex);
        if(referenceSpotChannel != referenceChannelResults->end()) {
          //
          // Remove reference spots
          //
          for(auto const &referenceRoi : referenceSpotChannel->second.result) {
            for(auto &spot : detectionResult.result) {
              if(referenceRoi.isValid() && spot.isValid()) {
                auto isIntersecting = referenceRoi.isIntersecting(spot, 0.7);
                if(isIntersecting) {
                  spot.setValidity(func::ParticleValidity::REFERENCE_SPOT);
                  break;
                }
              }
            }
          }
        } else {
          joda::log::logWarning("A reference channel index was selected which is not part of the channel list.");
        }
      }
    }
  }

  ///
  /// \brief      Generate the control image
  /// \author     Joachim Danmayr
  ///
  static void generateControlImage(func::DetectionResponse &detectionResult, const std::string &areaColor)
  {
    joda::func::DetectionFunction::paintBoundingBox(detectionResult.controlImage, detectionResult.result, areaColor);
  }

  ///
  /// \brief      Load channel properties
  /// \author     Joachim Danmayr
  ///
  static ChannelProperties loadChannelProperties(const FileInfo &imagePath, int channelIndex, uint16_t series)
  {
    //
    // Load image properties
    //
    ImageProperties imgProperties;
    auto id = DurationCount::start("load img properties");
    std::set<uint32_t> tiffDirectories;
    switch(imagePath.getDecoder()) {
      case FileInfo::Decoder::JPG: {
        imgProperties = JpgLoader::getImageProperties(imagePath.getPath());
      } break;
      case FileInfo::Decoder::TIFF: {
        auto omeInfo    = TiffLoader::getOmeInformation(imagePath.getPath());
        tiffDirectories = omeInfo.getDirectoryForChannel(channelIndex, TIME_FRAME);
        if(tiffDirectories.empty()) {
          throw std::runtime_error("Selected channel does not contain images!");
        }
        imgProperties = TiffLoader::getImageProperties(imagePath.getPath(), *tiffDirectories.begin());
      } break;
      case FileInfo::Decoder::BIOFORMATS: {
        auto [omeInfo, props] = BioformatsLoader::getOmeInformation(imagePath.getPath(), series);
        tiffDirectories       = omeInfo.getDirectoryForChannel(channelIndex, TIME_FRAME);
        if(tiffDirectories.empty()) {
          throw std::runtime_error("Selected channel does not contain images!");
        }
        imgProperties = props;
      } break;
    }

    DurationCount::stop(id);
    return ChannelProperties{.props = imgProperties, .tifDirs = tiffDirectories};
  }
};
}    // namespace joda::algo
