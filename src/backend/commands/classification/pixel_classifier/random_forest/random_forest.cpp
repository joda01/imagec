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
#include "backend/enums/enums_units.hpp"
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
RandomForest::RandomForest()
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
  cv::Ptr<cv::ml::RTrees> model =
      cv::ml::StatModel::load<cv::ml::RTrees>("/workspaces/imagec/src/backend/commands/classification/random_forest/myTree.xml");

  // ===============================
  // Predict
  // ===============================

  cv::Mat features = extractFeatures(image);

  cv::Mat predFloat;    // (H*W) x 1, CV_32F
  model->predict(features, predFloat);
  cv::Mat segMask = predFloat.reshape(1, image.rows);    // H x W, CV_32F
  segMask.convertTo(segMask, CV_16UC1);

  image        = cv::Mat::zeros(segMask.size(), CV_16U);
  cv::Mat inv  = 65536 - segMask;
  cv::Mat mask = (segMask > 0);
  image.setTo(0, ~mask);
  inv.copyTo(image, mask);

  // Now segMask is a binary mask (0 = background, 1 = object)
  cv::imwrite("tmp/segmentation_02.png", image);    // multiply for visualization
}

// TRAINING //////////////////////////////////

void RandomForest::train(const cv::Mat &image, const std::set<joda::enums::ClassId> &classesToTrain, const atom::ObjectMap &regionOfInterest,
                         const std::filesystem::path &trainedModelOutputFile)
{
  cv::Mat trainSamples;
  cv::Mat labelList;
  prepareTrainingDataFromROI(image, classesToTrain, regionOfInterest, trainSamples, labelList);
  auto mlTree = trainRandomForest(trainSamples, labelList);
  mlTree->save(trainedModelOutputFile);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Mat RandomForest::extractFeatures(const cv::Mat &img)
{
  CV_Assert(img.channels() == 1);
  std::vector<cv::Mat> featureMaps;

  // Raw intensity
  featureMaps.push_back(img.clone());

  // Gaussian blur at different scales
  cv::Mat blur1;
  cv::Mat blur2;
  cv::GaussianBlur(img, blur1, cv::Size(5, 5), 1.0);
  cv::GaussianBlur(img, blur2, cv::Size(9, 9), 2.0);
  featureMaps.push_back(blur1);
  featureMaps.push_back(blur2);

  // Gradient magnitude
  cv::Mat gradX;
  cv::Mat gradY;
  cv::Mat gradMag;
  cv::Sobel(img, gradX, CV_32F, 1, 0);
  cv::Sobel(img, gradY, CV_32F, 0, 1);
  cv::magnitude(gradX, gradY, gradMag);
  featureMaps.push_back(gradMag);

  // Laplacian of Gaussian
  cv::Mat lap;
  cv::Laplacian(img, lap, CV_32F);
  featureMaps.push_back(lap);

  // Stack into feature matrix
  cv::Mat features(img.rows * img.cols, static_cast<int>(featureMaps.size()), CV_32F);
  for(int i = 0; i < static_cast<int>(featureMaps.size()); i++) {
    cv::Mat f = featureMaps[i].reshape(1, img.rows * img.cols);
    f.copyTo(features.col(i));
  }

  return features;    // Each row = pixel, each col = feature
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RandomForest::prepareTrainingDataFromROI(const cv::Mat &image, const std::set<joda::enums::ClassId> &classesToTrain,
                                              const atom::ObjectMap &regionOfInterest, cv::Mat &trainSamples, cv::Mat &trainLabels)
{
  // Extract features
  cv::Mat features = extractFeatures(image);

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
  cv::Mat overlayOfAllRois =
      cv::Mat::zeros(image.size(), CV_16UC1);    // At the end of the loop this cv::Mat contains all trained areas as white pixels
  int32_t classLabelIndex = 1;                   // We start at 1, because 0 is reserver for the background
  for(const auto classIdToTrain : classesToTrain) {
    if(!regionOfInterest.contains(classIdToTrain)) {
      continue;
    }
    cv::Mat roiMask            = cv::Mat::zeros(image.size(), CV_16UC1);
    const auto &objectsToLearn = regionOfInterest.at(classIdToTrain);
    objectsToLearn->createBinaryImage(roiMask);
    cv::bitwise_or(roiMask, overlayOfAllRois, overlayOfAllRois);
    extractSamples(roiMask, classLabelIndex);
    classLabelIndex++;
  }

  // ====================================
  // Train the background
  // ====================================
  cv::bitwise_not(overlayOfAllRois, overlayOfAllRois);
  extractSamples(overlayOfAllRois, 0);    // Background is always 0

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
cv::Ptr<cv::ml::RTrees> RandomForest::trainRandomForest(const cv::Mat &trainSamples, const cv::Mat &trainLabels)
{
  cv::Ptr<cv::ml::RTrees> rf = cv::ml::RTrees::create();

  rf->setMaxDepth(15);
  rf->setMinSampleCount(2);
  rf->setRegressionAccuracy(0);
  rf->setUseSurrogates(false);
  rf->setMaxCategories(2);    // adjust for multiclass
  rf->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 50, 0.01));
  cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create(trainSamples, cv::ml::ROW_SAMPLE, trainLabels);
  rf->train(td);
  return rf;
}

}    // namespace joda::cmd
