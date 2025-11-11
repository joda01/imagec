///
/// \file      margin_crop.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "backend/commands/command.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "hessian_settings.hpp"

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Gaussian Hessian (2D convolution)
///
class Hessian : public ImageProcessingCommand
{
public:
  /////////////////////////////////////////////////////
  explicit Hessian(const settings::HessianSettings &settings) : mSettings(settings)
  {
  }
  void execute(cv::Mat &image) override
  {
    cv::Mat gx;
    cv::Mat gy;

    // Detect input type
    bool was16bit = (image.depth() == CV_16U);
    bool was8bit  = (image.depth() == CV_8U);

    // Convert input to float for processing
    cv::Mat gray32f;
    if(was8bit) {
      image.convertTo(gray32f, CV_32F, 1.0 / 255.0);
    } else if(was16bit) {
      image.convertTo(gray32f, CV_32F, 1.0 / 65535.0);
    } else {
      gray32f = image;
    }

    cv::Mat dxx;
    cv::Mat dyy;
    cv::Mat dxy;
    cv::Sobel(image, dxx, CV_32F, 2, 0, 3);
    cv::Sobel(image, dyy, CV_32F, 0, 2, 3);
    cv::Sobel(image, dxy, CV_32F, 1, 1, 3);

    // Optional feature maps
    if(mSettings.mode == settings::HessianSettings::Mode::EigenvaluesX) {
      // Eigenvalues of Hessian
      cv::Mat tmp = (dxx - dyy).mul(dxx - dyy) + 4 * dxy.mul(dxy);
      cv::sqrt(tmp, tmp);
      cv::Mat l1 = 0.5 * (dxx + dyy + tmp);

      if(was16bit) {
        cv::Mat l1_16u;
        l1.convertTo(l1_16u, CV_16U, 65535.0);    // back to 16-bit range
        image = l1_16u;
      } else if(was8bit) {
        cv::Mat l1_8u;
        l1.convertTo(l1_8u, CV_8U, 255.0);
        image = l1_8u;
      } else {
        image = l1;
      }
    } else if(mSettings.mode == settings::HessianSettings::Mode::EigenvaluesY) {
      // Eigenvalues of Hessian
      cv::Mat tmp = (dxx - dyy).mul(dxx - dyy) + 4 * dxy.mul(dxy);
      cv::sqrt(tmp, tmp);
      cv::Mat l2 = 0.5 * (dxx + dyy - tmp);

      if(was16bit) {
        cv::Mat l2_16u;
        l2.convertTo(l2_16u, CV_16U, 65535.0);
        image = l2_16u;
      } else if(was8bit) {
        cv::Mat l2_8u;
        l2.convertTo(l2_8u, CV_8U, 255.0);
        image = l2_8u;
      } else {
        image = l2;
      }
    } else if(mSettings.mode == settings::HessianSettings::Mode::Determinant) {
      cv::Mat detH = dxx.mul(dyy) - dxy.mul(dxy);

      if(was16bit) {
        cv::Mat coherence16;
        detH.convertTo(coherence16, CV_16U, 65535.0);
        image = coherence16;
      } else if(was8bit) {
        cv::Mat coherence8;
        detH.convertTo(coherence8, CV_8U, 255.0);
        image = coherence8;
      } else {
        image = detH;
      }
    }
  }

private:
  /////////////////////////////////////////////////////
  const settings::HessianSettings &mSettings;
};

}    // namespace joda::cmd
