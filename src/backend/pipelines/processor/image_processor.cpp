///
/// \file      image_processor.cpp
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

#include "image_processor.hpp"
#include <cstdint>
#include <string>
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/file_info.hpp"
#include "backend/helper/file_info_images.hpp"
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/image_processing/functions/blur/blur.hpp"
#include "backend/image_processing/functions/blur_gausian/blur_gausian.hpp"
#include "backend/image_processing/functions/edge_detection/edge_detection.hpp"
#include "backend/image_processing/functions/median_substraction/median_substraction.hpp"
#include "backend/image_processing/functions/rolling_ball/rolling_ball.hpp"
#include "backend/image_processing/roi/roi.hpp"
#include "backend/pipelines/processor/histogram_filter.hpp"
#include "backend/settings/channel/channel_settings_image_filter.hpp"
#include "backend/settings/detection/detection_settings.hpp"

namespace joda::pipeline {

///
/// \brief      Executed the algorithm and generates reporting
/// \author     Joachim Danmayr
///
image::detect::DetectionResponse ImageProcessor::executeAlgorithm(
    const helper::fs::FileInfoImages &imagePath, const joda::settings::ChannelSettings &channelSetting,
    uint64_t tileIndex, uint16_t resolution, const std::map<std::string, joda::onnx::OnnxParser::Data> &onnxModels,
    const ChannelProperties *channelProperties,
    const std::map<joda::settings::ChannelIndex, joda::image::detect::DetectionResponse> *const referenceChannelResults)
{
  //
  // Execute the algorithms
  //
  ChannelProperties chProps;
  if(nullptr == channelProperties) {
    chProps = ImageProcessor::loadChannelProperties(imagePath, channelSetting.meta.series);
  } else {
    chProps = *channelProperties;
  }

  auto tifDirs = ImageProcessor::getTifDirs(chProps, channelSetting.meta.channelIdx);
  if(chProps.ome.getImageInfo().resolutions.at(resolution).imageMemoryUsage > MAX_IMAGE_SIZE_BYTES_TO_LOAD_AT_ONCE &&
     (imagePath.getDecoder() == helper::fs::FileInfoImages::Decoder::TIFF ||
      imagePath.getDecoder() == helper::fs::FileInfoImages::Decoder::BIOFORMATS)) {
    return processImage<TiffLoaderTileWrapper>(imagePath, channelSetting, tifDirs, tileIndex, resolution,
                                               referenceChannelResults, onnxModels);
  }
  if(imagePath.getDecoder() == helper::fs::FileInfoImages::Decoder::JPG) {
    return processImage<JpgLoaderEntireWrapper>(imagePath, channelSetting, tifDirs, 0, resolution,
                                                referenceChannelResults, onnxModels);
  }
  if(imagePath.getDecoder() == helper::fs::FileInfoImages::Decoder::TIFF) {
    return processImage<TiffLoaderEntireWrapper>(imagePath, channelSetting, tifDirs, 0, resolution,
                                                 referenceChannelResults, onnxModels);
  }
  return processImage<BioformatsEntireWrapper>(imagePath, channelSetting, tifDirs, 0, resolution,
                                               referenceChannelResults, onnxModels);
}

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
image::detect::DetectionResponse ImageProcessor::processImage(
    const helper::fs::FileInfoImages &imagePath, const joda::settings::ChannelSettings &channelSetting,
    const std::set<uint32_t> &tiffDirectories, int64_t idx, uint16_t resolution,
    const std::map<joda::settings::ChannelIndex, joda::image::detect::DetectionResponse> *const referenceChannelResults,
    const std::map<std::string, joda::onnx::OnnxParser::Data> &onnxModels)
{
  cv::Mat image       = doZProjection<TIFFLOADER>(imagePath, channelSetting, tiffDirectories, idx, resolution);
  cv::Mat originalImg = image.clone();

  doPreprocessingPipeline<TIFFLOADER>(image, imagePath, channelSetting, tiffDirectories, idx, resolution);

  auto detectionResult = doDetection(image, originalImg, channelSetting, onnxModels);

  doFiltering(originalImg, detectionResult, channelSetting, referenceChannelResults);
  detectionResult.originalImage = std::move(originalImg);

  auto modelInfo = onnxModels.find(channelSetting.detection.ai.modelPath);
  if(modelInfo != onnxModels.end()) {
    generateControlImage(detectionResult, channelSetting.meta.color, modelInfo->second,
                         channelSetting.detection.detectionMode);
  } else {
    generateControlImage(detectionResult, channelSetting.meta.color, {}, channelSetting.detection.detectionMode);
  }

  return detectionResult;
}

///,
/// \brief      Load tile and to intensity projection if enabled
/// \author     Joachim Danmayr
/// \param[in]  imagePath   Path of the original image
/// \param[in]  tiffDirectories TIFF directories of the Z-Stack
/// \param[in]  idx tile index [0... nrOfTiles]
/// \return     Processed image
///
template <class TIFFLOADER>
cv::Mat ImageProcessor::loadTileAndToIntensityProjectionIfEnabled(const helper::fs::FileInfoImages &imagePath,
                                                                  int64_t idx,
                                                                  const joda::settings::ChannelSettings &channelSetting,
                                                                  const std::set<uint32_t> &tiffDirectories,
                                                                  uint16_t resolution)
{
  auto series = channelSetting.meta.series;

  auto actDirectory = tiffDirectories.begin();
  cv::Mat tilePart  = TIFFLOADER::loadImage(imagePath.getFilePath().string(), *actDirectory, series, idx,
                                            TILES_TO_LOAD_PER_RUN, resolution);
  if(channelSetting.preprocessing.$zStack.method == joda::settings::ZStackProcessing::ZStackMethod::MAX_INTENSITY) {
    //
    // Do maximum intensity projection
    //
    while(actDirectory != tiffDirectories.end()) {
      actDirectory = std::next(actDirectory);
      if(actDirectory == tiffDirectories.end()) {
        break;
      }
      cv::max(tilePart,
              TIFFLOADER::loadImage(imagePath.getFilePath().string(), *actDirectory, series, idx, TILES_TO_LOAD_PER_RUN,
                                    resolution),
              tilePart);
    }
  }
  return tilePart;
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
cv::Mat ImageProcessor::doZProjection(const helper::fs::FileInfoImages &imagePath,
                                      const joda::settings::ChannelSettings &channelSetting,
                                      const std::set<uint32_t> &tifDirs, int64_t idx, uint16_t resolution)
{
  auto id         = DurationCount::start("Zprojection");
  uint16_t series = channelSetting.meta.series;
  cv::Mat tilePart =
      loadTileAndToIntensityProjectionIfEnabled<TIFFLOADER>(imagePath, idx, channelSetting, tifDirs, resolution);
  DurationCount::stop(id);

#warning "Do Margin crop here"

  return tilePart;
}

///
/// \brief      Does some preprocessing with the image
/// \author     Joachim Danmayr
/// \param[in,out]  image  Image to preprocess and returns the preprocessed image
///
template <class TIFFLOADER>
void ImageProcessor::doPreprocessingPipeline(cv::Mat &image, const helper::fs::FileInfoImages &imagePath,
                                             const joda::settings::ChannelSettings &channelSetting,
                                             const std::set<uint32_t> &tifDirs, int64_t idx, uint16_t resolution)
{
  auto series = channelSetting.meta.series;

  for(auto const &pipelineStep : channelSetting.preprocessing.pipeline) {
    //
    // Channel subtraction
    //
    if(pipelineStep.$subtractChannel.has_value() &&
       pipelineStep.$subtractChannel->channelIdx != joda::settings::ChannelIndex::NONE) {
      // ChannelProperties chPropsToSubtract =
      //     loadChannelProperties(imagePath.getPath(), pipelineStep.$subtractChannel->channelIdx, series);
      cv::Mat tileToSubtract =
          loadTileAndToIntensityProjectionIfEnabled<TIFFLOADER>(imagePath, idx, channelSetting, tifDirs, resolution);

      image = image - tileToSubtract;
    }

    //
    // Edge detection
    //
    if(pipelineStep.$edgeDetection.has_value() &&
       pipelineStep.$subtractChannel->channelIdx != joda::settings::ChannelIndex::NONE) {
      joda::image::func::EdgeDetection algo(pipelineStep.$edgeDetection.value());
      algo.execute(image);
    }

    //
    // Gaussian blur
    //
    if(pipelineStep.$gaussianBlur.has_value() &&
       pipelineStep.$subtractChannel->channelIdx != joda::settings::ChannelIndex::NONE) {
      joda::image::func::GaussianBlur algo(pipelineStep.$gaussianBlur.value());
      algo.execute(image);
    }

    //
    // Median subtract
    //
    if(pipelineStep.$medianSubtract.has_value()) {
      joda::image::func::MedianSubtraction algo(pipelineStep.$medianSubtract.value());
      algo.execute(image);
    }

    //
    // Rolling ball
    //
    if(pipelineStep.$rollingBall.has_value()) {
      joda::image::func::RollingBallBackground algo(pipelineStep.$rollingBall.value());
      algo.execute(image);
    }

    //
    // Smoothing
    //
    if(pipelineStep.$blur.has_value()) {
      joda::image::func::Blur algo(pipelineStep.$blur.value());
      algo.execute(image);
    }
  }
}

///
/// \brief      Executes the detection algorithm
/// \author     Joachim Danmayr
/// \param[in]  originalImage  Image the detection should be executed on
/// \return     Detection results with control image
///
image::detect::DetectionResponse
ImageProcessor::doDetection(const cv::Mat &image, const cv::Mat &originalImg,
                            const joda::settings::ChannelSettings &channelSetting,
                            const std::map<std::string, joda::onnx::OnnxParser::Data> &onnxModels)
{
  ObjectSegmentation algo(onnxModels);
  auto ret = algo.execute(image, originalImg, channelSetting);
  return ret;
}

///
/// \brief      Does some filtering and returns the newly detection response
/// \author     Joachim Danmayr
/// \param[in,out]  detectionResult  Detection result and removes the filtered objects from the detection results
///
void ImageProcessor::doFiltering(
    const cv::Mat &originalImg, image::detect::DetectionResponse &detectionResult,
    const joda::settings::ChannelSettings &channelSetting,
    const std::map<joda::settings::ChannelIndex, joda::image::detect::DetectionResponse> *const referenceChannelResults)
{
  //
  // Reference spot removal
  //
  auto id = DurationCount::start("Tetraspeck removal");
  if(nullptr != referenceChannelResults) {
    auto referenceSpotChannelIndex = channelSetting.objectFilter.referenceSpotChannelIndex;
    if(referenceSpotChannelIndex != joda::settings::ChannelIndex::NONE) {
      auto referenceSpotChannel = referenceChannelResults->find(referenceSpotChannelIndex);
      if(referenceSpotChannel != referenceChannelResults->end()) {
        //
        // Remove reference spots
        //
        for(auto const &referenceRoi : *referenceSpotChannel->second.result) {
          for(auto &spot : *detectionResult.result) {
            if(referenceRoi.isValid() && spot.isValid()) {
              auto isIntersecting = referenceRoi.isIntersecting(spot, 0.7);
              if(isIntersecting) {
                spot.setValidity(image::ParticleValidityEnums::REFERENCE_SPOT);
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
  DurationCount::stop(id);

  //
  // Image result plausibility check
  //
  id = DurationCount::start("Image filtering");
  if(channelSetting.imageFilter.filterMode != joda::settings::ChannelImageFilter::FilterMode::OFF) {
    detectionResult.invalidateWholeImage =
        joda::settings::ChannelImageFilter::FilterMode::INVALIDATE_WHOLE_IMAGE == channelSetting.imageFilter.filterMode;
    //
    // Filter by max particles
    //
    if(channelSetting.imageFilter.maxObjects > 0 &&
       detectionResult.result->size() > channelSetting.imageFilter.maxObjects) {
      detectionResult.responseValidity.set(
          static_cast<size_t>(image::detect::ResponseDataValidityEnum::POSSIBLE_NOISE));
    }

    //
    // Filter by threshold
    //
    joda::pipeline::applyHistogramFilter(originalImg, detectionResult, channelSetting);
  }
  DurationCount::stop(id);
}

///
/// \brief      Generate the control image
/// \author     Joachim Danmayr
///
void ImageProcessor::generateControlImage(image::detect::DetectionResponse &detectionResult,
                                          const std::string &areaColor, const joda::onnx::OnnxParser::Data &onnxModels,
                                          joda::settings::DetectionSettings::DetectionMode mode)
{
  /*
  cv::Mat &img, const DetectionResults &result,
                                 const joda::onnx::OnnxParser::Data &modelInfo, const std::string &fillColor,
                                 bool paintRectangel, bool paintLabels
  */
  if(mode == joda::settings::DetectionSettings::DetectionMode::THRESHOLD) {
    image::detect::DetectionFunction::paintBoundingBox(detectionResult.controlImage, detectionResult.result, {},
                                                       areaColor, false, false);
  } else {
    image::detect::DetectionFunction::paintBoundingBox(detectionResult.controlImage, detectionResult.result, onnxModels,
                                                       areaColor, true, true);
  }
}

///
/// \brief      Load channel properties
/// \author     Joachim Danmayr
///
ChannelProperties ImageProcessor::loadChannelProperties(const helper::fs::FileInfoImages &imagePath, uint16_t series)
{
  //
  // Load image properties
  //
  joda::ome::OmeInfo omeInfo;
  switch(imagePath.getDecoder()) {
    case helper::fs::FileInfoImages::Decoder::JPG: {
      omeInfo = image::JpgLoader::getImageProperties(imagePath.getFilePath().string());
    } break;
    case helper::fs::FileInfoImages::Decoder::TIFF: {
      omeInfo = image::BioformatsLoader::getOmeInformation(imagePath.getFilePath().string());
    } break;
    case helper::fs::FileInfoImages::Decoder::BIOFORMATS: {
      omeInfo = image::BioformatsLoader::getOmeInformation(imagePath.getFilePath().string());
    } break;
  }

  return ChannelProperties{.ome = omeInfo};
}

///
/// \brief      Load channel properties
/// \author     Joachim Danmayr
///
std::set<uint32_t> ImageProcessor::getTifDirs(const ChannelProperties &props, joda::settings::ChannelIndex channelIndex)
{
  auto tiffDirectories = props.ome.getDirectoryForChannel(static_cast<int32_t>(channelIndex), TIME_FRAME);
  if(tiffDirectories.empty()) {
    throw std::runtime_error("Selected channel does not contain images!");
  }
  return tiffDirectories;
}

}    // namespace joda::pipeline
