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
#include "backend/image_processing/functions/blur/blur.hpp"
#include "backend/image_processing/functions/blur_gausian/blur_gausian.hpp"
#include "backend/image_processing/functions/edge_detection/edge_detection.hpp"
#include "backend/image_processing/functions/median_substraction/median_substraction.hpp"
#include "backend/image_processing/functions/rolling_ball/rolling_ball.hpp"

namespace joda::algo {

///
/// \brief      Executed the algorithm and generates reporting
/// \author     Joachim Danmayr
///
func::DetectionResponse ImageProcessor::executeAlgorithm(
    const FileInfo &imagePath, const joda::settings::ChannelSettings &channelSetting, uint64_t tileIndex,
    const std::map<std::string, joda::onnx::OnnxParser::Data> &onnxModels,
    const std::map<joda::settings::ChannelIndex, joda::func::DetectionResponse> *const referenceChannelResults)
{
  //
  // Execute the algorithms
  //
  ChannelProperties chProps =
      loadChannelProperties(imagePath, channelSetting.meta.channelIdx, channelSetting.meta.series);
  if(chProps.props.imageSize > MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE && imagePath.getDecoder() == FileInfo::Decoder::TIFF) {
    return processImage<TiffLoaderTileWrapper>(imagePath, channelSetting, chProps.tifDirs, tileIndex,
                                               referenceChannelResults, onnxModels);
  }
  if(imagePath.getDecoder() == FileInfo::Decoder::JPG) {
    return processImage<JpgLoaderEntireWrapper>(imagePath, channelSetting, chProps.tifDirs, 0, referenceChannelResults,
                                                onnxModels);
  }
  if(imagePath.getDecoder() == FileInfo::Decoder::TIFF) {
    return processImage<TiffLoaderEntireWrapper>(imagePath, channelSetting, chProps.tifDirs, 0, referenceChannelResults,
                                                 onnxModels);
  }
  return processImage<BioformatsEntireWrapper>(imagePath, channelSetting, chProps.tifDirs, 0, referenceChannelResults,
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
  auto id             = DurationCount::start("z-projection");
  cv::Mat image       = doZProjection<TIFFLOADER>(imagePath, channelSetting, tiffDirectories, idx);
  cv::Mat originalImg = image.clone();
  DurationCount::stop(id);

  id = DurationCount::start("preprocessing");
  doPreprocessingPipeline<TIFFLOADER>(image, imagePath, channelSetting, tiffDirectories, idx);
  DurationCount::stop(id);

  id                   = DurationCount::start("detection");
  auto detectionResult = doDetection(image, originalImg, channelSetting, onnxModels);
  DurationCount::stop(id);

  id = DurationCount::start("filtering");
  doFiltering(detectionResult, channelSetting, referenceChannelResults);
  detectionResult.originalImage = std::move(originalImg);
  DurationCount::stop(id);

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
  auto id          = DurationCount::start("load");
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
      ChannelProperties chPropsToSubtract =
          loadChannelProperties(imagePath.getPath(), pipelineStep.$subtractChannel->channelIdx, series);
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
  if(nullptr != referenceChannelResults) {
    auto referenceSpotChannelIndex = channelSetting.filter.referenceSpotChannelIndex;
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
ChannelProperties ImageProcessor::loadChannelProperties(const FileInfo &imagePath,
                                                        joda::settings::ChannelIndex channelIndex, uint16_t series)
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
      tiffDirectories = omeInfo.getDirectoryForChannel(static_cast<int32_t>(channelIndex), TIME_FRAME);
      if(tiffDirectories.empty()) {
        throw std::runtime_error("Selected channel does not contain images!");
      }
      imgProperties = TiffLoader::getImageProperties(imagePath.getPath(), *tiffDirectories.begin());
    } break;
    case FileInfo::Decoder::BIOFORMATS: {
      auto [omeInfo, props] = BioformatsLoader::getOmeInformation(imagePath.getPath(), series);
      tiffDirectories       = omeInfo.getDirectoryForChannel(static_cast<int32_t>(channelIndex), TIME_FRAME);
      if(tiffDirectories.empty()) {
        throw std::runtime_error("Selected channel does not contain images!");
      }
      imgProperties = props;
    } break;
  }

  DurationCount::stop(id);
  return ChannelProperties{.props = imgProperties, .tifDirs = tiffDirectories};
}

}    // namespace joda::algo