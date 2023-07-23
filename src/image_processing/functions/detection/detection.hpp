///
/// \file      detection.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-23
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <string>
#include "image_processing/functions/func_types.hpp"

namespace joda::func {

///
/// \class      Detection
/// \author     Joachim Danmayr
/// \brief
///
class DetectionFunction
{
public:
  /////////////////////////////////////////////////////
  virtual auto forward(const cv::Mat &srcImg) -> DetectionResponse = 0;

  ///
  /// \brief      Calculate metrics based on bounding box and mask
  /// \author     Joachim Danmayr
  ///
  static void calculateMetrics(Detection &detection, const cv::Mat &image, const cv::Rect &rect, const cv::Mat &mask)
  {
    double intensity    = 0;
    double intensityMin = USHRT_MAX;
    double intensityMax = 0;

    uint64_t areaSize = 0;

    // std::cout << "MAsk " << std::to_string(mask.channels()) << " x " << std::to_string(mask.type()) << std::endl;

    // Calculate the intensity and area of the polygon ROI
    for(int x = 0; x < rect.width; x++) {
      for(int y = 0; y < rect.height; y++) {
        unsigned char maskPxl = mask.at<unsigned char>(y, x);    // Get the pixel value at (x, y)
        if(maskPxl > 0) {
          double pixelGrayScale = image.at<unsigned short>(y, x);    // Get the pixel value at (x, y)
          if(pixelGrayScale < intensityMin) {
            intensityMin = pixelGrayScale;
          }
          if(pixelGrayScale > intensityMax) {
            intensityMax = pixelGrayScale;
          }
          intensity += pixelGrayScale;
          areaSize++;
        }
      }
    }
    float intensityAvg = 0.0f;
    if(areaSize > 0) {
      intensityAvg = intensity / static_cast<float>(areaSize);
    }
    detection.intensity    = intensityAvg;
    detection.intensityMin = intensityMin;
    detection.intensityMax = intensityMax;
    detection.areaSize     = areaSize;

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    float circularity = 0;
    if(!contours.empty()) {
      double area      = cv::contourArea(contours[0]);
      double perimeter = cv::arcLength(contours[0], true);
      if(perimeter != 0) {
        circularity = (4 * M_PI * area) / (perimeter * perimeter);
      } else {
        circularity = 1;
      }
    }
    detection.circularity = circularity;
  }

  ///
  /// \brief      Paints the masks and bounding boxes around the found elements
  /// \author     Joachim Danmayr
  /// \param[in]  img    Image where the mask should be painted on
  /// \param[in]  result Prediction result of the forward
  ///
  void paintBoundingBox(cv::Mat &img, const DetectionResults &result)
  {
    cv::Mat mask = img.clone();

    for(int i = 0; i < result.size(); i++) {
      if(!result[i].box.empty() && !result[i].boxMask.empty() && !mask.empty()) {
        int left      = result[i].box.x;
        int top       = result[i].box.y;
        int color_num = i;
        rectangle(img, result[i].box, RED, 1, cv::LINE_4);

        mask(result[i].box).setTo(RED, result[i].boxMask);
        //  string label =
        //      mClassNames[result[i].classId] + ":" + to_string(result[i].confidence) + ":" +
        //      to_string(result[i].index);

        std::string label = std::to_string(result[i].index);

        int baseLine       = 0;
        cv::Size labelSize = getTextSize(label, cv::FONT_HERSHEY_PLAIN, 0.5, 1, &baseLine);
        top                = cv::max(top, labelSize.height);
        rectangle(img, cv::Point(left, top - int(1.5 * labelSize.height)),
                  cv::Point(left + int(1.5 * labelSize.width), top + baseLine), BLACK, cv::FILLED);
        putText(img, label, cv::Point(left, top), cv::FONT_HERSHEY_PLAIN, 0.5, WHITE, 1);
      }
    }
    addWeighted(mask, 0.5, img, 1, 0, img);
  }

protected:
  /////////////////////////////////////////////////////
  const cv::Scalar BLACK  = cv::Scalar(0, 0, 0);
  const cv::Scalar WHITE  = cv::Scalar(255, 255, 255);
  const cv::Scalar YELLOW = cv::Scalar(0, 255, 255);
  const cv::Scalar RED    = cv::Scalar(0, 0, 255);
  const cv::Scalar GREEN  = cv::Scalar(0, 255, 0);
};
}    // namespace joda::func
