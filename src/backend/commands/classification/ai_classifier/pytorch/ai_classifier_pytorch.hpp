///
/// \file      ai_classifier_pytorch.hpp
/// \author    Joachim Danmayr
/// \date      2025-01-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#if defined(WITH_TENSORFLOW)

#include <vector>
#include "../ai_segmentation.hpp"
#include "backend/artifacts/roi/roi.hpp"
#include "backend/commands/classification/ai_classifier/ai_classifier_settings.hpp"

namespace at {
class Tensor;
}

namespace joda::ai {

///
/// \class      ObjectSegmentation
/// \author     Joachim Danmayr
/// \brief      Object segmentation using pytorch
///
class AiClassifierPyTorch : public AiSegmentation
{
public:
  /////////////////////////////////////////////////////
  AiClassifierPyTorch(const settings::AiClassifierSettings &settings);
  auto execute(const cv::Mat &originalImage) -> std::vector<Result> override;

private:
  auto tensorToObjectMasks(const at::Tensor &tensor, const at::Tensor &class_probabilities) -> std::vector<Result>;
  const settings::AiClassifierSettings &mSettings;
};

}    // namespace joda::ai

#endif
