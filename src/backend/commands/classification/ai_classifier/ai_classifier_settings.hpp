///
/// \file      classifier_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <cstdint>
#include <set>
#include "backend/commands/classification/classifier_filter.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include "backend/settings/settings_types.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct AiClassifierSettings : public SettingBase
{
  enum class NetInputDataType
  {
    FLOAT32,
    UINT8,
    UINT16,
    UINT32
  };

  enum class NetChannels
  {
    GRAYSCALE = 1,
    RGB       = 3
  };

  enum class ModelFormat
  {
    UNKNOWN,
    ONNX,
    TORCHSCRIPT,
    TENSORFLOW
  };

  enum class ModelArchitecture
  {
    UNKNOWN,
    YOLO_V5,
    STAR_DIST,
    CYTO3,
    U_NET,
    MASK_R_CNN,
  };

  struct ModelParameters
  {
    //
    // Model format
    //
    ModelFormat modelFormat = ModelFormat::ONNX;

    //
    // Model architecture
    //
    ModelArchitecture modelArchitecture = ModelArchitecture::UNKNOWN;

    void check() const
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ModelParameters, modelFormat, modelArchitecture);
  };

  struct NetInputParameters
  {
    //
    // Axes input order (b=batch, c = channel, y = height, x = width)
    //
    std::string axes = "bcyx";

    //
    // Batch size
    //
    int32_t batch = 1;

    //
    // Nr. of input channels. 1=gray scale, 3 = color
    //
    NetChannels channels = NetChannels::RGB;

    //
    // The expected image input with of the net
    //
    int32_t spaceX = 256;

    //
    // The expected image input heigh the net
    //
    int32_t spaceY = 256;

    //
    // Data type input
    //
    NetInputDataType dataType = NetInputDataType::FLOAT32;

    void check() const
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(NetInputParameters, axes, batch, channels, spaceX, spaceY, dataType);
  };

  struct NetOutputParameters
  {
    //
    // Axes input order (b=batch, c = channel, y = height, x = width)
    //
    std::string axes = "bcyx";

    //
    // Batch size
    //
    int32_t batch = 0;

    //
    // Nr. of output channels
    //
    int32_t channels = 0;

    //
    // The expected image input with of the net
    //
    int32_t spaceX = 0;

    //
    // The expected image input heigh the net
    //
    int32_t spaceY = 0;

    //
    // Index size
    //
    int32_t index = 0;
  };

  struct Thresholds
  {
    //
    // Default class threshold used to mark an object as object
    //
    float classThreshold = 0.5;

    //
    // Default mask threshold
    //
    float maskThreshold = 0.8;

    void check() const
    {
      CHECK_ERROR(classThreshold >= 0, "Class threshold must be >0.");
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Thresholds, classThreshold, maskThreshold);
  };

  //
  // Path to the AI model which should be used for classification
  //
  std::string modelPath;

  //
  // Model parameter
  //
  ModelParameters modelParameter;

  //
  // Model input parameter settings
  //
  NetInputParameters modelInputParameter;

  //
  // Vector array index is the class ID used by the AI model starting with 0
  //
  std::list<ObjectClass> modelClasses = {{}};

  //
  // The thresholds used for the prediction
  //
  Thresholds thresholds;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(!modelPath.empty(), "A AI model path must be given!");
    CHECK_ERROR(std::filesystem::exists(modelPath), "AI model >" + modelPath + "< cannot be opened!");
    CHECK_ERROR(!modelClasses.empty(), "At least one classifier must be given!");
  }

  [[nodiscard]] ObjectOutputClasses getOutputClasses() const override
  {
    ObjectOutputClasses out;
    for(const auto &clas : modelClasses) {
      out.emplace(clas.outputClassNoMatch);
      for(const auto &clasInner : clas.filters) {
        out.emplace(clasInner.outputClass);
      }
    }
    return out;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(AiClassifierSettings, modelPath, modelParameter, thresholds, modelClasses);
};

NLOHMANN_JSON_SERIALIZE_ENUM(AiClassifierSettings::NetInputDataType, {
                                                                         {AiClassifierSettings::NetInputDataType::FLOAT32, "float32"},
                                                                         {AiClassifierSettings::NetInputDataType::UINT8, "uint8"},
                                                                         {AiClassifierSettings::NetInputDataType::UINT16, "uint16"},
                                                                         {AiClassifierSettings::NetInputDataType::UINT32, "uint32"},
                                                                     });

NLOHMANN_JSON_SERIALIZE_ENUM(AiClassifierSettings::NetChannels, {
                                                                    {AiClassifierSettings::NetChannels::GRAYSCALE, 1},
                                                                    {AiClassifierSettings::NetChannels::RGB, 3},
                                                                });

NLOHMANN_JSON_SERIALIZE_ENUM(AiClassifierSettings::ModelFormat, {
                                                                    {AiClassifierSettings::ModelFormat::UNKNOWN, "Unknown"},
                                                                    {AiClassifierSettings::ModelFormat::ONNX, "Onnx"},
                                                                    {AiClassifierSettings::ModelFormat::TORCHSCRIPT, "Torchscript"},
                                                                    {AiClassifierSettings::ModelFormat::TENSORFLOW, "Tensorflow"},
                                                                });

NLOHMANN_JSON_SERIALIZE_ENUM(AiClassifierSettings::ModelArchitecture, {
                                                                          {AiClassifierSettings::ModelArchitecture::UNKNOWN, "Unknown"},
                                                                          {AiClassifierSettings::ModelArchitecture::YOLO_V5, "YoloV5"},
                                                                          {AiClassifierSettings::ModelArchitecture::CYTO3, "Cyto3"},
                                                                          {AiClassifierSettings::ModelArchitecture::STAR_DIST, "StarDist"},
                                                                          {AiClassifierSettings::ModelArchitecture::U_NET, "U-Net"},
                                                                          {AiClassifierSettings::ModelArchitecture::MASK_R_CNN, "Mask R-CNN"},
                                                                      });

}    // namespace joda::settings
