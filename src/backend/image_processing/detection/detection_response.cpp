///
/// \file      detection_response.cpp
/// \author    Joachim Danmayr
/// \date      2024-03-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Functions on detection response
///

#include "detection_response.hpp"
#include <random>
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

namespace joda::image::detect {

static inline cv::Scalar BLACK   = cv::Scalar(0, 0, 0);
static inline cv::Scalar WHITE   = cv::Scalar(255, 255, 255);
static inline cv::Scalar YELLOW  = cv::Scalar(0, 255, 255);
static inline cv::Scalar RED     = cv::Scalar(0, 0, 255);
static inline cv::Scalar GREEN   = cv::Scalar(0, 255, 0);
static inline int THICKNESS      = 2;
static inline int FONT_THICKNESS = 2;

// Text parameters.
static inline float FONT_SCALE = 0.8;
static inline int FONT_FACE    = cv::FONT_HERSHEY_SIMPLEX;

void paintBoundingBox(cv::Mat &img, const std::unique_ptr<DetectionResults> &result, const std::string &fillColor,
                      bool paintRectangel, bool paintLabels);
cv::Scalar hexToScalar(const std::string &hexColor);
std::string generateRandomColorHex();

void DetectionResults::createBinaryImage(cv::Mat &img) const
{
  for(const auto &roi : *this) {
    if(roi.isValid()) {
      int left   = roi.getBoundingBox().x;
      int top    = roi.getBoundingBox().y;
      int width  = roi.getBoundingBox().width;
      int height = roi.getBoundingBox().height;

      if(!roi.getMask().empty() && !roi.getBoundingBox().empty() && roi.getBoundingBox().x >= 0 &&
         roi.getBoundingBox().y >= 0 && roi.getBoundingBox().width >= 0 && roi.getBoundingBox().height >= 0 &&
         roi.getBoundingBox().x + roi.getBoundingBox().width <= img.cols &&
         roi.getBoundingBox().y + roi.getBoundingBox().height <= img.rows) {
        try {
          img(roi.getBoundingBox()).setTo(cv::Scalar(255), roi.getMask());
        } catch(const std::exception &ex) {
          std::cout << "PA: " << ex.what() << std::endl;
        }
      }
    }
  }
}

///
/// \brief      Generate the control image
/// \author     Joachim Danmayr
///
cv::Mat DetectionResults::generateControlImage(const std::string &areaColor, const cv::Size &size)
{
  cv::Mat controlImageOut = cv::Mat::zeros(size, CV_8UC3);
  paintBoundingBox(controlImageOut, areaColor, false, false);
  return controlImageOut;
}

///
/// \brief      Paints the masks and bounding boxes around the found elements
/// \author     Joachim Danmayr
/// \param[in]  img    Image where the mask should be painted on
/// \param[in]  result Prediction result of the forward
///
void DetectionResults::paintBoundingBox(cv::Mat &img, const std::string &fillColor, bool paintRectangel,
                                        bool paintLabels)
{
  auto id              = DurationCount::start("Paint bounding box");
  cv::Scalar areaColor = hexToScalar("#" + generateRandomColorHex());

  int i = 0;
  for(const auto &roi : *this) {
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
        img(roi.getBoundingBox()).setTo(areaColor, roi.getMask());

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
    /*  if(paintLabels) {
        std::string label = std::to_string(roi.getIndex()) + " | " + std::to_string(roi.getConfidence());
        if(modelInfo.classes.size() > roi.getClassId()) {
          label += " | " + modelInfo.classes[roi.getClassId()];
        }

        drawLabel(img, areaColor, label, left, top);
      }*/
  }

  DurationCount::stop(id);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
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
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
cv::Scalar hexToScalar(const std::string &hexColor)
{
  int r, g, b;
  sscanf(hexColor.c_str(), "#%2x%2x%2x", &r, &g, &b);
  return cv::Scalar(b, g, r);    // OpenCV uses BGR order
}

///
/// \brief      Draw labels
/// \author     Joachim Danmayr
/// \param[in,out]  inputImage input image where bounding boxes should be painted on
/// \param[in]      label Text to print in the image
/// \param[in]      left position to print
/// \param[in]      top position to print
///
static void drawLabel(cv::Mat &inputImage, const cv::Scalar &foregroundColor, const std::string &label, int left,
                      int top)
{
  // Display the label at the top of the bounding box.
  int baseLine;
  cv::Size label_size = cv::getTextSize(label, FONT_FACE, FONT_SCALE, FONT_THICKNESS, &baseLine);
  top                 = cv::max(top, label_size.height);
  // Top left corner.
  cv::Point tlc = cv::Point(left, top);
  // Bottom right corner.
  cv::Point brc = cv::Point(left + label_size.width, top + label_size.height + baseLine);
  // Draw white rectangle.
  // rectangle(inputImage, tlc, brc, BLACK, cv::FILLED);
  // Put the label on the black rectangle.
  putText(inputImage, label, cv::Point(left, top + label_size.height), FONT_FACE, FONT_SCALE, foregroundColor,
          FONT_THICKNESS);
}

}    // namespace joda::image::detect
