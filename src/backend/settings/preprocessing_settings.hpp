///
/// \file      preprocessing_settings.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <memory>
#include "../image_processing/functions/function.hpp"
#include "../image_processing/functions/margin_crop/margin_crop.hpp"
#include "../image_processing/functions/rolling_ball/rolling_ball.hpp"

#include <nlohmann/json.hpp>

class PreprocessingZStack final
{
public:
  enum ZStackMethod
  {
    NONE,
    MAX_INTENSITY,
    PROJECT_3D
  };
  std::string value;

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PreprocessingZStack, value);
};

class PreprocessingRollingBall final
{
public:
  int32_t value{0};

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PreprocessingRollingBall, value);
};

class PreprocessingMarginCrop final
{
public:
  int32_t value{0};

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PreprocessingMarginCrop, value);
};

///
/// \class      PreprocessingStep
/// \author     Joachim Danmayr
/// \brief      Parses all available preprocessing steps
///
class PreprocessingStep final
{
public:
  enum class Functions
  {
    NONE,
    Z_STACK,
    MARGIN_CROP,
    ROLLING_BALL,
  };

  void interpretConfig()
  {
    if(!z_stack.value.empty()) {
    }

    if(rolling_ball.value > 0) {
      mPreprocessingFunction = std::make_shared<joda::func::img::RollingBallBackground>(rolling_ball.value);
    }

    if(margin_crop.value > 0) {
      mPreprocessingFunction = std::make_shared<joda::func::img::MarginCrop>(margin_crop.value);
    }
  }

  void execute(cv::Mat &image) const
  {
    if(mPreprocessingFunction != nullptr) {
      mPreprocessingFunction->execute(image);
    }
  }

  PreprocessingZStack::ZStackMethod getZStackMethod()
  {
    if(!z_stack.value.empty()) {
      if(z_stack.value == "NONE") {
        return PreprocessingZStack::ZStackMethod::NONE;
      }
      if(z_stack.value == "PROJECT_MAX_INTENSITY") {
        return PreprocessingZStack::ZStackMethod::MAX_INTENSITY;
      }
      if(z_stack.value == "PROJECT_3D") {
        return PreprocessingZStack::ZStackMethod::PROJECT_3D;
      }
    }
    return PreprocessingZStack::ZStackMethod::NONE;
  }

private:
  /////////////////////////////////////////////////////
  PreprocessingZStack z_stack;
  PreprocessingRollingBall rolling_ball;
  PreprocessingMarginCrop margin_crop;

  std::shared_ptr<joda::func::img::Function> mPreprocessingFunction;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PreprocessingStep, z_stack, rolling_ball, margin_crop);
};
