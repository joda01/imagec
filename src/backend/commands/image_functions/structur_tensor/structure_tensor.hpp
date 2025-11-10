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
#include "structure_tensor_settings.hpp"

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Gaussian StructureTensor (2D convolution)
///
class StructureTensor : public ImageProcessingCommand
{
public:
  /////////////////////////////////////////////////////
  explicit StructureTensor(const settings::StructureTensorSettings &settings) : mSettings(settings)
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

    // Compute gradients
    cv::Sobel(gray32f, gx, CV_32F, 1, 0, 3);
    cv::Sobel(gray32f, gy, CV_32F, 0, 1, 3);

    // Structure tensor components
    cv::Mat Jxx = gx.mul(gx);
    cv::Mat Jyy = gy.mul(gy);
    cv::Mat Jxy = gx.mul(gy);

    // Smooth tensor components
    cv::GaussianBlur(Jxx, Jxx, cv::Size(mSettings.kernelSize, mSettings.kernelSize), 1.0);
    cv::GaussianBlur(Jyy, Jyy, cv::Size(mSettings.kernelSize, mSettings.kernelSize), 1.0);
    cv::GaussianBlur(Jxy, Jxy, cv::Size(mSettings.kernelSize, mSettings.kernelSize), 1.0);

    // Eigenvalues λ1, λ2
    cv::Mat tmp = (Jxx - Jyy).mul(Jxx - Jyy) + 4.0 * Jxy.mul(Jxy);
    cv::sqrt(tmp, tmp);
    cv::Mat l1 = 0.5 * (Jxx + Jyy + tmp);
    cv::Mat l2 = 0.5 * (Jxx + Jyy - tmp);

    // Optional feature maps
    if(mSettings.mode == settings::StructureTensorSettings::Mode::StructureTensorEigenvaluesX) {
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
    } else if(mSettings.mode == settings::StructureTensorSettings::Mode::StructureTensorEigenvaluesY) {
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
    } else if(mSettings.mode == settings::StructureTensorSettings::Mode::StructureTensorCoherence) {
      cv::Mat coherence = (l1 - l2) / (l1 + l2 + 1e-6);

      if(was16bit) {
        cv::Mat coherence16;
        coherence.convertTo(coherence16, CV_16U, 65535.0);
        image = coherence16;
      } else if(was8bit) {
        cv::Mat coherence8;
        coherence.convertTo(coherence8, CV_8U, 255.0);
        image = coherence8;
      } else {
        image = coherence;
      }
    }
  }

private:
  /////////////////////////////////////////////////////
  const settings::StructureTensorSettings &mSettings;
};

}    // namespace joda::cmd
