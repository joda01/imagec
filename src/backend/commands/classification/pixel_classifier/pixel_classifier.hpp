///
/// \file      classifier.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "backend/commands/classification/pixel_classifier/machine_learning/ann_mlp/ann_mlp_settings.hpp"
#include "backend/commands/classification/pixel_classifier/machine_learning/k_nearest/k_nearest_settings.hpp"
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"
#include "backend/commands/classification/pixel_classifier/machine_learning/random_forest/random_forest_settings.hpp"
#include "backend/commands/command.hpp"
#include "backend/enums/types.hpp"
#include "backend/processor/context/process_context.hpp"
#include "pixel_classifier_settings.hpp"

template <class T>
concept CvModel_t =
    std::is_base_of<cv::ml::RTrees, T>::value || std::is_base_of<cv::ml::NormalBayesClassifier, T>::value ||
    std::is_base_of<cv::ml::KNearest, T>::value || std::is_base_of<cv::ml::SVM, T>::value || std::is_base_of<cv::ml::DTrees, T>::value ||
    std::is_base_of<cv::ml::Boost, T>::value || std::is_base_of<cv::ml::ANN_MLP, T>::value || std::is_base_of<cv::ml::LogisticRegression, T>::value ||
    std::is_base_of<cv::ml::SVMSGD, T>::value || std::is_base_of<cv::ml::EM, T>::value;

namespace joda::cmd {

struct TrainingsModelSettings
{
  ml::RandomForestTrainingSettings randomForest;
  ml::AnnMlpTrainingSettings annMlp;
  ml::KNearestTrainingSettings kNearest;
};

///
/// \class      Classifier
/// \author     Joachim Danmayr
/// \brief      Classify objects in a grayscal image.
///             Each gray value is used for one object class.
///
class PixelClassifier : public Command
{
public:
  /////////////////////////////////////////////////////
  PixelClassifier(const settings::PixelClassifierSettings &);
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override;
  static void train(const cv::Mat &image, const enums::TileInfo &tileInfo, const atom::ObjectList &result,
                    const ml::MachineLearningSettings &trainingSettings, const TrainingsModelSettings &modelSettings);

private:
  /////////////////////////////////////////////////////
  const settings::PixelClassifierSettings &mSettings;
};

}    // namespace joda::cmd
