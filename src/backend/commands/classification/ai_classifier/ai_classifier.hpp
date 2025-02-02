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
  void loadONNXModel(const std::string &modelPath);
  void loadTORCHModel(const std::string &modelPath);

  void executeONNX(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result);
  void executeTORCH(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result);
  auto getMask(const cv::Mat &maskChannel, const cv::Size &inputImageShape, const cv::Rect &box) -> cv::Mat;

  // Colors
  bool mIsReady = false;

  const settings::AiClassifierSettings &mSettings;
};
}    // namespace joda::cmd
