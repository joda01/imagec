///
/// \file      machine_learning.cpp
/// \author    Joachim Danmayr
/// \date      2025-10-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "machine_learning.hpp"
#include <fstream>
#include <string>
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/logger/console_logger.hpp"

namespace joda::ml {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void MachineLearning::forward(const std::filesystem::path &path, cv::Mat &image)
{
  // Load trained model
  cv::Mat predFloat;    // output (H*W) x 1, CV_32F
  predict(path, image, predFloat);

  // ===============================
  // Reshape back to segmentation mask
  // ===============================
  cv::Mat segMask = predFloat.reshape(1, image.rows);    // H x W, CV_32F
  segMask.convertTo(segMask, CV_16UC1);

  // Build binary mask (assuming class 0 = background, >0 = foreground)
  image        = cv::Mat::zeros(segMask.size(), CV_16U);
  cv::Mat mask = (segMask > 0);
  image.setTo(0, ~mask);
  segMask.copyTo(image, mask);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void MachineLearning::train(const MachineLearningSettings &settings, const cv::Mat &image, const enums::TileInfo &tileInfo,
                            const atom::ObjectList &result)
{
  cv::Mat trainSamples;
  cv::Mat labelList;
  auto trainingClasses = settings.toTrainingsClassesMap();
  const bool normalize = settings.modelTyp == ModelType::ANN_MLP;    //

  prepareTrainingDataFromROI(image, tileInfo, trainingClasses, settings.categoryToTrain, result, trainSamples, labelList,
                             settings.featureExtractionPipelines, normalize);

  train(trainSamples, labelList, static_cast<int32_t>(trainingClasses.size()), settings.outPath, settings);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void MachineLearning::prepareTrainingDataFromROI(const cv::Mat &image, const enums::TileInfo &tileInfo,
                                                 const std::map<enums::ClassId, int32_t> &classesToTrain, joda::atom::ROI::Category categoryToTain,
                                                 const atom::ObjectList &regionOfInterest, cv::Mat &trainSamples, cv::Mat &trainLabels,
                                                 const std::list<ImageCommandPipeline> &featuresSet, bool normalizeForMLP)
{
  // ====================================
  // Extract features
  // ====================================
  cv::Mat features = extractFeatures(image, featuresSet, normalizeForMLP);

  // Collect training samples from labeled ROI mask
  std::vector<int> labels;
  std::vector<int> sampleIdx;

  auto extractSamples = [&image, &sampleIdx, &labels](const cv::Mat &roiMask, int32_t classLabelIndex) {
    for(int y = 0; y < roiMask.rows; y++) {
      for(int x = 0; x < roiMask.cols; x++) {
        uint16_t lbl = roiMask.at<uint16_t>(y, x);
        int idx      = y * image.cols + x;
        if(lbl > 0) {    // skip unlabeled pixels
          sampleIdx.push_back(idx);
          labels.push_back(classLabelIndex);
        }
      }
    }
  };

  // ====================================
  // Train the individual classes
  // ====================================
  for(const auto [classIdToTrain, pixelClassId] : classesToTrain) {
    if(!regionOfInterest.contains(classIdToTrain)) {
      continue;
    }
    cv::Mat roiMask            = cv::Mat::zeros(image.size(), CV_16UC1);
    const auto &objectsToLearn = regionOfInterest.at(classIdToTrain);
    auto addedRois             = objectsToLearn->createBinaryImage(roiMask, 1, categoryToTain, tileInfo);
    if(addedRois > 0) {
      extractSamples(roiMask, pixelClassId);
    }
  }

  // ====================================
  // Convert labels and samples to cv::Mat
  // ====================================
  trainSamples = cv::Mat(static_cast<int>(sampleIdx.size()), features.cols, CV_32F);
  trainLabels  = cv::Mat(static_cast<int>(sampleIdx.size()), 1, CV_32S);

  for(int i = 0; i < static_cast<int>(sampleIdx.size()); i++) {
    features.row(sampleIdx[i]).copyTo(trainSamples.row(i));
    trainLabels.at<int>(i, 0) = labels[i];
  }

  if(trainSamples.empty() || trainLabels.empty()) {
    joda::log::logWarning("No training samples!");
    throw std::invalid_argument("No training samples! Did you created annotations?");
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Mat MachineLearning::extractFeatures(const cv::Mat &img, const std::list<ImageCommandPipeline> &features, bool normalizeForMLP)
{
  CV_Assert(!img.empty());

  cv::Mat gray;
  if(img.channels() == 3) {
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
  } else {
    gray = img.clone();
  }
  gray.convertTo(gray, CV_32F);

  std::vector<cv::Mat> featureMaps;

  // ====================================
  // Execute feature extraction pipeline
  // ====================================
  for(const auto &featurePipeline : features) {
    cv::Mat featureMat = gray.clone();
    for(const auto &step : featurePipeline.pipelineSteps) {
      step(featureMat);
    }
    featureMaps.push_back(featureMat);
  }

  // ---- Convert feature maps to feature matrix (pixels × features) ----

  // Stack into feature matrix
  cv::Mat featureMatrix(img.rows * img.cols, static_cast<int>(featureMaps.size()), CV_32F);
  for(int i = 0; i < static_cast<int>(featureMaps.size()); i++) {
    cv::Mat f = featureMaps[i].reshape(1, img.rows * img.cols);
    f.copyTo(featureMatrix.col(i));
  }

  // ---- Optional normalization for MLP ----
  if(normalizeForMLP) {
    cv::Mat featureMatrixNorm;
    cv::normalize(featureMatrix, featureMatrixNorm, 0, 1, cv::NORM_MINMAX);
    return featureMatrixNorm;
  }

  return featureMatrix;    // each row = pixel, each col = feature
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void MachineLearning::fireTrainingProgress(const std::string &progress)
{
  for(const auto &func : mProgressCallbacks) {
    func(progress);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void MachineLearning::registerProgressCallback(const std::function<void(const std::string &)> &func)
{
  mProgressCallbacks.emplace_back(func);
}
///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void MachineLearning::registerProgressCallback(const std::vector<std::function<void(const std::string &)>> &func)
{
  mProgressCallbacks = func;
}

}    // namespace joda::ml
