///
/// \file      random_forest.cpp
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "random_forest.hpp"
#include <opencv2/core/hal/interface.h>
#include <string>
#include "backend/commands/classification/pixel_classifier/pixel_classifier_training_settings.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_units.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/ml.hpp>

namespace joda::cmd {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
RandomForest::RandomForest(const std::filesystem::path &modelPath) : mModelPath(modelPath)
{
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RandomForest::execute(processor::ProcessContext & /*context*/, cv::Mat &image, atom::ObjectList & /*result*/)
{
  // Load trained forest
  std::set<joda::settings::PixelClassifierFeatures> featuresSet;
  settings::PixelClassifierMethod method;
  auto model = loadModel(mModelPath, featuresSet, method);

  // ===============================
  // Predict
  // ===============================

  cv::Mat features = extractFeatures(image, featuresSet);

  cv::Mat predFloat;    // (H*W) x 1, CV_32F
  model->predict(features, predFloat);
  cv::Mat segMask = predFloat.reshape(1, image.rows);    // H x W, CV_32F
  segMask.convertTo(segMask, CV_16UC1);

  image        = cv::Mat::zeros(segMask.size(), CV_16U);
  cv::Mat mask = (segMask > 0);
  image.setTo(0, ~mask);
  segMask.copyTo(image, mask);

  // Now segMask is a binary mask (0 = background, 1 = object)
  cv::imwrite("tmp/segmentation_02.png", image);    // multiply for visualization
}

// TRAINING //////////////////////////////////

void RandomForest::train(const settings::PixelClassifierTrainingSettings &trainingSettings, const cv::Mat &image,
                         const std::set<int32_t> &classesToTrain, const atom::ObjectList &regionOfInterest,
                         const std::filesystem::path &trainedModelOutputFile)
{
  cv::Mat trainSamples;
  cv::Mat labelList;
  prepareTrainingDataFromROI(image, classesToTrain, regionOfInterest, trainSamples, labelList, trainingSettings.features);
  if(trainSamples.empty() || labelList.empty()) {
    joda::log::logWarning("No training samples!");
    return;
  }
  auto mlTree = trainRandomForest(trainingSettings.randomForest.value_or(joda::settings::RandomForestTrainingSettings{}), trainSamples, labelList);
  storeModel(mlTree, trainedModelOutputFile, trainingSettings.features);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RandomForest::prepareTrainingDataFromROI(const cv::Mat &image, const std::set<int32_t> &classesToTrain, const atom::ObjectList &regionOfInterest,
                                              cv::Mat &trainSamples, cv::Mat &trainLabels,
                                              const std::set<joda::settings::PixelClassifierFeatures> &featuresSet)
{
  // Extract features
  cv::Mat features = extractFeatures(image, featuresSet);

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
  for(const auto classIdToTrain : classesToTrain) {
    if(!regionOfInterest.contains(static_cast<enums::ClassId>(classIdToTrain))) {
      continue;
    }
    cv::Mat roiMask            = cv::Mat::zeros(image.size(), CV_16UC1);
    const auto &objectsToLearn = regionOfInterest.at(static_cast<enums::ClassId>(classIdToTrain));
    objectsToLearn->createBinaryImage(roiMask);
    extractSamples(roiMask, classIdToTrain);
    cv::imwrite("tmp/training_mask_" + std::to_string(classIdToTrain) + ".png", roiMask);      // multiply for visualization
    cv::imwrite("tmp/image_" + std::to_string(classIdToTrain) + ".png", image);                // multiply for visualization
    cv::imwrite("tmp/features_" + std::to_string(classIdToTrain) + ".png", features / 256);    // multiply for visualization
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
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Ptr<cv::ml::RTrees> RandomForest::trainRandomForest(const joda::settings::RandomForestTrainingSettings &settings, const cv::Mat &trainSamples,
                                                        const cv::Mat &trainLabels)
{
  cv::Ptr<cv::ml::RTrees> rf = cv::ml::RTrees::create();

  rf->setMaxDepth(settings.maxTreeDepth);
  rf->setMinSampleCount(settings.minSampleCount);
  rf->setRegressionAccuracy(settings.regressionAccuracy);
  rf->setUseSurrogates(false);
  rf->setMaxCategories(10);
  rf->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, settings.maxNumberOfTrees, settings.terminationEpsilon));
  cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create(trainSamples, cv::ml::ROW_SAMPLE, trainLabels);
  rf->train(td);
  return rf;
}

}    // namespace joda::cmd
