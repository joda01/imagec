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
///

#include "roi.hpp"
#include <qnamespace.h>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/types.hpp"
#include "backend/global_enums.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace joda::atom {

ROI::ROI() :
    mIsNull(true), mObjectId(mGlobalUniqueObjectId++), mId({}), confidence(0), mBoundingBoxTile({}), mBoundingBoxReal({}),
    mMask(cv::Mat(0, 0, CV_16UC1)), mMaskContours({}), mImageSize(cv::Size{0, 0}), mOriginalImageSize(cv::Size{0, 0}), mAreaSize(0), mPerimeter(0),
    mCircularity(0), mOriginObjectId(mObjectId)
{
}

ROI::ROI(RoiObjectId index, Confidence confidence, const Boxes &boundingBox, const cv::Mat &mask, const std::vector<cv::Point> &contour,
         const cv::Size &imageSize, const cv::Size &originalImageSize, const enums::tile_t &tile, const cv::Size &tileSize) :
    mIsNull(false),
    mObjectId(mGlobalUniqueObjectId++), mId(index), confidence(confidence), mBoundingBoxTile(boundingBox),
    mBoundingBoxReal(calcRealBoundingBox(tile, tileSize)), mMask(mask), mMaskContours(contour), mImageSize(imageSize),
    mOriginalImageSize(originalImageSize), mAreaSize(calcAreaSize()), mPerimeter(getTracedPerimeter(mMaskContours)), mCircularity(calcCircularity()),
    mOriginObjectId(mObjectId)
{
}

///
/// \brief      Calculates a the bounding box in the overall image if it is a tiled image
/// \author     Joachim Danmayr
///
Boxes ROI::calcRealBoundingBox(const enums::tile_t &tile, const cv::Size &tileSize)
{
  Boxes box;
  box.width  = mBoundingBoxTile.width;
  box.height = mBoundingBoxTile.width;
  box.x      = mBoundingBoxTile.x + std::get<0>(tile) * tileSize.width;
  box.y      = mBoundingBoxTile.y + std::get<1>(tile) * tileSize.height;

  return box;
}

std::tuple<int32_t, int32_t, int32_t, int32_t, int32_t> ROI::calcCircleRadius(int32_t snapAreaSize) const
{
  int32_t x = mBoundingBoxTile.x + (mBoundingBoxTile.width - snapAreaSize) / 2.0F;

  int32_t boundingBoxWith   = snapAreaSize;
  int32_t boundingBoxHeight = snapAreaSize;

  auto circleRadius = static_cast<int32_t>(static_cast<float>(snapAreaSize) / 2.0F);
  auto circleX      = circleRadius;
  auto circleY      = circleRadius;

  if(x < 0) {
    boundingBoxWith += x;
    circleX += x;
    x = 0;
  }
  int32_t y = mBoundingBoxTile.y + (mBoundingBoxTile.height - snapAreaSize) / 2.0F;
  if(y < 0) {
    boundingBoxHeight += y;
    circleY += y;
    y = 0;
  }
  return {x, y, circleY, circleY, circleRadius};
}

///
/// \brief        Calculate area size
/// \author       Joachim Danmayr
///
uint64_t ROI::calcAreaSize() const
{
  return cv::countNonZero(mMask);
}

///
/// \brief        Calculate ROI metrics
/// \author       Joachim Danmayr
///
float ROI::calcCircularity() const
{
  float dividend       = 4.0F * M_PI * static_cast<float>(mAreaSize);
  float perimterSquare = static_cast<float>(mPerimeter) * static_cast<float>(mPerimeter);
  if(dividend < perimterSquare) {
    return dividend / perimterSquare;
  }
  return 1;
}

///
/// \brief        Calculate the avg, min and max intensity in the given image
/// \author       Joachim Danmayr
///
auto ROI::calcIntensity(const cv::Mat &image) -> Intensity
{
  // \todo make more efficient
  Intensity intensityRet;
  cv::Mat maskImg = image(mBoundingBoxTile).clone();
  for(int x = 0; x < maskImg.cols; x++) {
    for(int y = 0; y < maskImg.rows; y++) {
      if(mMask.at<uint8_t>(y, x) <= 0) {
        maskImg.at<uint16_t>(y, x) = 0;
      }
    }
  }
  intensityRet.intensityAvg = cv::mean(maskImg, mMask)[0];
  intensityRet.intensitySum = cv::sum(maskImg)[0];
  cv::minMaxLoc(maskImg, &intensityRet.intensityMin, &intensityRet.intensityMax, nullptr, nullptr, mMask);
  return intensityRet;
}

///
/// \brief        Get smoothing
/// \author       Joachim Danmayr
///
double ROI::getSmoothedLineLength(const std::vector<cv::Point> &points)
{
  double length  = 0.0;
  double w2      = 1.0;
  double h2      = 1.0;
  size_t nPoints = points.size();
  double dx      = (points[0].x + points[1].x + points[2].x) / 3.0 - points[0].x;
  double dy      = (points[0].y + points[1].y + points[2].y) / 3.0 - points[0].y;
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

///
/// Returns the perimeter length of ROIs created using the wand tool
/// and the particle analyzer. The algorithm counts pixels in straight edges
/// as 1 and pixels in corners as sqrt(2).
/// It does this by calculating the total length of the ROI boundary and subtracting
/// 2-sqrt(2) for each non-adjacent corner. For example, a 1x1 pixel
/// ROI has a boundary length of 4 and 2 non-adjacent edges so the
/// perimeter is 4-2*(2-sqrt(2)). A 2x2 pixel ROI has a boundary length
/// of 8 and 4 non-adjacent edges so the perimeter is 8-4*(2-sqrt(2)).
/// Note that this code can currently create inconsistent legths depending on
/// the starting position.
///
float ROI::getTracedPerimeter(const std::vector<cv::Point> &points)
{
  if(points.empty()) {
    return 0.0;
  }
  size_t nPoints = points.size();

  if(nPoints == 1) {
    return 4 - 2 * (2 - sqrt(2));
  }
  if(nPoints == 2) {
    return 6 - 3 * (2 - sqrt(2));
  }
  if(nPoints == 3) {
    return 8 - 3 * (2 - sqrt(2));
  }
  if(nPoints == 4) {
    return 8 - 4 * (2 - sqrt(2));
  }

  int sumdx    = 2;    // Starting with 2 is an approximation because ImageJ has an other conour algorhtm then opencv
  int sumdy    = 2;    // Starting with 2 is an approximation because ImageJ has an other conour algorhtm then opencv
  int nCorners = 0;
  int dx1      = points[0].x - points[nPoints - 1].x;
  int dy1      = points[0].y - points[nPoints - 1].y;
  int side1    = std::abs(dx1) + std::abs(dy1);    // one of these is 0
  bool corner  = false;
  int nexti    = 0;
  int dx2      = 0;
  int dy2      = 0;
  int side2    = 0;
  for(int i = 0; i < nPoints; i++) {
    int nexti = i + 1;
    if(nexti == nPoints) {
      nexti = 0;
    }
    dx2 = points[nexti].x - points[i].x;
    dy2 = points[nexti].y - points[i].y;
    sumdx += std::abs(dx1);
    sumdy += std::abs(dy1);
    side2 = std::abs(dx2) + std::abs(dy2);
    if(side1 > 1 || !corner) {
      corner = true;
      nCorners++;
    } else {
      corner = false;
    }
    dx1   = dx2;
    dy1   = dy2;
    side1 = side2;
  }

  return static_cast<float>(static_cast<float>(sumdx) + static_cast<float>(sumdy) - (static_cast<float>(nCorners) * (2.0F - std::sqrt(2))));
}

///
/// Returns the length of a polygon with integer coordinates. Uses no calibration if imp is null.
///
double ROI::getLength(const std::vector<cv::Point> &points, bool closeShape)
{
  size_t npoints = points.size();
  if(npoints < 2) {
    return 0;
  }
  double pixelWidth  = 1.0;
  double pixelHeight = 1.0;
  double length      = 0;
  for(int i = 0; i < npoints - 1; i++) {
    length += std::sqrt(std::pow((points[i + 1].x - points[i].x) * pixelWidth, 2) + std::pow((points[i + 1].y - points[i].y) * pixelHeight, 2));
  }
  if(closeShape) {
    length += std::sqrt(pow((points[0].x - points[npoints - 1].x) * pixelWidth, 2) + pow((points[0].y - points[npoints - 1].y) * pixelHeight, 2));
  }
  return length;
}

///
/// \brief      Calculates if an intersection between the ROIs exist without any measurement
/// \author     Joachim Danmayr
/// \param[in]  roi   ROI to check against
/// \return     Intersection of the areas in percent
///
[[nodiscard]] bool ROI::isIntersecting(const ROI &roi, float minIntersection) const
{
  auto intersecting = calcIntersectingMask(roi);
  return intersecting.intersectionArea >= minIntersection;
}

///
/// \brief      Calculates if an intersection between the ROIs exist
/// \author     Joachim Danmayr
/// \param[in]  roi   ROI to check against
/// \return     Intersection of the areas in percent
///
[[nodiscard]] ROI ROI::calcIntersection(const enums::PlaneId &iterator, const ROI &roi, float minIntersection, const enums::tile_t &tile,
                                        const cv::Size &tileSize, joda::enums::ClusterId objectClusterIntersectingObjectsShouldBeAssignedTo,
                                        joda::enums::ClassId objectClassIntersectingObjectsShouldBeAssignedTo) const
{
  auto intersectingMask = calcIntersectingMask(roi);

  if(intersectingMask.nrOfIntersectingPixels > 0) {
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Point> contour = {};
    cv::findContours(intersectingMask.intersectedMask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
    if(!contours.empty()) {
      int32_t contourSize = contours[0].size();
      contour             = contours[0];
      for(const auto &cont : contours) {
        if(cont.size() > contourSize) {
          contourSize = cont.size();
          contour     = cont;
        }
      }
    }
    if(intersectingMask.intersectionArea >= minIntersection) {
      return ROI{atom::ROI::RoiObjectId{
                     .clusterId  = objectClusterIntersectingObjectsShouldBeAssignedTo,
                     .classId    = objectClassIntersectingObjectsShouldBeAssignedTo,
                     .imagePlane = iterator,
                 },
                 intersectingMask.intersectionArea,
                 intersectingMask.intersectedRect,
                 intersectingMask.intersectedMask,
                 contour,
                 mImageSize,
                 mOriginalImageSize,
                 tile,
                 tileSize};
    }
  }
  return {};
}

///
/// \brief      Calculates the intersection mask of this and the input object
/// \author     Joachim Danmayr
/// \param[in]  roi ROI to calculate the intersection with
/// \return     Intersecting mask
///
ROI::IntersectingMask ROI::calcIntersectingMask(const ROI &roi) const
{
  IntersectingMask result;
  result.intersectedRect = getBoundingBox() & roi.getBoundingBox();

  if(result.intersectedRect.area() <= 0) {
    return {};
  }
  result.intersectedMask = cv::Mat::zeros(result.intersectedRect.height, result.intersectedRect.width, CV_8UC1);

  const int32_t xM1Base = (result.intersectedRect.x - getBoundingBox().x);
  const int32_t yM1Base = (result.intersectedRect.y - getBoundingBox().y);
  const int32_t xM2Base = (result.intersectedRect.x - roi.getBoundingBox().x);
  const int32_t yM2Base = (result.intersectedRect.y - roi.getBoundingBox().y);

  // Iterate through the pixels in the intersection and set them in the new mask
  for(int y = 0; y < result.intersectedRect.height; ++y) {
    for(int x = 0; x < result.intersectedRect.width; ++x) {
      int xM1 = x + xM1Base;
      int yM1 = y + yM1Base;

      bool mask1On = false;
      if(xM1 >= 0 && yM1 >= 0) {
        mask1On = getMask().at<uchar>(yM1, xM1) > 0;
      }

      int xM2      = x + xM2Base;
      int yM2      = y + yM2Base;
      bool mask2On = false;
      if(xM2 >= 0 && yM2 >= 0) {
        mask2On = roi.getMask().at<uchar>(yM2, xM2) > 0;
      }

      if(mask1On) {
        result.nrOfPixelsMask1++;
      }

      if(mask2On) {
        result.nrOfPixelsMask2++;
      }

      if(mask1On && mask2On) {
        result.intersectedMask.at<uchar>(y, x) = 255;
        result.nrOfIntersectingPixels++;
      }
    }
  }

  int smallestMask = std::min(result.nrOfPixelsMask1, result.nrOfPixelsMask2);
  if(smallestMask > 0) {
    result.intersectionArea = static_cast<float>(result.nrOfIntersectingPixels) / static_cast<float>(smallestMask);
  }

  return result;
}

///
/// \brief      Measures the intensity in the given original image at the ROI
///             and adds the result to the ROI intensity map for this image
/// \author     Joachim Danmayr
/// \param[in]  channelIdx   Channel index of the given image
/// \param[in]  imageOriginal   Image to measure the intensity in
///
auto ROI::measureIntensityAndAdd(const joda::atom::ImagePlane &image) -> Intensity
{
  if(!intensity.contains(image.getId())) {
    // Just add an empty entry
    intensity[image.getId()].intensitySum = 0;
    intensity[image.getId()].intensityAvg = 0;
    intensity[image.getId()].intensityMax = 0;
    intensity[image.getId()].intensityMin = 0;

    if(!image.image.empty() && !mBoundingBoxTile.empty() && !mMask.empty()) {
      intensity[image.getId()] = calcIntensity(image.image);
    }
  } else {
  }
  return intensity[image.getId()];
}

///
/// \brief  Resizes the ROI based on the given scale factors
/// \author Joachim Danmayr
/// \todo   If there were still intensity measurements they are not valid any more for the new size what should happen?
///
void ROI::resize(float scaleX, float scaleY)
{
  if(mIsNull || mBoundingBoxReal.width <= 0 || mBoundingBoxReal.height <= 0) {
    return;
  }
  std::cout << "-----" << std::endl;

  std::cout << "Old width: " << std::to_string(mBoundingBoxReal.width) << " | " << std::to_string(mMask.cols) << std::endl;
  std::cout << "Old width: " << std::to_string(mBoundingBoxTile.width) << " | " << std::to_string(mMask.cols) << std::endl;
  std::cout << "width: " << std::to_string(mOriginalImageSize.width) << " | " << std::to_string(mImageSize.width) << std::endl;

  // Compute the new size
  cv::Size newSize(static_cast<int>(mMask.cols * scaleX), static_cast<int>(mMask.rows * scaleY));
  if(newSize.height <= 0) {
    newSize.height = 1;
  }
  if(newSize.width <= 0) {
    newSize.width = 1;
  }

  // Resize the image
  cv::Mat scaledImage = cv::Mat::zeros(newSize, CV_8UC1);
  cv::resize(mMask, scaledImage, newSize);
  mMask = scaledImage;

  auto scaleBoundingBox = [&](Boxes &box, const cv::Size &imgSize) {
    int32_t widthDif  = newSize.width - box.width;
    int32_t heightDif = newSize.height - box.height;

    int32_t moveX = std::ceil(static_cast<float>(widthDif) / 2.0);
    int32_t moveY = std::ceil(static_cast<float>(heightDif) / 2.0);

    std::cout << "Move x: " << std::to_string(moveX) << std::endl;

    box.x = box.x - moveX;
    if(box.x < 0) {
      box.x = 0;
    }
    if(box.x > imgSize.width) {
      box.x = imgSize.width;
    }
    box.y = box.y - moveY;
    if(box.y < 0) {
      box.y = 0;
    }
    if(box.y > imgSize.height) {
      box.y = imgSize.height;
    }

    box.width = newSize.width;
    if((box.x + box.width) > imgSize.width) {
      box.width = box.width - ((box.x + box.width) - imgSize.width);
    }

    box.height = newSize.height;
    if((box.y + box.height) > imgSize.height) {
      box.height = box.height - ((box.y + box.height) - imgSize.height);
    }
  };
  scaleBoundingBox(mBoundingBoxTile, mImageSize);
  scaleBoundingBox(mBoundingBoxReal, mOriginalImageSize);

  cv::Rect crop(0, 0, mBoundingBoxTile.width, mBoundingBoxTile.height);
  mMask = mMask(crop).clone();

  std::cout << "New width: " << std::to_string(mBoundingBoxReal.width) << " | " << std::to_string(mMask.cols) << std::endl;
  std::cout << "New width: " << std::to_string(mBoundingBoxTile.width) << " | " << std::to_string(mMask.cols) << std::endl;

  std::cout << "-----" << std::endl;

  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(mMask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
  if(!contours.empty()) {
    mMaskContours = contours[0];
  }
  mAreaSize    = static_cast<double>(calcAreaSize());
  mPerimeter   = getTracedPerimeter(mMaskContours);
  mCircularity = calcCircularity();
}

}    // namespace joda::atom
