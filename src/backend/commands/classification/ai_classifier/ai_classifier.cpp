///
/// \file      ai_object_segmentation.cpp
/// \author    Joachim Danmayr
/// \date      2023-02-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

/// \link      https://github.com/UNeedCryDear/yolov5-seg-opencv-onnxruntime-cpp

#include <c10/core/ScalarType.h>
#include <c10/core/TensorOptions.h>
#include <torch/csrc/jit/serialization/import.h>
#include <torch/script.h>    // One-stop header.
#include <torch/types.h>

#include <exception>
#include <memory>
#include <string>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/enums/enum_objects.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core/matx.hpp>
#include <opencv2/core/persistence.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "ai_classifier.hpp"
#include "ai_segmentation.hpp"

namespace joda::cmd {

using namespace std;
using namespace cv;
using namespace cv::dnn;

///
/// \brief      Constructor
/// \author     Joachim Danmayr
/// \param[in]  onnxNetPath Path to the ONNX net file
/// \param[in]  classNames  Array of class names e.g. {"nuclues","cell"}
///
AiClassifier::AiClassifier(const settings::AiClassifierSettings &settings) : mSettings(settings), mNumberOfClasses(settings.numberOfModelClasses)
{
}

void AiClassifier::execute(processor::ProcessContext &context, cv::Mat &imageNotUse, atom::ObjectList &result)
{
  std::vector<joda::ai::AiSegmentation::Result> segResult;

  for(const auto &res : segResult) {
    //
    // Apply the filter based on the object class
    //
    if(mSettings.modelClasses.size() > res.classId) {
      auto objectClassToUse = *mSettings.modelClasses.begin();
      for(const auto &objectClass : mSettings.modelClasses) {
        if(objectClass.modelClassId == res.classId) {
          objectClassToUse = objectClass;
          break;
        }
      }
      joda::atom::ROI detectedRoi(
          atom::ROI::RoiObjectId{
              .classId    = context.getClassId(objectClassToUse.outputClassNoMatch),
              .imagePlane = context.getActIterator(),
          },
          context.getAppliedMinThreshold(), res.boundingBox, res.mask, res.contour, context.getImageSize(), context.getOriginalImageSize(),
          context.getActTile(), context.getTileSize());

      for(const auto &filter : objectClassToUse.filters) {
        if(joda::settings::ClassifierFilter::doesFilterMatch(context, detectedRoi, filter.metrics, filter.intensity)) {
          detectedRoi.changeClass(context.getClassId(filter.outputClass));
          break;
        }
      }
    }
  }
}

}    // namespace joda::cmd
