///
/// \file      random_forest_cv.cpp
/// \author    Joachim Danmayr
/// \date      2025-10-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "random_forest_cv.hpp"
#include <fstream>
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"

namespace joda::ml {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RandomForestCv::predict(const std::filesystem::path &path, const cv::Mat &image, const cv::Mat &features, cv::Mat &prediction,
                             const std::filesystem::path &modelStoragePath)
{
  cv::Ptr<cv::ml::RTrees> mModel;

  cv::FileStorage fs(path.string(), cv::FileStorage::READ | cv::FileStorage::FORMAT_JSON);
  mModel             = cv::ml::RTrees::create();
  cv::FileNode model = fs["model"];
  mModel->read(model);

  if(features.type() != CV_32F || !features.isContinuous()) {
    features.convertTo(features, CV_32F);
  }
  mModel->predict(features, prediction);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RandomForestCv::train(const cv::Mat &trainSamples, const cv::Mat &trainLabels, int32_t /*nrOfClasses*/,
                           const std::filesystem::path &modelStoragePath)
{
  cv::Ptr<cv::ml::RTrees> mModel;

  mModel = cv::ml::RTrees::create();
  mModel->setMaxDepth(mSettings.maxTreeDepth);
  mModel->setMinSampleCount(mSettings.minSampleCount);
  mModel->setRegressionAccuracy(mSettings.regressionAccuracy);
  mModel->setUseSurrogates(false);
  mModel->setMaxCategories(10);
  mModel->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, mSettings.maxNumberOfTrees, mSettings.terminationEpsilon));
  cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create(trainSamples, cv::ml::ROW_SAMPLE, trainLabels);
  mModel->train(td);

  {
    cv::FileStorage fs(modelStoragePath.string(), cv::FileStorage::WRITE | cv::FileStorage::FORMAT_JSON);
    fs << "model"
       << "{";
    mModel->write(fs);
    fs << "}";
  }
}

}    // namespace joda::ml
