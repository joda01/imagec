/*
///
/// \brief      Post process the prediction.
///             YOLO produces an ouput array with following format
///             +-+-+-+-+----------+-------------+-------------+-------+--------------+---------------
///             |x|y|w|h|confidence|class score 1|class score 2|.....  |class
///             score n | masking ...
///             +-+-+-+-+----------+-------------+-------------+-------+--------------+---------------
///             The first two places are normalized center coordinates of the
///             detected bounding box. Then comes the normalized width and
///             height. Index 4 has the confidence score that tells the
///             probability of the detection being an object. The following
///             entries tell the class scores.
///
/// \author     Joachim Danmayr
/// \ref
/// https://learnopencv.com/object-detection-using-yolov5-and-opencv-dnn-in-c-and-python/
///
/// \param[in]  inputImage Image which has been used for detection
/// \return     Result of the analysis
///
/*

// Function to detect model type
string detectModelType(Ort::Session &session)
{
  auto outputShape = session.GetOutputTypeInfo(1).GetTensorTypeAndShapeInfo().GetShape();

  if(outputShape.size() == 3) {
    return "semantic";    // (C, H, W) - Semantic Segmentation
  }
  if(outputShape.size() == 4) {
    return "instance";    // (N, C, H, W) - Instance Segmentation
  }
  return "unknown";
}
*/

///
/// \file      ai_model_bioimage.hpp
/// \author    Joachim Danmayr
/// \date      2025-02-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include "backend/commands/classification/ai_classifier/models/ai_model.hpp"

namespace joda::ai {

class AiModelYolo : public AiModel
{
public:
  AiModelYolo() = default;
  auto processPrediction(const cv::Mat &inputImage, const at::IValue &prediction) -> std::vector<Result> override;

private:
  /////////////////////////////////////////////////////
};

}    // namespace joda::ai
