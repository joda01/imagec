///
/// \file      cpu_flow_field.h
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <opencv2/core.hpp>

cv::Point2f cpuFlowIterationKernel(const cv::Mat &flowX, const cv::Mat &flowY, float startX, float startY, int numSteps, float stepSize);
