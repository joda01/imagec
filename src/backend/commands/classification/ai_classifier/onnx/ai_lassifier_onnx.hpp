///
/// \file      ai_lassifier_onnx.hpp
/// \author    Joachim Danmayr
/// \date      2025-01-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <vector>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/commands/classification/ai_classifier/ai_classifier_settings.hpp"
#include "backend/commands/classification/ai_classifier/ai_segmentation.hpp"

namespace joda::ai {

#define YOLO_P6 false

class AiClassifierOnnx : public AiSegmentation
{
  /////////////////////////////////////////////////////
  AiClassifierOnnx(const settings::AiClassifierSettings &settings);
  auto execute(const cv::Mat &originalImage) -> std::vector<ai::AiSegmentation::Result> override;

private:
  /////////////////////////////////////////////////////
  auto getMask(const cv::Mat &maskChannel, const cv::Size &inputImageShape, const cv::Rect &box) -> cv::Mat;

#if(defined YOLO_P6 && YOLO_P6 == true)

  //
  // Anchors defined in: https://github.com/ultralytics/yolov5/blob/master/models/hub/yolov5l6.yaml
  static constexpr inline float NET_ANCHORS[4][6] = {{19, 27, 44, 40, 38, 94},
                                                     {96, 68, 86, 152, 180, 137},
                                                     {140, 301, 303, 264, 238, 542},
                                                     { 436,
                                                       615,
                                                       739,
                                                       380,
                                                       925,
                                                       792 }};

  static constexpr inline int NET_WIDTH    = 1280;
  static constexpr inline int NET_HEIGHT   = 1280;
  static constexpr inline int SEG_WIDTH    = 320;
  static constexpr inline int SEG_HEIGHT   = 320;
  static constexpr inline int SEG_CHANNELS = 32;
  static constexpr inline int STRIDE_SIZE  = 4;
#else

  //
  // Anchors defined in: https://github.com/ultralytics/yolov5/blob/master/models/segment/yolov5l-seg.yaml
  static constexpr inline float NET_ANCHORS[3][6] = {{10, 13, 16, 30, 33, 23}, {30, 61, 62, 45, 59, 119}, {116, 90, 156, 198, 373, 326}};

  static constexpr inline int NET_WIDTH    = 640;
  static constexpr inline int NET_HEIGHT   = 640;
  static constexpr inline int SEG_WIDTH    = 160;
  static constexpr inline int SEG_HEIGHT   = 160;
  static constexpr inline int SEG_CHANNELS = 32;
  static constexpr inline int STRIDE_SIZE  = 3;
#endif    // YOLO_P6

  static constexpr inline float NET_STRIDE[4] = {8, 16, 32, 64};
  static constexpr inline float BOX_THRESHOLD = 0.25;    // (default = 0.25)
  // static constexpr inline float CLASS_THRESHOLD_DEFAULT = 0.5;     // (default = 0.5)
  static constexpr inline float NMS_THRESHOLD = 0.45;    // To prevent double bounding boxes (default = 0.45)
                                                         // static constexpr inline float MASK_THRESHOLD = 0.8;     // (default = 0.5)
  //  static constexpr inline float NMS_SCORE_THRESHOLD     = BOX_THRESHOLD * CLASS_THRESHOLD_DEFAULT;

  const float mClassThreshold;
  const float mNmsScoreThreshold;
  const float mMaskThreshold;
  const settings::AiClassifierSettings &mSettings;
  std::map<int32_t, float> mProbabilityHandicap;
};
}    // namespace joda::ai
