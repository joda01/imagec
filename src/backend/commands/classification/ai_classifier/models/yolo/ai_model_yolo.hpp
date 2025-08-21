

///
/// \file      ai_model_unet.hpp
/// \author    Joachim Danmayr
/// \date      2025-02-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include "backend/commands/classification/ai_classifier/models/ai_model.hpp"

namespace joda::ai {

class AiModelYolo : public AiModel
{
public:
  struct ProbabilitySettings
  {
    float maskThreshold  = 0.8F;
    float classThreshold = 0.5F;
  };

  AiModelYolo(const ProbabilitySettings &settings);
  auto processPrediction(const at::Device &device, const cv::Mat &inputImage, const at::IValue &prediction) -> std::vector<Result> override;

private:
  /////////////////////////////////////////////////////
  static constexpr inline int SEG_WIDTH        = 160;
  static constexpr inline int SEG_HEIGHT       = 160;
  static constexpr inline int NET_WIDTH        = 640;
  static constexpr inline int NET_HEIGHT       = 640;
  static constexpr inline float BOX_THRESHOLD  = 0.25;    // (default = 0.25)
  static constexpr inline float NET_STRIDE[4]  = {8, 16, 32, 64};
  static constexpr inline int64_t SEG_CHANNELS = 32;
  static constexpr inline int STRIDE_SIZE      = 3;
  static constexpr inline float NMS_THRESHOLD  = 0.45F;    // To prevent double bounding boxes (default = 0.45)

  const float mClassThreshold = 0.5;
  const float mMaskThreshold  = 0.8F;
  const float mNmsScoreThreshold;

  /////////////////////////////////////////////////////
  [[nodiscard]] auto getMask(const cv::Mat &maskChannel, const cv::Size &inputImageShape, const cv::Rect &box) const -> cv::Mat;
};

}    // namespace joda::ai
