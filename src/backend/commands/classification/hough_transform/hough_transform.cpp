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

#include "hough_transform.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include "hough_transform_settings.hpp"

namespace joda::cmd {

HoughTransform::HoughTransform(const settings::HoughTransformSettings &settings) : mSettings(settings)
{
}

void HoughTransform::execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result)
{
  cv::Mat binaryImage(image.size(), CV_8UC1);
  image.convertTo(binaryImage, CV_8UC1, 255.0 / 65535.0);

  auto circleToRoi = [&](const cv::Point &center, const int32_t &radius) {
    if(radius <= 0) {
      return;
    }
    cv::Mat mask = cv::Mat::zeros(cv::Size(radius * 2, radius * 2), CV_8UC1);
    cv::circle(mask, cv::Point(radius, radius), radius, cv::Scalar(255), cv::FILLED);

    // Find contours in the binary image
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(mask, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
    if(contours.empty()) {
      return;
    }
    int width  = radius * 2;
    int height = radius * 2;
    int x      = center.x - radius;
    if(x < 0) {
      x = 0;
      return;
    }
    int y = center.y - radius;
    if(y < 0) {
      y = 0;
      return;
    }

    if(x + width > image.cols) {
      auto delta = (x + width) - image.cols;
      width      = width - delta;
      return;
    }

    if(y + height > image.rows) {
      auto delta = (y + height) - image.rows;
      height     = height - delta;
      return;
    }

    cv::Rect boundingBox(x, y, width, height);

    joda::atom::ROI detectedRoi(atom::ROI::RoiObjectId{.classId = context.getClassId(mSettings.outputClass), .imagePlane = context.getActIterator()},
                                context.getAppliedMinThreshold(), boundingBox, mask, contours[0], context.getImageSize(),
                                context.getOriginalImageSize(), context.getActTile(), context.getTileSize());
    result.push_back(detectedRoi);
  };

  /////////////////////

  // Standard Hough Line Transform
  /* if(false) {
     std::vector<cv::Vec2f> lines;                                     // will hold the results of the detection
     cv::HoughLines(binaryImage, lines, 1, CV_PI / 180, 150, 0, 0);    // runs the actual detection
     // Draw the lines
     binaryImage = cv::Mat::zeros(binaryImage.size(), CV_8UC1);
     for(size_t i = 0; i < lines.size(); i++) {
       float rho   = lines[i][0];
       float theta = lines[i][1];
       cv::Point pt1;
       cv::Point pt2;
       double a  = cos(theta);
       double b  = sin(theta);
       double x0 = a * rho;
       double y0 = b * rho;
       pt1.x     = cvRound(x0 + 1000 * (-b));
       pt1.y     = cvRound(y0 + 1000 * (a));
       pt2.x     = cvRound(x0 - 1000 * (-b));
       pt2.y     = cvRound(y0 - 1000 * (a));
       cv::line(binaryImage, pt1, pt2, cv::Scalar(255), 2, cv::LINE_AA);
     }
   }*/
  {
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(binaryImage, circles, cv::HOUGH_GRADIENT, 1.5, mSettings.circleProperties.minCircleDistance, mSettings.circleProperties.param01,
                     mSettings.circleProperties.param02, mSettings.circleProperties.minCircleRadius, mSettings.circleProperties.maxCircleRadius);
    binaryImage = cv::Mat::zeros(binaryImage.size(), CV_8UC1);

    for(size_t i = 0; i < circles.size(); i++) {
      cv::Vec3i c = circles[i];
      cv::Point center(c[0], c[1]);
      int radius = c[2];
      // Draw the circle center
      cv::circle(binaryImage, center, 3, cv::Scalar(255), -1);
      // Draw the circle outline
      cv::circle(binaryImage, center, radius, cv::Scalar(255), 1);
      circleToRoi(center, radius);
    }
  }
  ///////////////////

  // binaryImage.convertTo(image, CV_16UC1, 257.0);
}

}    // namespace joda::cmd
