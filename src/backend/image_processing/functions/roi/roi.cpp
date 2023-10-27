///
/// \file      roi.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-30
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "roi.hpp"
#include <algorithm>
#include <iterator>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace joda::func {

ROI::ROI(uint32_t index, Confidence confidence, ClassId classId, const Boxes &boundingBox, const cv::Mat &mask) :
    index(index), confidence(confidence), classId(classId), box(boundingBox), boxMask(mask)
{
}
ROI::ROI(uint32_t index, Confidence confidence, ClassId classId, const Boxes &boundingBox, const cv::Mat &mask,
         const cv::Mat &imageOriginal, const joda::settings::json::ChannelFiltering *filter) :
    index(index),
    confidence(confidence), classId(classId), box(boundingBox), boxMask(mask)
{
  calculateMetrics(imageOriginal, filter);
}
ROI::ROI(uint32_t index, Confidence confidence, ClassId classId, const Boxes &boundingBox, const cv::Mat &mask,
         const cv::Mat &imageOriginal) :
    index(index),
    confidence(confidence), classId(classId), box(boundingBox), boxMask(mask)
{
  calculateMetrics(imageOriginal, nullptr);
}

///
/// \brief      Calculate metrics based on bounding box and mask
///
/// cv::Mat optimalCircle = cv::Mat::zeros(box.height, box.width, CV_8UC1);
/// double radius         = std::min(box.height, box.width) / 2;
/// circle(optimalCircle, cv::Point(box.height / 2, box.width / 2), radius, cv::Scalar(255, 255, 255), cv::FILLED,
///        cv::LINE_AA, 0);
/// int pointsInCircle = 0;
///  Calculate the intensity and area of the polygon ROI
/// unsigned char circleMask = optimalCircle.at<unsigned char>(y, x);    // Get the pixel value at (x, y)
/// if(circleMask > 0) {
///   pointsInCircle++;
/// }
///
/// \author     Joachim Danmayr
///
void ROI::calculateMetrics(const cv::Mat &imageOriginal, const joda::settings::json::ChannelFiltering *filter)
{
  if(!imageOriginal.empty() && !box.empty() && !boxMask.empty()) {
    cv::Mat maskImg = imageOriginal(box);
    areaSize        = cv::countNonZero(boxMask);
    intensity       = cv::mean(maskImg, boxMask)[0];
    cv::minMaxLoc(maskImg, &intensityMin, &intensityMax, nullptr, nullptr, boxMask);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(boxMask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

    perimeter = calcPerimeter(contours[0]);

    float dividend       = 4.0 * M_PI * static_cast<float>(areaSize);
    float perimterSquare = static_cast<float>(perimeter) * static_cast<float>(perimeter);
    if(dividend < perimterSquare) {
      circularity = dividend / perimterSquare;
    } else {
      circularity = 1;
    }
  }
  if(filter != nullptr) {
    applyParticleFilter(filter);
  } else {
    validity = ParticleValidity::VALID;
  }
}

double ROI::getSmoothedLineLength(const std::vector<cv::Point> &points) const
{
  double length = 0.0;
  double w2     = 1.0;
  double h2     = 1.0;
  double dx, dy;

  int nPoints = points.size();
  dx          = (points[0].x + points[1].x + points[2].x) / 3.0 - points[0].x;
  dy          = (points[0].y + points[1].y + points[2].y) / 3.0 - points[0].y;
  length += std::sqrt(dx * dx * w2 + dy * dy * h2);
  for(int i = 1; i < points.size() - 2; i++) {
    dx = (points[i + 2].x - points[i - 1].x) / 3.0;    // = (x[i]+x[i+1]+x[i+2])/3-(x[i-1]+x[i]+x[i+1])/3
    dy = (points[i + 2].y - points[i - 1].y) / 3.0;    // = (y[i]+y[i+1]+y[i+2])/3-(y[i-1]+y[i]+y[i+1])/3
    length += std::sqrt(dx * dx * w2 + dy * dy * h2);
  }
  dx = points[nPoints - 1].x - (points[nPoints - 3].x + points[nPoints - 2].x + points[nPoints - 1].x) / 3.0;
  dy = points[nPoints - 1].y - (points[nPoints - 3].y + points[nPoints - 2].y + points[nPoints - 1].y) / 3.0;
  length += std::sqrt(dx * dx * w2 + dy * dy * h2);
  return length;
}

/** Returns the perimeter length of ROIs created using the wand tool
 *  and the particle analyzer. The algorithm counts pixels in straight edges
 *  as 1 and pixels in corners as sqrt(2).
 *  It does this by calculating the total length of the ROI boundary and subtracting
 *  2-sqrt(2) for each non-adjacent corner. For example, a 1x1 pixel
 *  ROI has a boundary length of 4 and 2 non-adjacent edges so the
 *  perimeter is 4-2*(2-sqrt(2)). A 2x2 pixel ROI has a boundary length
 *  of 8 and 4 non-adjacent edges so the perimeter is 8-4*(2-sqrt(2)).
 *  Note that this code can currently create inconsistent legths depending on
 *  the starting position.
 */
double ROI::getTracedPerimeter(const std::vector<cv::Point> &points) const
{
  int nPoints = points.size();

  if(nPoints < 4)
    return 0;
  int sumdx    = 0;
  int sumdy    = 0;
  int nCorners = 0;
  int dx1      = points[0].x - points[nPoints - 1].x;
  int dy1      = points[0].y - points[nPoints - 1].y;
  int side1    = std::abs(dx1) + std::abs(dy1);    // one of these is 0
  bool corner  = false;
  int nexti, dx2, dy2, side2;
  for(int i = 0; i < nPoints; i++) {
    nexti = i + 1;
    if(nexti == nPoints)
      nexti = 0;
    dx2 = points[nexti].x - points[i].x;
    dy2 = points[nexti].y - points[i].y;
    sumdx += std::abs(dx1);
    sumdy += std::abs(dy1);
    side2 = std::abs(dx2) + std::abs(dy2);
    if(side1 > 1 || !corner) {
      corner = true;
      nCorners++;
    } else
      corner = false;
    dx1   = dx2;
    dy1   = dy2;
    side1 = side2;
  }
  double w = 1.0, h = 1.0;

  return sumdx * w + sumdy * h - (nCorners * ((w + h) - std::sqrt(w * w + h * h)));
}

/** Returns the length of a polygon with integer coordinates. Uses no calibration if imp is null. */
double ROI::getLength(const std::vector<cv::Point> &points, bool closeShape) const
{
  auto npoints = points.size();
  if(npoints < 2)
    return 0;
  double pixelWidth = 1.0, pixelHeight = 1.0;
  double length = 0;
  for(int i = 0; i < npoints - 1; i++)
    length += std::sqrt(std::pow((points[i + 1].x - points[i].x) * pixelWidth, 2) +
                        std::pow((points[i + 1].y - points[i].y) * pixelHeight, 2));
  if(closeShape)
    length += std::sqrt(pow((points[0].x - points[npoints - 1].x) * pixelWidth, 2) +
                        pow((points[0].y - points[npoints - 1].y) * pixelHeight, 2));
  return length;
}

double ROI::calcPerimeter(const std::vector<cv::Point> &points) const
{
  return getTracedPerimeter(points);

  /*double perimeter = 0;
  for(int y = 0; y < boxMask.rows; y++) {
    for(int x = 0; x < boxMask.cols; x++) {
      auto isBoundary = boxMask.at<uchar>(y - 1, x - 1) & boxMask.at<uchar>(y - 1, x + 0) &
                        boxMask.at<uchar>(y - 1, x + 1) & boxMask.at<uchar>(y + 0, x - 1) &
                        boxMask.at<uchar>(y + 0, x + 1) & boxMask.at<uchar>(y + 1, x - 1) &
                        boxMask.at<uchar>(y + 1, x + 0) & boxMask.at<uchar>(y + 1, x + 1);
      if(0 == isBoundary) {
        perimeter++;
      }
    }
  }
  return perimeter;*/
}

///
/// \brief     Applies particle filter and sets the validity
///            based on the detection results
/// \author    Joachim Danmayr
///
void ROI::applyParticleFilter(const joda::settings::json::ChannelFiltering *filter)
{
  validity = ParticleValidity::UNKNOWN;
  if(areaSize > filter->getMaxParticleSize()) {
    validity = static_cast<ParticleValidity>(static_cast<int>(validity) | static_cast<int>(ParticleValidity::TOO_BIG));
  }
  if(areaSize < filter->getMinParticleSize()) {
    validity =
        static_cast<ParticleValidity>(static_cast<int>(validity) | static_cast<int>(ParticleValidity::TOO_SMALL));
  }
  if(circularity < filter->getMinCircularity()) {
    validity = static_cast<ParticleValidity>(static_cast<int>(validity) |
                                             static_cast<int>(ParticleValidity::TOO_LESS_CIRCULARITY));
  }
  if(validity == ParticleValidity::UNKNOWN) {
    validity = ParticleValidity::VALID;
  }

  // filter.getSnapAreaSize();
}

///
/// \brief      Calculates if an intersection between the ROIs exist
/// \author     Joachim Danmayr
/// \param[in]  roi   ROI to check against
/// \return     Intersection of the areas in percent
///

[[nodiscard]] std::tuple<ROI, bool> ROI::calcIntersection(const ROI &roi, const cv::Mat &imageOriginal,
                                                          float minIntersection) const
{
  // Calculate the intersection of the bounding boxes
  cv::Rect intersectedRect = getBoundingBox() & roi.getBoundingBox();

  if(intersectedRect.area() > 0) {
    uint32_t nrOfIntersectingPixels = 0;
    uint32_t nrOfPixelsMask1        = 0;
    uint32_t nrOfPixelsMask2        = 0;
    cv::Mat intersectedMask         = cv::Mat::zeros(intersectedRect.height, intersectedRect.width, CV_8UC1);

    // Iterate through the pixels in the intersection and set them in the new mask
    for(int y = 0; y < intersectedRect.height; ++y) {
      for(int x = 0; x < intersectedRect.width; ++x) {
        int xM1      = x + (intersectedRect.x - getBoundingBox().x);
        int yM1      = y + (intersectedRect.y - getBoundingBox().y);
        bool mask1On = false;
        if(xM1 >= 0 && yM1 >= 0) {
          mask1On = getMask().at<uchar>(yM1, xM1) > 0;
        }

        int xM2      = x + (intersectedRect.x - roi.getBoundingBox().x);
        int yM2      = y + (intersectedRect.y - roi.getBoundingBox().y);
        bool mask2On = false;
        if(xM2 >= 0 && yM2 >= 0) {
          mask2On = roi.getMask().at<uchar>(yM2, xM2) > 0;
        }

        if(mask1On) {
          nrOfPixelsMask1++;
        }

        if(mask2On) {
          nrOfPixelsMask2++;
        }

        if(mask1On && mask2On) {
          intersectedMask.at<uchar>(y, x) = 255;
          nrOfIntersectingPixels++;
        }
      }
    }
    if(nrOfIntersectingPixels > 0) {
      int smallestMask       = std::min(nrOfPixelsMask1, nrOfPixelsMask2);
      float intersectionArea = 0;
      if(smallestMask > 0) {
        intersectionArea = static_cast<float>(nrOfIntersectingPixels) / static_cast<float>(smallestMask);
      }
      ROI intersectionROI(index, intersectionArea, 0, intersectedRect, intersectedMask, imageOriginal);
      if(intersectionArea < minIntersection) {
        intersectionROI.setValidity(ParticleValidity::TOO_LESS_OVERLAPPING);
      }
      return {intersectionROI, true};
    }
  }
  return {ROI(index, 0.0, 0, Boxes{}, cv::Mat{}, cv::Mat{}), false};
}

}    // namespace joda::func
