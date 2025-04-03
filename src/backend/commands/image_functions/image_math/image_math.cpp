///
/// \file      Intersection.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "image_math.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include "image_math_settings.hpp"

namespace joda::cmd {

ImageMath::ImageMath(const settings::ImageMathSettings &settings) : mSettings(settings)
{
}

void ImageMath::execute(processor::ProcessContext &context, cv::Mat &imageInOut, atom::ObjectList & /*resultIn*/)
{
  auto const *img2 = &context.loadImageFromCache(enums::MemoryScope::ITERATION, mSettings.inputImageSecond)->image;
  auto const *img1 = &imageInOut;
  if(mSettings.operatorOrder == settings::ImageMathSettings::OperationOrder::BoA) {
    img1 = img2;
    img2 = &imageInOut;
  }

  switch(mSettings.function) {
    case settings::ImageMathSettings::Function::INVERT:
      cv::bitwise_not(*img1, *img1);
      break;
    case settings::ImageMathSettings::Function::AND:
      cv::bitwise_and(*img1, *img2, imageInOut);
      break;
    case settings::ImageMathSettings::Function::OR:
      cv::bitwise_or(*img1, *img2, imageInOut);
      break;
    case settings::ImageMathSettings::Function::XOR:
      cv::bitwise_xor(*img1, *img2, imageInOut);
      break;
    case settings::ImageMathSettings::Function::ADD:
      cv::add(*img1, *img2, imageInOut);
      break;
    case settings::ImageMathSettings::Function::SUBTRACT:
      cv::subtract(*img1, *img2, imageInOut);
      break;
    case settings::ImageMathSettings::Function::MULTIPLY:
      cv::multiply(*img1, *img2, imageInOut);
      break;
    case settings::ImageMathSettings::Function::DIVIDE: {
      // Normalize if needed (optional, depends on expected output range)
      double minVal;
      double maxAbefore;
      double maxBbefore;
      cv::minMaxLoc(*img1, &minVal, &maxAbefore);
      cv::minMaxLoc(*img2, &minVal, &maxBbefore);
      maxAbefore = std::max(maxAbefore, maxBbefore);

      cv::Mat imgA;
      img1->convertTo(imgA, CV_32F);    // Normalize to [0,1]
      cv::Mat imgB;
      img2->convertTo(imgB, CV_32F);    // Normalize to [0,1]
      cv::divide(imgA, imgB, imgB);
      imgB.setTo(0, imgB == std::numeric_limits<float>::infinity());

      // Normalize if needed (optional, depends on expected output range)
      double maxVal;
      cv::minMaxLoc(imgB, &minVal, &maxVal);
      imgB.convertTo(imageInOut, CV_16UC1, maxAbefore / maxVal);
      break;
    }
    case settings::ImageMathSettings::Function::MIN:
      cv::min(*img1, *img2, imageInOut);
      break;
    case settings::ImageMathSettings::Function::MAX:
      cv::max(*img1, *img2, imageInOut);
      break;
    case settings::ImageMathSettings::Function::AVERAGE:
      imageInOut = (*img1 + *img2) / 2.0;
      break;
    case settings::ImageMathSettings::Function::DIFFERENCE_TYPE:
      imageInOut = cv::abs(*img1 - *img2);
      break;
    default:
      break;
  }
}

}    // namespace joda::cmd
