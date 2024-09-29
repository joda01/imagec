///
/// \file      ai_object_segmentation.h
/// \author    Joachim Danmayr
/// \date      2023-02-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Object segmentaiotn
///

#pragma once

#include <iostream>
#include "backend/commands/command.hpp"
#include "backend/helper/onnx_parser/onnx_parser.hpp"
#include <opencv2/opencv.hpp>
#include "ai_classifier_settings.hpp"

#define YOLO_P6 false

namespace joda::cmd {

///
/// \class      ObjectSegmentation
/// \author     Joachim Danmayr
/// \brief      Object segmentation using ONNX model
///
class AiClassifier : public Command
{
public:
  /////////////////////////////////////////////////////
  AiClassifier(const settings::AiClassifierSettings &);
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override;

private:
  /////////////////////////////////////////////////////

  auto getMask(const cv::Mat &maskChannel, const cv::Size &inputImageShape, const cv::Rect &box) -> cv::Mat;

/////////////////////////////////////////////////////
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

  static constexpr inline float NET_STRIDE[4]           = {8, 16, 32, 64};
  static constexpr inline float BOX_THRESHOLD           = 0.5;     // (default = 0.25)
  static constexpr inline float CLASS_THRESHOLD_DEFAULT = 0.5;     // (default = 0.5)
  static constexpr inline float NMS_THRESHOLD           = 0.45;    // To prevent double bounding boxes (default = 0.45)
  static constexpr inline float MASK_THRESHOLD          = 0.8;     // (default = 0.5)
  static constexpr inline float NMS_SCORE_THRESHOLD     = BOX_THRESHOLD * CLASS_THRESHOLD_DEFAULT;

  // float mClassThreshold;
  // float mNmsScoreThreshold;

  // Colors
  int32_t mNumberOfClasses;
  cv::dnn::Net mNet;

  const settings::AiClassifierSettings &mSettings;
};
}    // namespace joda::cmd
