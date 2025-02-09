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

#include "ai_classifier.hpp"
#include <exception>
#include <memory>
#include <string>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/commands/classification/ai_classifier/frameworks/ai_framework.hpp"
#include "backend/commands/classification/ai_classifier/frameworks/onnx/ai_classifier_onnx.hpp"
#include "backend/commands/classification/ai_classifier/frameworks/pytorch/ai_classifier_pytorch.hpp"
#include "backend/commands/classification/ai_classifier/models/ai_model.hpp"
#include "backend/commands/classification/ai_classifier/models/bioimage/ai_model_bioimage.hpp"
#include "backend/commands/classification/ai_classifier/models/yolo/ai_model_yolo.hpp"
#include "backend/enums/enum_objects.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core/matx.hpp>
#include <opencv2/core/persistence.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::cmd {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
/// \param[in]  onnxNetPath Path to the ONNX net file
/// \param[in]  classNames  Array of class names e.g. {"nuclues","cell"}
///
AiClassifier::AiClassifier(const settings::AiClassifierSettings &settings) : mSettings(settings)
{
}

void AiClassifier::execute(processor::ProcessContext &context, cv::Mat &imageNotUse, atom::ObjectList &result)
{
  if(mSettings.modelPath.empty()) {
    return;
  }
  std::vector<joda::ai::AiModel::Result> segResult;

  auto params = joda::ai::AiFramework::InputParameters{
      .axesOrder    = mSettings.modelInputParameter.axes,
      .dataType     = static_cast<joda::ai::AiFramework::InputParameters::NetInputDataType>(mSettings.modelInputParameter.dataType),
      .batchSize    = mSettings.modelInputParameter.batch,
      .nrOfChannels = static_cast<int32_t>(mSettings.modelInputParameter.channels),
      .inputWidth   = mSettings.modelInputParameter.spaceX,
      .inputHeight  = mSettings.modelInputParameter.spaceY};

  at::IValue prediction;
  switch(mSettings.modelParameter.modelFormat) {
    case settings::AiClassifierSettings::ModelFormat::UNKNOWN:
      THROW("Unsupported model format!");
      break;

    case settings::AiClassifierSettings::ModelFormat::ONNX: {
      joda::ai::AiFrameworkOnnx onnxClassifier(mSettings.modelPath, params);
      prediction = onnxClassifier.predict(imageNotUse);
    } break;

    case settings::AiClassifierSettings::ModelFormat::TORCHSCRIPT: {
      joda::ai::AiFrameworkPytorch torch(mSettings.modelPath, params);
      prediction = torch.predict(imageNotUse);
    } break;

    case settings::AiClassifierSettings::ModelFormat::TENSORFLOW:
      THROW("Tensorflow is not yet supported!");
      break;
  }

  switch(mSettings.modelParameter.modelArchitecture) {
    case settings::AiClassifierSettings::ModelArchitecture::UNKNOWN:
      THROW("Unsupported architecture!");
      break;
    case settings::AiClassifierSettings::ModelArchitecture::YOLO_V5: {
      ai::AiModelYolo yoloy({.maskThreshold = mSettings.thresholds.maskThreshold, .classThreshold = mSettings.thresholds.classThreshold});
      segResult = yoloy.processPrediction(imageNotUse, prediction);
    } break;
    case settings::AiClassifierSettings::ModelArchitecture::STAR_DIST:
    case settings::AiClassifierSettings::ModelArchitecture::U_NET: {
      ai::AiModelBioImage bioImage({.maskThreshold = mSettings.thresholds.maskThreshold, .contourThreshold = 0.3});
      segResult = bioImage.processPrediction(imageNotUse, prediction);
    } break;
    case settings::AiClassifierSettings::ModelArchitecture::MASK_R_CNN:
      THROW("Mask R-CNN architecture is not supported yet");
      break;
    case settings::AiClassifierSettings::ModelArchitecture::CYTO3:
      THROW("Cyto3 architecture is not supported yet");
      break;
  }

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

      result.push_back(std::move(detectedRoi));
    }
  }
}

}    // namespace joda::cmd
