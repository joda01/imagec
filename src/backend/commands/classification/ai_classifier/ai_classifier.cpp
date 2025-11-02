///
/// \file      ai_object_segmentation.cpp
/// \author    Joachim Danmayr
/// \date      2023-02-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///\link       https :// github.com/UNeedCryDear/yolov5-seg-opencv-onnxruntime-cpp

#include "ai_classifier.hpp"
#include <exception>
#include <filesystem>
#include <memory>
#include <string>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/commands/classification/ai_classifier/ai_classifier_settings.hpp"
#include "backend/commands/classification/ai_classifier/frameworks/ai_framework.hpp"
#include "backend/commands/classification/ai_classifier/frameworks/onnx/ai_classifier_onnx.hpp"
#include "backend/commands/classification/ai_classifier/frameworks/pytorch/ai_classifier_pytorch.hpp"
#include "backend/commands/classification/ai_classifier/models/ai_model.hpp"
#include "backend/commands/classification/ai_classifier/models/cyto3/ai_model_cyto3.hpp"
#include "backend/commands/classification/ai_classifier/models/unet/ai_model_unet.hpp"
#include "backend/commands/classification/ai_classifier/models/yolo/ai_model_yolo.hpp"
#include "backend/enums/enum_objects.hpp"
#include "backend/helper/ai_model_parser/ai_model_parser.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core/matx.hpp>
#include <opencv2/core/persistence.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::cmd {

at::Device getCudaDevice(bool allowGpu, c10::DeviceIndex index = 0);

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
  at::Device device            = getCudaDevice(mSettings.gpuUsage == settings::AiClassifierSettings::GpuUsage::Auto);
  const auto absoluteModelPath = std::filesystem::weakly_canonical(context.getWorkingDirectory() / mSettings.modelPath);

  auto parsed = joda::ai::AiModelParser::parseResourceDescriptionFile(absoluteModelPath);
  if(parsed.inputs.empty()) {
    THROW("Could not read model input parameter!");
  }
  mSettings.modelInputParameter = parsed.inputs.begin()->second;

  if(absoluteModelPath.empty()) {
    return;
  }
  auto modelPath = std::filesystem::current_path() / std::filesystem::path(absoluteModelPath);
  if(!std::filesystem::exists(modelPath)) {
    THROW("Could not open model >" + modelPath.string() + "<!");
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
      joda::ai::AiFrameworkOnnx onnxClassifier(modelPath.string(), params);
      prediction = onnxClassifier.predict(device, imageNotUse);
    } break;

    case settings::AiClassifierSettings::ModelFormat::TORCHSCRIPT: {
      joda::ai::AiFrameworkPytorch torch(modelPath.string(), params);
      prediction = torch.predict(device, imageNotUse);
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
      segResult = yoloy.processPrediction(device, imageNotUse, prediction);
    } break;
    case settings::AiClassifierSettings::ModelArchitecture::STAR_DIST:
    case settings::AiClassifierSettings::ModelArchitecture::U_NET: {
      ai::AiModelUNet bioImage({.maskThreshold = mSettings.thresholds.maskThreshold, .contourThreshold = 0.3F});
      segResult = bioImage.processPrediction(device, imageNotUse, prediction);
    } break;
    case settings::AiClassifierSettings::ModelArchitecture::MASK_R_CNN:
      THROW("Mask R-CNN architecture is not supported yet");
      break;
    case settings::AiClassifierSettings::ModelArchitecture::CYTO3:
      ai::AiModelCyto3 cyto3({.maskThreshold = mSettings.thresholds.maskThreshold, .classThreshold = mSettings.thresholds.classThreshold});
      segResult = cyto3.processPrediction(device, imageNotUse, prediction);
      break;
  }

  for(const auto &res : segResult) {
    //
    // Apply the filter based on the object class
    //
    if(static_cast<int32_t>(mSettings.modelClasses.size()) > res.classId) {
      auto objectClassToUse = *mSettings.modelClasses.begin();
      for(const auto &objectClass : mSettings.modelClasses) {
        if(objectClass.pixelClassId == res.classId) {
          objectClassToUse = objectClass;
          break;
        }
      }

      joda::atom::ROI detectedRoi(
          atom::ROI::RoiObjectId{
              .classId    = context.getClassId(objectClassToUse.outputClassNoMatch),
              .imagePlane = context.getActIterator(),
          },
          context.getAppliedMinThreshold(), res.boundingBox, res.mask, res.contour, context.getTileInfo());

      for(const auto &filter : objectClassToUse.filters) {
        if(joda::settings::ClassifierFilter::doesFilterMatch(context, detectedRoi, filter.metrics, filter.intensity)) {
          detectedRoi.changeClass(context.getClassId(filter.outputClass), 0);
          break;
        }
      }
      if(detectedRoi.getClassId() != enums::ClassId::NONE) {
        result.push_back(detectedRoi);
      }
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
at::Device getCudaDevice(bool allowGpu, c10::DeviceIndex index)
{
  if(allowGpu && torch::cuda::is_available()) {
    return {torch::kCUDA, index};
  }
  return {at::kCPU};
}

}    // namespace joda::cmd
