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

#include "ann_mlp_cv.hpp"
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
void AnnMlpCv::predict(const std::filesystem::path &path, const cv::Mat &image, const cv::Mat &features, cv::Mat &prediction)
{
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
void AnnMlpCv::train(const cv::Mat &trainSamples, const cv::Mat &trainLabels, int32_t nrOfClasses)
{
  cv::Mat labelsOneHot = cv::Mat::zeros(trainLabels.rows, nrOfClasses, CV_32F);
  for(int32_t i = 0; i < trainLabels.rows; i++) {
    int32_t cls                    = trainLabels.at<int>(i, 0);
    labelsOneHot.at<float>(i, cls) = 1.0F;
  }
  mModel = cv::ml::ANN_MLP::create();

  // Collect layer sizes: [input, hidden..., output]
  std::vector<int32_t> layers;
  layers.push_back(trainSamples.cols);    // Input layer
  for(int neurons : mSettings.neuronsLayer) {
    if(neurons > 0) {
      layers.push_back(neurons);    // Add hidden layer if > 0
    }
  }
  layers.push_back(nrOfClasses);    // Output layer

  cv::Mat layerSizes(1, static_cast<int>(layers.size()), CV_32S, layers.data());
  layerSizes = layerSizes.clone();    // clone, because .data() would go out of scope
  mModel->setLayerSizes(layerSizes);
  mModel->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 1, 1);
  mModel->setTrainMethod(cv::ml::ANN_MLP::BACKPROP, 0.001);    // learning rate
  mModel->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, mSettings.maxIterations, mSettings.terminationEpsilon));
  cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create(trainSamples, cv::ml::ROW_SAMPLE, labelsOneHot);
  mModel->train(td);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void AnnMlpCv::storeModel(const std::filesystem::path &path, const MachineLearningSettings &settings)
{
  cv::FileStorage fs(path.string(), cv::FileStorage::WRITE | cv::FileStorage::FORMAT_JSON);
  fs << "model"
     << "{";
  mModel->write(fs);
  fs << "}";
  fs.endWriteStruct();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void AnnMlpCv::loadModel(const std::filesystem::path &path)
{
  cv::FileStorage fs(path.string(), cv::FileStorage::READ | cv::FileStorage::FORMAT_JSON);
  mModel             = cv::ml::ANN_MLP::create();
  cv::FileNode model = fs["model"];
  mModel->read(model);
}

}    // namespace joda::ml
