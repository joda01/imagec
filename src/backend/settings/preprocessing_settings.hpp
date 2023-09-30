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

#include "backend/image_processing/functions/blur/blur.hpp"
#include "backend/image_processing/functions/median_substraction/median_substraction.hpp"
#include <nlohmann/json.hpp>

class PreprocessingBlur final
{
public:
  int kernel_size = 0;

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PreprocessingBlur, kernel_size);
};

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

  ZStackMethod getZStackMethod() const
  {
    if(!value.empty()) {
      if(value == "NONE") {
        return PreprocessingZStack::ZStackMethod::NONE;
      }
      if(value == "PROJECT_MAX_INTENSITY") {
        return PreprocessingZStack::ZStackMethod::MAX_INTENSITY;
      }
      if(value == "PROJECT_3D") {
        return PreprocessingZStack::ZStackMethod::PROJECT_3D;
      }
    }
    return PreprocessingZStack::ZStackMethod::NONE;
  }

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

class PreprocessingMedianBackgroundSubtraction final
{
public:
  int32_t kernel_size{0};

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PreprocessingMedianBackgroundSubtraction, kernel_size);
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
  }

  void execute(cv::Mat &image) const
  {
    if(!z_stack.value.empty()) {
    }

    if(rolling_ball.value > 0) {
      joda::func::img::RollingBallBackground function(rolling_ball.value);
      function.execute(image);
    }

    if(margin_crop.value > 0) {
      joda::func::img::MarginCrop function(margin_crop.value);
      function.execute(image);
    }

    if(gaussian_blur.kernel_size > 0) {
      joda::func::img::Blur function(gaussian_blur.kernel_size);
      function.execute(image);
    }

    if(median_bg_subtraction.kernel_size > 0) {
      joda::func::img::MedianSubtraction function(median_bg_subtraction.kernel_size);
      function.execute(image);
    }
  }

  [[nodiscard]] auto getZStack() const -> const PreprocessingZStack *
  {
    if(!z_stack.value.empty()) {
      return &z_stack;
    }
    return nullptr;
  }

  [[nodiscard]] auto getRollingBall() const -> const PreprocessingRollingBall *
  {
    if(rolling_ball.value > 0) {
      return &rolling_ball;
    }
    return nullptr;
  }

  [[nodiscard]] auto getMarginCrop() const -> const PreprocessingMarginCrop *
  {
    if(margin_crop.value > 0) {
      return &margin_crop;
    }
    return nullptr;
  }

  [[nodiscard]] auto getGaussianBlur() const -> const PreprocessingBlur *
  {
    if(gaussian_blur.kernel_size > 0) {
      return &gaussian_blur;
    }
    return nullptr;
  }

  [[nodiscard]] auto getMedianBgSubtraction() const -> const PreprocessingMedianBackgroundSubtraction *
  {
    if(median_bg_subtraction.kernel_size > 0) {
      return &median_bg_subtraction;
    }
    return nullptr;
  }

private:
  /////////////////////////////////////////////////////
  PreprocessingZStack z_stack;
  PreprocessingRollingBall rolling_ball;
  PreprocessingMarginCrop margin_crop;
  PreprocessingBlur gaussian_blur;
  PreprocessingMedianBackgroundSubtraction median_bg_subtraction;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PreprocessingStep, z_stack, rolling_ball, margin_crop, gaussian_blur,
                                              median_bg_subtraction);
};
