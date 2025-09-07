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

namespace joda::ml {

class PixelClassifier
{
public:
  virtual void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) = 0;
  virtual void train(const cv::Mat &image, const std::set<joda::enums::ClassId> &classesToTrain, const atom::ObjectMap &regionOfInterest,
                     const std::filesystem::path &trainedModelOutputFile)                            = 0;
};

}    // namespace joda::ml
