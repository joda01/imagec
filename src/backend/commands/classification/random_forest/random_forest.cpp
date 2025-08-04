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
#include <opencv2/ml.hpp>

namespace joda::cmd {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
RandomForest::RandomForest(const settings::RandomForestSettings &settings) : mSettings(settings)
{
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RandomForest::execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result)
{
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RandomForest::prepareTrainingDataFromROI(const cv::Mat &image, const std::unique_ptr<atom::SpheralIndex> &objectsToLearn, cv::Mat &samples,
                                              cv::Mat &labels)
{
  std::vector<std::vector<float>> tempFeatures;    // temporary to count total size
  int featureLength = 0;

  // First pass: gather features to determine total count and dimensionality
  for(const auto &roi : *objectsToLearn) {
    const cv::Mat mask = roi.getMask();
    const auto bbox    = roi.getBoundingBoxTile();

    cv::Mat gradMag, gradAngle;
    roi.calcGradients(image, gradMag, gradAngle);
    auto roiIntensity = roi.calcIntensity(image);

    for(int y = 0; y < mask.rows; ++y) {
      for(int x = 0; x < mask.cols; ++x) {
        if(mask.at<uchar>(y, x) == 0) {
          continue;
        }

        int img_x = bbox.x + x;
        int img_y = bbox.y + y;

        float intensity = static_cast<float>(image.at<uchar>(img_y, img_x));
        float gmag      = gradMag.at<float>(y, x);
        float gangle    = gradAngle.at<float>(y, x);

        std::vector<float> featureVec = {
            intensity,
            static_cast<float>(img_x),
            static_cast<float>(img_y),
            static_cast<float>(roi.getCircularity()),
            static_cast<float>(roi.getAreaSize()),
            static_cast<float>(roiIntensity.intensitySum),
            static_cast<float>(roiIntensity.intensityMax),
            static_cast<float>(roiIntensity.intensityMin),
            static_cast<float>(roiIntensity.intensityAvg),
            gmag,
            gangle,
        };

        tempFeatures.push_back(featureVec);
      }
    }
  }

  if(tempFeatures.empty())
    return;

  featureLength  = static_cast<int>(tempFeatures[0].size());
  int numSamples = static_cast<int>(tempFeatures.size());

  // Allocate final matrices
  samples = cv::Mat(numSamples, featureLength, CV_32F);
  labels  = cv::Mat(numSamples, 1, CV_32S);    // assuming classification

  for(int i = 0; i < numSamples; ++i) {
    for(int j = 0; j < featureLength; ++j) {
      samples.at<float>(i, j) = tempFeatures[i][j];
    }
    labels.at<int>(i, 0) = 0;    // Replace with actual label logic if needed
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Ptr<cv::ml::RTrees> RandomForest::trainRandomForest(std::vector<std::vector<float>> &featList, std::vector<int> &labelList)
{
  cv::Ptr<cv::ml::RTrees> rf = cv::ml::RTrees::create();
  rf->setMaxDepth(10);
  rf->setMinSampleCount(10);
  rf->setRegressionAccuracy(0);
  rf->setUseSurrogates(false);
  rf->setMaxCategories(2);
  rf->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 0.01));
  cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create(featList, cv::ml::ROW_SAMPLE, labelList);
  rf->train(td);
  return rf;
}

}    // namespace joda::cmd
