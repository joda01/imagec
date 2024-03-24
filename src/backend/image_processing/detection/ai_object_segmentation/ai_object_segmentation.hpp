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
#include "../detection.hpp"
#include "../detection_response.hpp"
#include <opencv2/opencv.hpp>

#define YOLO_P6 false

namespace joda::func::ai {

///
/// \class      ObjectSegmentation
/// \author     Joachim Danmayr
/// \brief      Object segmentation using ONNX model
///
class ObjectSegmentation : public DetectionFunction
{
public:
  /////////////////////////////////////////////////////
  ObjectSegmentation(const joda::settings::json::ChannelFiltering *filt, const std::string &onnxNetPath,
                     const std::vector<std::string> &classNames);
  auto forward(const cv::Mat &srcImg, const cv::Mat &originalImage, int32_t channelIndex) -> DetectionResponse override;

private:
  /////////////////////////////////////////////////////

  auto getMask(const cv::Mat &maskChannel, const cv::Vec4d &params, const cv::Size &inputImageShape,
               const cv::Rect &box) -> cv::Mat;

  void letterBox(const cv::Mat &image, cv::Mat &outImage,
                 cv::Vec4d &params,    //[ratio_x,ratio_y,dw,dh]
                 const cv::Size &newShape = cv::Size(NET_WIDTH, NET_HEIGHT), bool autoShape = false,
                 bool scaleFill = false, bool scaleUp = true, int stride = NET_STRIDE[2],
                 const cv::Scalar &color = cv::Scalar(114, 114, 114));

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
  static constexpr inline float NET_ANCHORS[3][6] = {{10, 13, 16, 30, 33, 23},
                                                     {30, 61, 62, 45, 59, 119},
                                                     {116, 90, 156, 198, 373, 326}};

  static constexpr inline int NET_WIDTH    = 640;
  static constexpr inline int NET_HEIGHT   = 640;
  static constexpr inline int SEG_WIDTH    = 160;
  static constexpr inline int SEG_HEIGHT   = 160;
  static constexpr inline int SEG_CHANNELS = 32;
  static constexpr inline int STRIDE_SIZE  = 3;
#endif    // YOLO_P6

  static constexpr inline float NET_STRIDE[4]       = {8, 16, 32, 64};
  static constexpr inline float BOX_THRESHOLD       = 0.4;    // (default = 0.25)
  static constexpr inline float CLASS_THRESHOLD     = 0.5;    // (default = 0.5)
  static constexpr inline float NMS_THRESHOLD       = 0.3;    // To prevent double bounding boxes (default = 0.45)
  static constexpr inline float MASK_THRESHOLD      = 0.5;    // (default = 0.5)
  static constexpr inline float NMS_SCORE_THRESHOLD = BOX_THRESHOLD * CLASS_THRESHOLD;

  // Colors
  std::vector<std::string> mClassNames;
  cv::dnn::Net mNet;
};
}    // namespace joda::func::ai
