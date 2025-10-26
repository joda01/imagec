///
/// \file      pixel_classifier.cpp
/// \author    Joachim Danmayr
/// \date      2025-08-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "pixel_classifier.hpp"
#include <chrono>
#include <stdexcept>
#include "backend/commands/classification/pixel_classifier/machine_learning/ann_mlp/ann_mlp_cv.hpp"
#include "backend/commands/classification/pixel_classifier/machine_learning/k_nearest/k_nearest_cv.hpp"
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"
#include "backend/commands/classification/pixel_classifier/machine_learning/random_forest/random_forest_cv.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/helper.hpp"
#include <opencv2/ml.hpp>

namespace joda::cmd {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
PixelClassifier::PixelClassifier(const settings::PixelClassifierSettings &settings) : mSettings(settings)
{
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PixelClassifier::execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList & /*result*/)
{
  // Load trained model
  ml::ModelType modelType      = ml::ModelType::RTrees;
  const auto absoluteModelPath = std::filesystem::weakly_canonical(context.getWorkingDirectory() / mSettings.modelPath);

  ml::MachineLearning *mlModel;

  switch(modelType) {
    case ml::ModelType::RTrees:
      mlModel = new ml::RandomForestCv(ml::RandomForestTrainingSettings{});
      break;
    case ml::ModelType::ANN_MLP:
      mlModel = new ml::AnnMlpCv(ml::AnnMlpTrainingSettings{});
      break;
    case ml::ModelType::KNearest:
      mlModel = new ml::KNearestCv(ml::KNearestTrainingSettings{});
      break;
    default:
      throw std::invalid_argument("Not supported model");
  }

  mlModel->forward(absoluteModelPath, image);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PixelClassifier::train(const cv::Mat &image, const enums::TileInfo &tileInfo, const atom::ObjectList &result,
                            const ml::MachineLearningSettings &trainingSettings, const TrainingsModelSettings &modelSettings)
{
  if(trainingSettings.features.empty()) {
    throw std::invalid_argument("At least one feature must be selected!");
  }

  cv::Mat trainSamples;
  cv::Mat labelList;
  ml::MachineLearning *mlModel;

  switch(trainingSettings.modelType) {
    case ml::ModelType::RTrees:
      mlModel = new ml::RandomForestCv(modelSettings.randomForest);
      break;
    case ml::ModelType::ANN_MLP:
      mlModel = new ml::AnnMlpCv(modelSettings.annMlp);
      break;
    case ml::ModelType::KNearest:
      mlModel = new ml::KNearestCv(modelSettings.kNearest);
      break;
    default:
      throw std::invalid_argument("Not supported model");
  }

  mlModel->train(trainingSettings, image, tileInfo, result);
  delete mlModel;
}

}    // namespace joda::cmd
