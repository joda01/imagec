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
#include "backend/duration_count/duration_count.h"
#include "backend/image_processing/functions/blur/blur.hpp"
#include "backend/image_processing/functions/blur_gausian/blur_gausian.hpp"
#include "backend/image_processing/functions/edge_detection/edge_detection.hpp"
#include "backend/image_processing/functions/median_substraction/median_substraction.hpp"
#include "backend/image_processing/functions/rolling_ball/rolling_ball.hpp"
#include "backend/image_processing/roi/roi.hpp"
#include "backend/settings/channel/channel_settings_image_filter.hpp"

namespace joda::algo {

///
/// \brief      Executed the algorithm and generates reporting
/// \author     Joachim Danmayr
///
func::DetectionResponse ImageProcessor::executeAlgorithm(
    const FileInfo &imagePath, const joda::settings::ChannelSettings &channelSetting, uint64_t tileIndex,
    const std::map<std::string, joda::onnx::OnnxParser::Data> &onnxModels, const ChannelProperties *channelProperties,
    const std::map<joda::settings::ChannelIndex, joda::func::DetectionResponse> *const referenceChannelResults)
{
  //
  // Execute the algorithms
  //
  ChannelProperties chProps;
  if(nullptr == channelProperties) {
    chProps = loadChannelProperties(imagePath, channelSetting.meta.series);
  } else {
    chProps = *channelProperties;
  }

  auto tifDirs = getTifDirs(chProps, channelSetting.meta.channelIdx);
  if(chProps.props.imageSize > MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE && imagePath.getDecoder() == FileInfo::Decoder::TIFF) {
    return processImage<TiffLoaderTileWrapper>(imagePath, channelSetting, tifDirs, tileIndex, referenceChannelResults,
                                               onnxModels);
  }
  if(imagePath.getDecoder() == FileInfo::Decoder::JPG) {
    return processImage<JpgLoaderEntireWrapper>(imagePath, channelSetting, tifDirs, 0, referenceChannelResults,
                                                onnxModels);
  }
  if(imagePath.getDecoder() == FileInfo::Decoder::TIFF) {
    return processImage<TiffLoaderEntireWrapper>(imagePath, channelSetting, tifDirs, 0, referenceChannelResults,
                                                 onnxModels);
  }
  return processImage<BioformatsEntireWrapper>(imagePath, channelSetting, tifDirs, 0, referenceChannelResults,
                                               onnxModels);
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
func::DetectionResponse ImageProcessor::processImage(
    const FileInfo &imagePath, const joda::settings::ChannelSettings &channelSetting,
    const std::set<uint32_t> &tiffDirectories, int64_t idx,
    const std::map<joda::settings::ChannelIndex, joda::func::DetectionResponse> *const referenceChannelResults,
    const std::map<std::string, joda::onnx::OnnxParser::Data> &onnxModels)
{
  cv::Mat image       = doZProjection<TIFFLOADER>(imagePath, channelSetting, tiffDirectories, idx);
  cv::Mat originalImg = image.clone();

  doPreprocessingPipeline<TIFFLOADER>(image, imagePath, channelSetting, tiffDirectories, idx);

  auto detectionResult = doDetection(image, originalImg, channelSetting, onnxModels);

  doFiltering(detectionResult, channelSetting, referenceChannelResults);
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
cv::Mat ImageProcessor::loadTileAndToIntensityProjectionIfEnabled(const FileInfo &imagePath, int64_t idx,
                                                                  const joda::settings::ChannelSettings &channelSetting,
                                                                  const std::set<uint32_t> &tiffDirectories)
{
  auto series = channelSetting.meta.series;

  auto actDirectory = tiffDirectories.begin();
  cv::Mat tilePart  = TIFFLOADER::loadImage(imagePath.getPath(), *actDirectory, series, idx, TILES_TO_LOAD_PER_RUN);
  if(channelSetting.preprocessing.$zStack.method == joda::settings::ZStackProcessing::ZStackMethod::MAX_INTENSITY) {
    //
    // Do maximum intensity projection
    //
    while(actDirectory != tiffDirectories.end()) {
      actDirectory = std::next(actDirectory);
      if(actDirectory == tiffDirectories.end()) {
        break;
      }
      cv::max(tilePart, TIFFLOADER::loadImage(imagePath.getPath(), *actDirectory, series, idx, TILES_TO_LOAD_PER_RUN),
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
cv::Mat ImageProcessor::doZProjection(const FileInfo &imagePath, const joda::settings::ChannelSettings &channelSetting,
                                      const std::set<uint32_t> &tifDirs, int64_t idx)
{
  auto id          = DurationCount::start("Zprojection");
  uint16_t series  = channelSetting.meta.series;
  cv::Mat tilePart = loadTileAndToIntensityProjectionIfEnabled<TIFFLOADER>(imagePath, idx, channelSetting, tifDirs);
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
void ImageProcessor::doPreprocessingPipeline(cv::Mat &image, const FileInfo &imagePath,
                                             const joda::settings::ChannelSettings &channelSetting,
                                             const std::set<uint32_t> &tifDirs, int64_t idx)
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
          loadTileAndToIntensityProjectionIfEnabled<TIFFLOADER>(imagePath, idx, channelSetting, tifDirs);

      image = image - tileToSubtract;
    }

    //
    // Edge detection
    //
    if(pipelineStep.$edgeDetection.has_value() &&
       pipelineStep.$subtractChannel->channelIdx != joda::settings::ChannelIndex::NONE) {
      joda::func::img::EdgeDetection algo(pipelineStep.$edgeDetection.value());
      algo.execute(image);
    }

    //
    // Gaussian blur
    //
    if(pipelineStep.$gaussianBlur.has_value() &&
       pipelineStep.$subtractChannel->channelIdx != joda::settings::ChannelIndex::NONE) {
      joda::func::img::GaussianBlur algo(pipelineStep.$gaussianBlur.value());
      algo.execute(image);
    }

    //
    // Median subtract
    //
    if(pipelineStep.$medianSubtract.has_value()) {
      joda::func::img::MedianSubtraction algo(pipelineStep.$medianSubtract.value());
      algo.execute(image);
    }

    //
    // Rolling ball
    //
    if(pipelineStep.$rollingBall.has_value()) {
      joda::func::img::RollingBallBackground algo(pipelineStep.$rollingBall.value());
      algo.execute(image);
    }

    //
    // Smoothing
    //
    if(pipelineStep.$blur.has_value()) {
      joda::func::img::Blur algo(pipelineStep.$blur.value());
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
func::DetectionResponse
ImageProcessor::doDetection(const cv::Mat &image, const cv::Mat &originalImg,
                            const joda::settings::ChannelSettings &channelSetting,
                            const std::map<std::string, joda::onnx::OnnxParser::Data> &onnxModels)
{
  ::joda::pipeline::detection::ObjectSegmentation algo(onnxModels);
  auto ret = algo.execute(image, originalImg, channelSetting);
  return ret;
}

///
/// \brief      Does some filtering and returns the newly detection response
/// \author     Joachim Danmayr
/// \param[in,out]  detectionResult  Detection result and removes the filtered objects from the detection results
///
void ImageProcessor::doFiltering(
    func::DetectionResponse &detectionResult, const joda::settings::ChannelSettings &channelSetting,
    const std::map<joda::settings::ChannelIndex, joda::func::DetectionResponse> *const referenceChannelResults)
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
    if(channelSetting.imageFilter.maxParticleNumber > 0 &&
       detectionResult.result.size() > channelSetting.imageFilter.maxParticleNumber) {
      detectionResult.responseValidity = func::ResponseDataValidity::POSSIBLE_NOISE;
    }
  }
  DurationCount::stop(id);
}

///
/// \brief      Generate the control image
/// \author     Joachim Danmayr
///
void ImageProcessor::generateControlImage(func::DetectionResponse &detectionResult, const std::string &areaColor,
                                          const joda::onnx::OnnxParser::Data &onnxModels,
                                          joda::settings::DetectionSettings::DetectionMode mode)
{
  /*
  cv::Mat &img, const DetectionResults &result,
                                 const joda::onnx::OnnxParser::Data &modelInfo, const std::string &fillColor,
                                 bool paintRectangel, bool paintLabels
  */
  if(mode == joda::settings::DetectionSettings::DetectionMode::THRESHOLD) {
    joda::func::DetectionFunction::paintBoundingBox(detectionResult.controlImage, detectionResult.result, {}, areaColor,
                                                    false, false);
  } else {
    joda::func::DetectionFunction::paintBoundingBox(detectionResult.controlImage, detectionResult.result, onnxModels,
                                                    areaColor, true, true);
  }
}

///
/// \brief      Load channel properties
/// \author     Joachim Danmayr
///
ChannelProperties ImageProcessor::loadChannelProperties(const FileInfo &imagePath, uint16_t series)
{
  //
  // Load image properties
  //
  ImageProperties imgProperties;
  joda::ome::OmeInfo omeInfo;
  switch(imagePath.getDecoder()) {
    case FileInfo::Decoder::JPG: {
      imgProperties = JpgLoader::getImageProperties(imagePath.getPath());
    } break;
    case FileInfo::Decoder::TIFF: {
      omeInfo       = TiffLoader::getOmeInformation(imagePath.getPath());
      imgProperties = TiffLoader::getImageProperties(imagePath.getPath(), 0);
    } break;
    case FileInfo::Decoder::BIOFORMATS: {
      auto [ome, props] = BioformatsLoader::getOmeInformation(imagePath.getPath(), series);
      omeInfo           = ome;
      imgProperties     = props;
    } break;
  }

  return ChannelProperties{.props = imgProperties, .ome = omeInfo};
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

}    // namespace joda::algo
