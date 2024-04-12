///
/// \file      rolling_ball.hpp
/// \author    Joachim Danmayr
/// \date      2023-02-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     C++ implementation of rolling ball algorithm based on
///            ImageJ rolling ball suggested by Michael Castle and Janice Keller
///            https://imagej.net/plugins/rolling-ball-background-subtraction
///

#pragma once

#include <cmath>
#include <vector>
#include "../../functions/function.hpp"
#include "backend/settings/preprocessing/functions/rolling_ball.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::func::img {

class RollingBall;

///
/// \class    RollingBallBackground
/// \author   Joachim Danmayr
/// \brief    Rolling ball background substraction
///
class RollingBallBackground : public Function
{
public:
  enum class Configuration
  {
    BALL,
    PARABOLOID
  };

  /////////////////////////////////////////////////////
  explicit RollingBallBackground(const joda::settings::RollingBall &settings) :
      mUseSlidingParaboloid(settings.ballType == joda::settings::RollingBall::BallType::PARABOLOID),
      radius(settings.ballSize)
  {
  }

  virtual ~RollingBallBackground() = default;
  void execute(cv::Mat & /*image*/) const override;

private:
  /////////////////////////////////////////////////////
  static int constexpr MAXIMUM     = 0;
  static int constexpr MEAN        = 1;    // filter types of filter3x3
  static int constexpr X_DIRECTION = 0, Y_DIRECTION = 1, DIAGONAL_1A = 2, DIAGONAL_1B = 3, DIAGONAL_2A = 4,
                       DIAGONAL_2B = 5;    // filter directions

  /////////////////////////////////////////////////////
  void rollingBallFloatBackground(cv::Mat &fp, float radius, bool invert, bool doPresmooth, RollingBall *ball) const;

  void slidingParaboloidFloatBackground(cv::Mat &fp, float radius, bool invert, bool doPresmooth,
                                        bool correctCorners) const;

  void correctCorners(cv::Mat &fp, float coeff2, float *cache, int *nextPoint) const;
  void filter1D(cv::Mat &fp, int direction, float coeff2, float *cache, int *nextPoint) const;
  float *lineSlideParabola(cv::Mat &pixels, int start, int inc, int length, float coeff2, float *cache, int *nextPoint,
                           float *correctedEdges) const;

  cv::Mat shrinkImage(const cv::Mat &ip, int shrinkFactor) const;
  void enlargeImage(const cv::Mat &smallImage, cv::Mat &fp, int shrinkFactor) const;
  void rollBall(RollingBall *ball, cv::Mat &fp) const;
  static void makeInterpolationArrays(int *smallIndices, float *weights, int length, int smallLength, int shrinkFactor);

  double filter3x3(cv::Mat &ip, int type) const;
  double filter3(cv::Mat &ip, int length, int pixel0, int inc, int type) const;
  // void setNPasses(int nPasses);

  /////////////////////////////////////////////////////
  bool mUseSlidingParaboloid = false;
  const float radius;
  const bool lightBackground = false;
  const int nPasses          = 1;
  const int flags = 0;    //= DOES_8G | DOES_16 | DOES_RGB | FINAL_PROCESSING | KEEP_PREVIEW | PARALLELIZE_STACKS;
};
}    // namespace joda::func::img
