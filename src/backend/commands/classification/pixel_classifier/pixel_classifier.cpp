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
#include "backend/commands/classification/pixel_classifier/machine_learning/ann_mlp/ann_mlp_pytorch.hpp"
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"
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
std::tuple<ml::ModelType, ml::Framework> PixelClassifier::fileEndianToModelType(const std::filesystem::path &absoluteModelPath)
{
  if(absoluteModelPath.filename().string().ends_with(joda::fs::MASCHINE_LEARNING_PYTORCH_ANN_MLP)) {
    return {ml::ModelType::ANN_MLP, ml::Framework::PyTorch};
  } else if(absoluteModelPath.string().ends_with(joda::fs::MASCHINE_LEARNING_MLPACK_RTREE)) {
    return {ml::ModelType::RTrees, ml::Framework::MlPack};
  }
  return {ml::ModelType::Unknown, ml::Framework::Unknown};
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

  auto [type, framework]       = fileEndianToModelType(absoluteModelPath);
  ml::MachineLearning *mlModel = nullptr;
  switch(type) {
    case ml::ModelType::RTrees:
      if(framework == ml::Framework::MlPack) {
        mlModel = new ml::RandomForestMlPack(ml::RandomForestTrainingSettings{}, &mSettings);
      }
      break;
    case ml::ModelType::ANN_MLP:
      if(framework == ml::Framework::PyTorch) {
        mlModel = new ml::AnnMlpPyTorch(ml::AnnMlpTrainingSettings{}, &mSettings);
      }
      break;
    case ml::ModelType::KNearest:
      break;
    default:
      throw std::invalid_argument("Not supported model >" + absoluteModelPath.string() + "<");
  }
  if(nullptr == mlModel) {
    throw std::invalid_argument("Not supported model >" + absoluteModelPath.string() + "<");
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
  if(trainingSettings.featureExtractionPipelines.empty()) {
    throw std::invalid_argument("At least one feature must be selected!");
  }

  cv::Mat trainSamples;
  cv::Mat labelList;
  mTrainingModel.reset();

  switch(trainingSettings.modelTyp) {
    case ml::ModelType::RTrees:
      if(trainingSettings.framework == ml::Framework::MlPack) {
        mTrainingModel = std::make_unique<ml::RandomForestMlPack>(modelSettings.randomForest, nullptr);
      }
      break;
    case ml::ModelType::ANN_MLP:
      if(trainingSettings.framework == ml::Framework::PyTorch) {
        mTrainingModel = std::make_unique<ml::AnnMlpPyTorch>(modelSettings.annMlp, nullptr);
      }
      break;
    case ml::ModelType::KNearest:
      break;
    default:
      throw std::invalid_argument("Not supported model");
  }
  if(nullptr == mTrainingModel) {
    throw std::invalid_argument("Not supported model");
  }
  mTrainingModel->registerProgressCallback(mProgressCallbacks);
  mTrainingModel->train(trainingSettings, image, tileInfo, result);
  mTrainingModel.reset();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PixelClassifier::stopTraining()
{
  if(nullptr != mTrainingModel) {
    mTrainingModel->stopTraining();
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PixelClassifier::registerProgressCallback(const std::function<void(const std::string &)> &func)
{
  mProgressCallbacks.emplace_back(func);
}

}    // namespace joda::cmd
