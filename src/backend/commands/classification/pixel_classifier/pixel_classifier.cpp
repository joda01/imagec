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
#include "backend/commands/classification/pixel_classifier/machine_learning/ann_mlp/ann_mlp_mlpack.hpp"
#include "backend/commands/classification/pixel_classifier/machine_learning/k_nearest/k_nearest_cv.hpp"
#include "backend/commands/classification/pixel_classifier/machine_learning/k_nearest/k_nearest_mlpack.hpp"
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"
#include "backend/commands/classification/pixel_classifier/machine_learning/random_forest/random_forest_cv.hpp"
#include "backend/commands/classification/pixel_classifier/machine_learning/random_forest/random_forest_mlpack.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/helper.hpp"
#include <nlohmann/json_fwd.hpp>
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
  const auto absoluteModelPath = std::filesystem::weakly_canonical(context.getWorkingDirectory() / mSettings.modelPath);

  // Open the JSON file
  std::ifstream file(absoluteModelPath.string());
  if(!file.is_open()) {
    return;
  }

  nlohmann::json json;
  file >> json;
  file.close();
  ml::MachineLearningSettings modelSettings = json;

  ml::MachineLearning *mlModel;
  switch(modelSettings.modelTyp) {
    case ml::ModelType::RTrees:
      if(modelSettings.framework == ml::Framework::OpenCv) {
        mlModel = new ml::RandomForestCv(ml::RandomForestTrainingSettings{});
      } else if(modelSettings.framework == ml::Framework::MlPack) {
        mlModel = new ml::RandomForestMlPack(ml::RandomForestTrainingSettings{});
      }
      break;
    case ml::ModelType::ANN_MLP:
      if(modelSettings.framework == ml::Framework::OpenCv) {
        mlModel = new ml::AnnMlpCv(ml::AnnMlpTrainingSettings{});
      } else if(modelSettings.framework == ml::Framework::MlPack) {
        mlModel = new ml::AnnMlpMlPack(ml::AnnMlpTrainingSettings{});
      }
      break;
    case ml::ModelType::KNearest:
      if(modelSettings.framework == ml::Framework::OpenCv) {
        mlModel = new ml::KNearestCv(ml::KNearestTrainingSettings{});
      }
      break;
    default:
      throw std::invalid_argument("Not supported model");
  }

  mlModel->forward(absoluteModelPath, image, modelSettings);
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
      if(trainingSettings.framework == ml::Framework::MlPack) {
        mlModel = new ml::RandomForestMlPack(modelSettings.randomForest);
      } else if(trainingSettings.framework == ml::Framework::OpenCv) {
        mlModel = new ml::RandomForestCv(modelSettings.randomForest);
      }
      break;
    case ml::ModelType::ANN_MLP:
      if(trainingSettings.framework == ml::Framework::MlPack) {
        mlModel = new ml::AnnMlpMlPack(modelSettings.annMlp);
      } else if(trainingSettings.framework == ml::Framework::OpenCv) {
        mlModel = new ml::AnnMlpCv(modelSettings.annMlp);
      }
      break;
    case ml::ModelType::KNearest:
      if(trainingSettings.framework == ml::Framework::MlPack) {
        mlModel = new ml::KNearestMlPack(modelSettings.kNearest);
      } else if(trainingSettings.framework == ml::Framework::OpenCv) {
        mlModel = new ml::KNearestCv(modelSettings.kNearest);
      }
      break;
    default:
      throw std::invalid_argument("Not supported model");
  }

  mlModel->train(trainingSettings, image, tileInfo, result);
  delete mlModel;
}

}    // namespace joda::cmd
