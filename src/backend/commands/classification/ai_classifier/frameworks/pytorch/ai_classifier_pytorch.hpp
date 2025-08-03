///
/// \file      ai_classifier_pytorch.hpp
/// \author    Joachim Danmayr
/// \date      2025-01-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include <mutex>
#if defined(WITH_PYTORCH)

#include <string>
#include <vector>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/commands/classification/ai_classifier/frameworks/ai_framework.hpp"

namespace joda::ai {

///
/// \class      Pytorch framework
/// \author     Joachim Danmayr
/// \brief      Prediction using pytorch
///
class AiFrameworkPytorch : public AiFramework
{
public:
  /////////////////////////////////////////////////////
  AiFrameworkPytorch(const std::string &modelPath, const InputParameters &inputParameters);
  auto predict(const at::Device &device, const cv::Mat &originalImage) -> at::IValue override;

private:
  /////////////////////////////////////////////////////

  const InputParameters &mSettings;
  const std::string mModelPath;
  static inline std::mutex mExecutionMutex;
};

}    // namespace joda::ai

#endif
