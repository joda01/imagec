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
#include "backend/pipelines/pipeline_detection/object_segmentation/object_segmentation.hpp"
#include "backend/settings/channel/channel_settings.hpp"
#include "backend/settings/detection/detection_settings.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::algo {

static constexpr int64_t MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE = 71680768;
static constexpr int64_t TILES_TO_LOAD_PER_RUN          = 36;
static constexpr int32_t TIME_FRAME                     = 0;

struct ChannelProperties
{
  ImageProperties props;
  joda::ome::OmeInfo ome;
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
// Concept for image loader classes
//
template <class T>
concept image_loader_t =
    std::is_base_of<TiffLoaderEntireWrapper, T>::value || std::is_base_of<TiffLoaderTileWrapper, T>::value ||
    std::is_base_of<JpgLoaderEntireWrapper, T>::value || std::is_base_of<BioformatsEntireWrapper, T>::value;

///< Processing result. Key is the image tile index, value os the detection result of this tile

class ImageProcessor
{
public:
  /////////////////////////////////////////////////////
  static func::DetectionResponse executeAlgorithm(
      const FileInfo &imagePath, const joda::settings::ChannelSettings &channelSetting, uint64_t tileIndex,
      const std::map<std::string, joda::onnx::OnnxParser::Data> &onnxModels,
      const ChannelProperties *channelProperties = nullptr,
      const std::map<joda::settings::ChannelIndex, joda::func::DetectionResponse> *const referenceChannelResults =
          nullptr);

  ///
  /// \brief      Load channel properties
  /// \author     Joachim Danmayr
  ///
  static ChannelProperties loadChannelProperties(const FileInfo &imagePath, uint16_t series);

private:
  /////////////////////////////////////////////////////
  template <image_loader_t TIFFLOADER>
  static func::DetectionResponse processImage(
      const FileInfo &imagePath, const joda::settings::ChannelSettings &channelSetting,
      const std::set<uint32_t> &tiffDirectories, int64_t idx,
      const std::map<joda::settings::ChannelIndex, joda::func::DetectionResponse> *const referenceChannelResults,
      const std::map<std::string, joda::onnx::OnnxParser::Data> &onnxModels);

  template <class TIFFLOADER>
  static cv::Mat loadTileAndToIntensityProjectionIfEnabled(const FileInfo &imagePath, int64_t idx,
                                                           const joda::settings::ChannelSettings &channelSetting,
                                                           const std::set<uint32_t> &tiffDirectories);

  template <class TIFFLOADER>
  static cv::Mat doZProjection(const FileInfo &imagePath, const joda::settings::ChannelSettings &channelSetting,
                               const std::set<uint32_t> &tifDirs, int64_t idx);

  template <class TIFFLOADER>
  static void doPreprocessingPipeline(cv::Mat &image, const FileInfo &imagePath,
                                      const joda::settings::ChannelSettings &channelSetting,
                                      const std::set<uint32_t> &tifDirs, int64_t idx);

  static func::DetectionResponse doDetection(const cv::Mat &image, const cv::Mat &originalImg,
                                             const joda::settings::ChannelSettings &channelSetting,
                                             const std::map<std::string, joda::onnx::OnnxParser::Data> &onnxModels);

  static void doFiltering(
      const cv::Mat &originalImg, func::DetectionResponse &detectionResult,
      const joda::settings::ChannelSettings &channelSetting,
      const std::map<joda::settings::ChannelIndex, joda::func::DetectionResponse> *const referenceChannelResults);

  ///
  /// \brief      Generate the control image
  /// \author     Joachim Danmayr
  ///
  static void generateControlImage(func::DetectionResponse &detectionResult, const std::string &areaColor,
                                   const joda::onnx::OnnxParser::Data &onnxModels,
                                   joda::settings::DetectionSettings::DetectionMode mode);

  static std::set<uint32_t> getTifDirs(const ChannelProperties &props, joda::settings::ChannelIndex channelIndex);
};
}    // namespace joda::algo
