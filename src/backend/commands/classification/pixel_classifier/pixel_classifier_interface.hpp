///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///

#pragma once

#include <filesystem>
#include "backend/processor/context/process_context.hpp"
#include "pixel_classifier_training_settings.hpp"

namespace joda::ml {

class PixelClassifier
{
public:
  virtual void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result)                = 0;
  virtual void train(const settings::PixelClassifierTrainingSettings &trainingSettings, const cv::Mat &image, const std::set<int32_t> &classesToTrain,
                     const atom::ObjectList &regionOfInterest, const std::filesystem::path &trainedModelOutputFile) = 0;
};

}    // namespace joda::ml
