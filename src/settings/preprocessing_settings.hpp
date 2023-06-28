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

#include <nlohmann/json.hpp>

class PreprocessingZStack final
{
public:
  std::string function;
  double value;

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PreprocessingZStack, function, value);
};

class PreprocessingRollingBall final
{
public:
  std::string function;
  double value;

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PreprocessingRollingBall, function, value);
};

class PreprocessingMarginCrop final
{
public:
  std::string function;
  double value;

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PreprocessingMarginCrop, function, value);
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
    if(!z_stack.function.empty()) {
      activeFunction = Functions::Z_STACK;
    }

    if(!rolling_ball.function.empty()) {
      activeFunction = Functions::ROLLING_BALL;
    }

    if(!margin_crop.function.empty()) {
      activeFunction = Functions::MARGIN_CROP;
    }
  }

private:
  /////////////////////////////////////////////////////
  PreprocessingZStack z_stack;
  PreprocessingRollingBall rolling_ball;
  PreprocessingMarginCrop margin_crop;

  Functions activeFunction = Functions::NONE;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PreprocessingStep, z_stack, rolling_ball, margin_crop);
};
