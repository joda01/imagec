///
/// \file      classifier_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

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

  enum class Format
  {
    ONNX,
    PYTORCH,
    TENSORFLOW
  };

  struct NetInputParameters
  {
    //
    // Axes input order (b=batch, c = channel, y = height, x = width)
    //
    std::string axesOrder = "bcyx";

    //
    // The expected image input with of the net
    //
    int32_t netInputWidth = 256;

    //
    // The expected image input heigh the net
    //
    int32_t netInputHeight = 256;

    //
    // Batch size
    //
    int32_t netInputBatchSize = 1;

    //
    // Nr. of input channels. 1=gray scale, 3 = color
    //
    NetChannels netNrOfChannels = NetChannels::RGB;

    //
    // Data type input
    //
    NetInputDataType netInputType = NetInputDataType::FLOAT32;

    void check() const
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(NetInputParameters, axesOrder, netInputWidth, netInputHeight, netInputBatchSize,
                                                         netNrOfChannels, netInputType);
  };

  //
  // Path to the AI model which should be used for classification
  //
  std::string modelPath;

  //
  // Model input parameter settings
  //
  NetInputParameters modelInputParameters;

  //
  // Vector array index is the class ID used by the AI model starting with 0
  //
  std::list<ObjectClass> modelClasses = {{}};

  //
  // Default class threshold used to mark an object as object
  //
  float classThreshold = 0.5;

  //
  // Default mask threshold
  //
  float maskThreshold = 0.8;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(!modelPath.empty(), "A AI model path must be given!");
    CHECK_ERROR(std::filesystem::exists(modelPath), "AI model >" + modelPath + "< cannot be opened!");
    CHECK_ERROR(classThreshold >= 0, "Class threshold must be >0.");
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

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(AiClassifierSettings, modelPath, classThreshold, maskThreshold, modelClasses);
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

NLOHMANN_JSON_SERIALIZE_ENUM(AiClassifierSettings::Format, {
                                                               {AiClassifierSettings::Format::ONNX, "Onnx"},
                                                               {AiClassifierSettings::Format::PYTORCH, "Pytorch"},
                                                               {AiClassifierSettings::Format::TENSORFLOW, "Tensorflow"},
                                                           });

}    // namespace joda::settings
