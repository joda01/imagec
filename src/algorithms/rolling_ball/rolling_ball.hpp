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

#include <cmath>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

class RollingBall;

///
/// \class    RollingBallBackground
/// \author   Joachim Danmayr
/// \brief    Rolling ball background substraction
///
class RollingBallBackground
{
public:
  /////////////////////////////////////////////////////
  void run(cv::Mat &ip);

private:
  /////////////////////////////////////////////////////
  void subtractRGBBackround(cv::Mat &ip, int ballRadius);
  void subtractBackround(cv::Mat &ip, int ballRadius);
  std::shared_ptr<cv::Mat> rollBall(RollingBall &ball, cv::Mat &image, std::shared_ptr<cv::Mat> smallImage);
  std::shared_ptr<cv::Mat> shrinkImage(cv::Mat &ip, int shrinkfactor);
  void interpolateBackground(std::shared_ptr<cv::Mat> background, RollingBall &ball);
  void extrapolateBackground(std::shared_ptr<cv::Mat> background, RollingBall &ball);
  std::shared_ptr<cv::Mat> rollBall16(RollingBall &ball, cv::Mat &image, std::shared_ptr<cv::Mat> smallImage);
  void interpolateBackground16(std::shared_ptr<cv::Mat> background, RollingBall &ball);
  void extrapolateBackground16(std::shared_ptr<cv::Mat> background, RollingBall &ball);
  void setNPasses(int nPasses);

  /////////////////////////////////////////////////////
  int radius           = 50.0;    // default rolling ball radius
  bool lightBackground = false;
  bool previewing;
  bool invert = false;
  int nPasses = 1;
  int pass;
  int flags = 0;    //= DOES_8G | DOES_16 | DOES_RGB | FINAL_PROCESSING | KEEP_PREVIEW | PARALLELIZE_STACKS;
};
