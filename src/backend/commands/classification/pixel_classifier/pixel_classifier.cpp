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
#include <stdexcept>
#include "backend/commands/classification/pixel_classifier/pixel_classifier_training_settings.hpp"
#include "backend/commands/classification/pixel_classifier/random_forest/random_forest.hpp"

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
void PixelClassifier::execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result)
{
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PixelClassifier::train(const cv::Mat &image, const atom::ObjectMap &result, const settings::PixelClassifierTrainingSettings &trainingSettings)
{
  if(!result.contains(trainingSettings.trainingClass)) {
    throw std::invalid_argument("No objects with selected class exist!");
  }
  switch(trainingSettings.method) {
    case settings::PixelClassifierMethod::UNKNOWN:
      break;
    case settings::PixelClassifierMethod::RANDOM_FOREST: {
      RandomForest randForrest;
      randForrest.train(image, result.at(trainingSettings.trainingClass), trainingSettings.outPath);
    } break;
    case settings::PixelClassifierMethod::K_NEAREST:
      break;
  }
}

}    // namespace joda::cmd
