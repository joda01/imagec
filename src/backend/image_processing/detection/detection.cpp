///
/// \file      detection.cpp
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

#include "detection.hpp"
#include <random>
#include <string>
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/types.hpp>

namespace joda::image::detect {

std::string generateRandomColorHex()
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 255);

  // Generate random values for R, G, and B components
  int r = dis(gen);
  int g = dis(gen);
  int b = dis(gen);

  // Convert the values to hexadecimal string format
  std::stringstream ss;
  ss << std::hex << (r << 16 | g << 8 | b);
  std::string hexColor = ss.str();

  // If the hexadecimal string is less than 6 characters, pad it with zeros
  while(hexColor.length() < 6) {
    hexColor = "0" + hexColor;
  }

  return hexColor;
}

///
/// \brief      Paints the masks and bounding boxes around the found elements
/// \author     Joachim Danmayr
/// \param[in]  img    Image where the mask should be painted on
/// \param[in]  result Prediction result of the forward
///
void DetectionFunction::paintBoundingBox(cv::Mat &img, const std::unique_ptr<DetectionResults> &result,
                                         const joda::onnx::OnnxParser::Data &modelInfo, const std::string &fillColor,
                                         bool paintRectangel, bool paintLabels)
{
  auto id              = DurationCount::start("Paint bounding box");
  cv::Mat mask         = img.clone();
  cv::Scalar areaColor = hexToScalar("#" + generateRandomColorHex());

  int i = 0;
  for(const auto &roi : *result) {
    int left      = roi.getBoundingBox().x;
    int top       = roi.getBoundingBox().y;
    int width     = roi.getBoundingBox().width;
    int height    = roi.getBoundingBox().height;
    int color_num = ++i;

    if(!roi.getMask().empty() && !roi.getBoundingBox().empty()) {
      try {
        if(!roi.isValid()) {
          areaColor = WHITE;
        } else {
          areaColor = hexToScalar("#" + generateRandomColorHex());
        }

        // Boundding box
        if(paintRectangel && !roi.getBoundingBox().empty()) {
          rectangle(img, roi.getBoundingBox(), areaColor, 1 * THICKNESS, cv::LINE_4);
        }

        // Fill area
        mask(roi.getBoundingBox()).setTo(areaColor, roi.getMask());

        // Paint contour only for valid particles
        if(roi.isValid()) {
          cv::Scalar contourColor = GREEN;    // hexToScalar(fillColor);

          {
            std::vector<std::vector<cv::Point>> contours;
            contours.push_back(roi.getContour());
            if(!contours.empty())
              drawContours(img(roi.getBoundingBox()), contours, -1, contourColor, 1);
          }
          if(roi.hasSnapArea()) {
            std::vector<std::vector<cv::Point>> contours;
            contours.push_back(roi.getSnapAreaContour());
            if(!contours.empty())
              drawContours(img(roi.getSnapAreaBoundingBox()), contours, -1, contourColor, 1);
          }
        }
      } catch(const std::exception &ex) {
        std::cout << "P" << ex.what() << std::endl;
      }
    }
    if(paintLabels) {
      std::string label = std::to_string(roi.getIndex()) + " | " + std::to_string(roi.getConfidence());
      if(modelInfo.classes.size() > roi.getClassId()) {
        label += " | " + modelInfo.classes[roi.getClassId()];
      }

      drawLabel(img, areaColor, label, left, top);
    }
  }
  addWeighted(mask, 0.5, img, 1, 0, img);

  DurationCount::stop(id);
}

cv::Scalar DetectionFunction::hexToScalar(const std::string &hexColor)
{
  int r, g, b;
  sscanf(hexColor.c_str(), "#%2x%2x%2x", &r, &g, &b);
  return cv::Scalar(b, g, r);    // OpenCV uses BGR order
}

struct OverlaySettings
{
  const DetectionResults &result;
  cv::Scalar backgroundColor;
  cv::Scalar borderColor;
  bool paintRectangel;
  float opaque;
};

void DetectionFunction::paintOverlay(cv::Mat &img, const std::vector<OverlaySettings> &overlays)
{
  for(const auto &ov : overlays) {
    cv::Mat mask = img.clone();
    int i        = 0;
    for(const auto &roi : *ov.result) {
      const auto &resultI = roi;

      int left      = resultI.getBoundingBox().x;
      int top       = resultI.getBoundingBox().y;
      int width     = resultI.getBoundingBox().width;
      int height    = resultI.getBoundingBox().height;
      int color_num = ++i;

      if(ov.paintRectangel && !resultI.getBoundingBox().empty()) {
        rectangle(mask, resultI.getBoundingBox(), RED, 1 * THICKNESS, cv::LINE_4);
      }
      if(!resultI.getMask().empty() && !resultI.getBoundingBox().empty() && resultI.getBoundingBox().x >= 0 &&
         resultI.getBoundingBox().y >= 0 && resultI.getBoundingBox().width >= 0 &&
         resultI.getBoundingBox().height >= 0 &&
         resultI.getBoundingBox().x + resultI.getBoundingBox().width <= mask.cols &&
         resultI.getBoundingBox().y + resultI.getBoundingBox().height <= mask.rows) {
        try {
          mask(resultI.getBoundingBox()).setTo(ov.backgroundColor, resultI.getMask());
          std::vector<std::vector<cv::Point>> contours;
          findContours(resultI.getMask(), contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
          drawContours(mask(resultI.getBoundingBox()), contours, -1, ov.borderColor, 1);

        } catch(const std::exception &ex) {
          std::cout << "PA: " << ex.what() << std::endl;
        }
      }
      std::string label = std::to_string(resultI.getIndex());
      // drawLabel(img, label, left, top);
    }
    try {
      addWeighted(mask, ov.opaque, img, 1, 0, img);
    } catch(const std::exception &ex) {
      std::cout << "PP: " << ex.what() << std::endl;
    }

    // PP: OpenCV(4.9.0) /opencv/modules/core/src/arithm.cpp:647: error: (-209:Sizes of input arguments do not match)
    // The operation is neither 'array op array' (where arrays have the same size and the same number of channels), nor
    // 'array op scalar', nor 'scalar op array' in function 'arithm_op'
  }
}

}    // namespace joda::image::detect
