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
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/file_info.hpp"
#include "backend/helper/file_info_images.hpp"
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/image_processing/detection/detection.hpp"
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/image_processing/functions/watershed/watershed.hpp"
#include "backend/image_processing/reader/bioformats/bioformats_loader.hpp"
#include "backend/image_processing/reader/jpg/image_loader_jpg.hpp"
#include "backend/pipelines/pipeline_detection/object_segmentation/object_segmentation.hpp"
#include "backend/settings/channel/channel_settings.hpp"
#include "backend/settings/detection/detection_settings.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::pipeline {

static constexpr int64_t MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE = 71680768;
static constexpr int64_t TILES_TO_LOAD_PER_RUN          = 36;
static constexpr int32_t TIME_FRAME                     = 0;

struct ChannelProperties
{
  joda::ome::OmeInfo ome;
};

class TiffLoaderEntireWrapper
{
public:
  static cv::Mat loadImage(const std::string &filename, uint16_t directory, uint16_t series, int offset,
                           int nrOfTilesToRead, int resolution)
  {
    return image::BioformatsLoader::loadEntireImage(filename, directory, series, resolution);
  }
};

class TiffLoaderTileWrapper
{
public:
  static cv::Mat loadImage(const std::string &filename, uint16_t directory, uint16_t series, int offset,
                           int nrOfTilesToRead, int resolution)
  {
    return image::BioformatsLoader::loadImageTile(filename, directory, series, offset, nrOfTilesToRead, resolution);
  }
};

class JpgLoaderEntireWrapper
{
public:
  static cv::Mat loadImage(const std::string &filename, uint16_t directory, uint16_t series, int offset,
                           int nrOfTilesToRead, int resolution)
  {
    return image::JpgLoader::loadEntireImage(filename);
  }
};

class BioformatsEntireWrapper
{
public:
  static cv::Mat loadImage(const std::string &filename, uint16_t directory, uint16_t series, int offset,
                           int nrOfTilesToRead, int resolution)
  {
    return image::BioformatsLoader::loadEntireImage(filename, directory, series, resolution);
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
  static image::detect::DetectionResponse executeAlgorithm(
      const helper::fs::FileInfoImages &imagePath, const joda::settings::ChannelSettings &channelSetting,
      uint64_t tileIndex, uint16_t resolution, const std::map<std::string, joda::onnx::OnnxParser::Data> &onnxModels,
      const ChannelProperties *channelProperties = nullptr,
      const std::map<joda::settings::ChannelIndex, image::detect::DetectionResponse> *const referenceChannelResults =
          nullptr);

  ///
  /// \brief      Load channel properties
  /// \author     Joachim Danmayr
  ///
  static ChannelProperties loadChannelProperties(const helper::fs::FileInfoImages &imagePath, uint16_t series);

private:
  /////////////////////////////////////////////////////
  template <image_loader_t TIFFLOADER>
  static image::detect::DetectionResponse processImage(
      const helper::fs::FileInfoImages &imagePath, const joda::settings::ChannelSettings &channelSetting,
      const std::set<uint32_t> &tiffDirectories, int64_t idx, uint16_t resolution,
      const std::map<joda::settings::ChannelIndex, image::detect::DetectionResponse> *const referenceChannelResults,
      const std::map<std::string, joda::onnx::OnnxParser::Data> &onnxModels);

  template <class TIFFLOADER>
  static cv::Mat loadTileAndToIntensityProjectionIfEnabled(const helper::fs::FileInfoImages &imagePath, int64_t idx,
                                                           const joda::settings::ChannelSettings &channelSetting,
                                                           const std::set<uint32_t> &tiffDirectories,
                                                           uint16_t resolution);

  template <class TIFFLOADER>
  static cv::Mat doZProjection(const helper::fs::FileInfoImages &imagePath,
                               const joda::settings::ChannelSettings &channelSetting, const std::set<uint32_t> &tifDirs,
                               int64_t idx, uint16_t resolution);

  template <class TIFFLOADER>
  static void doPreprocessingPipeline(cv::Mat &image, const helper::fs::FileInfoImages &imagePath,
                                      const joda::settings::ChannelSettings &channelSetting,
                                      const std::set<uint32_t> &tifDirs, int64_t idx, uint16_t resolution);

  static image::detect::DetectionResponse
  doDetection(const cv::Mat &image, const cv::Mat &originalImg, const joda::settings::ChannelSettings &channelSetting,
              const std::map<std::string, joda::onnx::OnnxParser::Data> &onnxModels);

  static void doFiltering(const cv::Mat &originalImg, image::detect::DetectionResponse &detectionResult,
                          const joda::settings::ChannelSettings &channelSetting,
                          const std::map<joda::settings::ChannelIndex, joda::image::detect::DetectionResponse>
                              *const referenceChannelResults);

  ///
  /// \brief      Generate the control image
  /// \author     Joachim Danmayr
  ///
  static void generateControlImage(image::detect::DetectionResponse &detectionResult, const std::string &areaColor,
                                   const joda::onnx::OnnxParser::Data &onnxModels,
                                   joda::settings::DetectionSettings::DetectionMode mode);

  static std::set<uint32_t> getTifDirs(const ChannelProperties &props, joda::settings::ChannelIndex channelIndex);
};
}    // namespace joda::pipeline
