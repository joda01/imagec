///
/// \file      roi.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-30
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "roi.hpp"
#include <qnamespace.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_units.hpp"
#include "backend/enums/types.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/ome_parser/physical_size.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace joda::atom {

ROI::ROI() :
    mIsNull(true), mObjectId(mGlobalUniqueObjectId++), mId({}), mConfidence(0), mMask(cv::Mat(0, 0, CV_8UC1)), mMaskContours({}),
    mImageSize(cv::Size{0, 0}), mOriginalImageSize(cv::Size{0, 0}), mAreaSize(0), mPerimeter(0), mCircularity(0), mCentroid(0, 0),
    mOriginObjectId(mObjectId)
{
  CV_Assert(mMask.type() == CV_8UC1);
}

ROI::ROI(RoiObjectId index, Confidence confidence, const Boxes &boundingBox, const cv::Mat &mask, const std::vector<cv::Point> &contour,
         const cv::Size &imageSize, const cv::Size &originalImageSize, const enums::tile_t &tile, const cv::Size &tileSize) :
    mIsNull(false),
    mObjectId(mGlobalUniqueObjectId++), mId(std::move(index)), mConfidence(confidence), mBoundingBoxTile(boundingBox),
    mBoundingBoxReal(calcRealBoundingBox(tile, tileSize)), mMask(mask), mMaskContours(contour), mImageSize(imageSize),
    mOriginalImageSize(originalImageSize), mAreaSize(static_cast<double>(calcAreaSize())), mPerimeter(getTracedPerimeter(mMaskContours)),
    mCircularity(calcCircularity()), mCentroid(calcCentroid(mMask)), mOriginObjectId(mObjectId)
{
  CV_Assert(mMask.type() == CV_8UC1);
}

///
/// \brief      Calculates a the bounding box in the overall image if it is a tiled image
/// \author     Joachim Danmayr
///
Boxes ROI::calcRealBoundingBox(const enums::tile_t &tile, const cv::Size &tileSize) const
{
  Boxes box;
  box.width  = mBoundingBoxTile.width;
  box.height = mBoundingBoxTile.width;
  box.x      = mBoundingBoxTile.x + std::get<0>(tile) * tileSize.width;
  box.y      = mBoundingBoxTile.y + std::get<1>(tile) * tileSize.height;

  return box;
}

///
/// \brief        Calculate area size
/// \author       Joachim Danmayr
///
uint64_t ROI::calcAreaSize() const
{
  return static_cast<uint64_t>(cv::countNonZero(mMask));
}

///
/// \brief        Calculate ROI metrics
/// \author       Joachim Danmayr
///
float ROI::calcCircularity() const
{
  double dividend       = 4.0 * M_PI * static_cast<double>(mAreaSize);
  double perimterSquare = static_cast<double>(mPerimeter) * static_cast<double>(mPerimeter);
  if(dividend < perimterSquare) {
    return static_cast<float>(dividend / perimterSquare);
  }
  return 1;
}

///
/// \brief        Calculate centroid
/// \author       Joachim Danmayr
///
auto ROI::calcCentroid(const cv::Mat &mask) -> cv::Point
{
  // Calculate moments
  cv::Moments moments = cv::moments(mask, true);
  auto cx             = static_cast<int32_t>(moments.m10 / moments.m00);
  auto cy             = static_cast<int32_t>(moments.m01 / moments.m00);
  return {cx, cy};
}

///
/// \brief        Returns true if the ROI is touching the image edge
/// \author       Joachim Danmayr
///
bool ROI::isTouchingTheImageEdge() const
{
  auto box       = getBoundingBoxTile();
  auto imageSize = mImageSize;
  if(box.x <= 0 || box.y <= 0 || box.x + box.width >= imageSize.width || box.y + box.height >= imageSize.height) {
    return true;    // Touches the edge
  }
  return false;
}

///
/// \brief        Calculate the avg, min and max intensity in the given image
/// \author       Joachim Danmayr
///
auto ROI::calcIntensity(const cv::Mat &image) const -> Intensity
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
  intensityRet.intensityAvg = static_cast<float>(cv::mean(maskImg, mMask)[0]);
  intensityRet.intensitySum = static_cast<uint64_t>(cv::sum(maskImg)[0]);
  cv::minMaxLoc(maskImg, &intensityRet.intensityMin, &intensityRet.intensityMax, nullptr, nullptr, mMask);
  return intensityRet;
}

///
/// \brief        Calculate the gradients in the given image
/// \author       Joachim Danmayr
///
auto ROI::calcGradients(const cv::Mat &image, cv::Mat &gradMag, cv::Mat &gradAngle) const -> void
{
  // \todo make more efficient
  cv::Mat maskImg = image(mBoundingBoxTile).clone();
  for(int x = 0; x < maskImg.cols; x++) {
    for(int y = 0; y < maskImg.rows; y++) {
      if(mMask.at<uint8_t>(y, x) <= 0) {
        maskImg.at<uint16_t>(y, x) = 0;
      }
    }
  }

  cv::Mat grad_x;
  cv::Mat grad_y;
  cv::Sobel(maskImg, grad_x, CV_32F, 1, 0, 3);
  cv::Sobel(maskImg, grad_y, CV_32F, 0, 1, 3);
  cv::cartToPolar(grad_x, grad_y, gradMag, gradAngle, true);
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
  for(size_t i = 1; i < points.size() - 2; i++) {
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
    return static_cast<float>(4.0 - 2.0 * (2.0 - sqrt(2)));
  }
  if(nPoints == 2) {
    return static_cast<float>(6 - 3 * (2 - sqrt(2)));
  }
  if(nPoints == 3) {
    return static_cast<float>(8 - 3 * (2 - sqrt(2)));
  }
  if(nPoints == 4) {
    return static_cast<float>(8 - 4 * (2 - sqrt(2)));
  }

  int sumdx    = 2;    // Starting with 2 is an approximation because ImageJ has an other conour algorhtm then opencv
  int sumdy    = 2;    // Starting with 2 is an approximation because ImageJ has an other conour algorhtm then opencv
  int nCorners = 0;
  int dx1      = points[0].x - points[nPoints - 1].x;
  int dy1      = points[0].y - points[nPoints - 1].y;
  int side1    = std::abs(dx1) + std::abs(dy1);    // one of these is 0
  bool corner  = false;
  // int nexti    = 0;
  int dx2   = 0;
  int dy2   = 0;
  int side2 = 0;
  for(size_t i = 0; i < nPoints; i++) {
    size_t nexti = i + 1;
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

  return static_cast<float>(static_cast<double>(sumdx) + static_cast<double>(sumdy) - (static_cast<double>(nCorners) * (2.0 - std::sqrt(2))));
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
  for(size_t i = 0; i < npoints - 1; i++) {
    length += std::sqrt(std::pow((points[i + 1].x - points[i].x) * pixelWidth, 2) + std::pow((points[i + 1].y - points[i].y) * pixelHeight, 2));
  }
  if(closeShape) {
    length += std::sqrt(pow((points[0].x - points[npoints - 1].x) * pixelWidth, 2) + pow((points[0].y - points[npoints - 1].y) * pixelHeight, 2));
  }
  return length;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] double ROI::getAreaSize(const ome::PhyiscalSize &physicalSize, enums::Units unit) const
{
  auto [pxSizeX, pxSizeY, pxSizeZ] = physicalSize.getPixelSize(unit);

  return mAreaSize * pxSizeX * pxSizeY;
}

///
/// \brief
/// \todo Support pixels which are not a square
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] float ROI::getPerimeter(const ome::PhyiscalSize &physicalSize, enums::Units unit) const
{
  auto [pxSizeX, pxSizeY, pxSizeZ] = physicalSize.getPixelSize(unit);
  if(pxSizeX != pxSizeY) {
    // joda::log::logWarning("Perimeter to real value with rectangle pixels not supported right now!");
    pxSizeX = std::max(pxSizeX, pxSizeY);
  }
  return static_cast<float>(static_cast<double>(ROI::mPerimeter) * pxSizeX);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] auto ROI::getDistances(const ome::PhyiscalSize &physicalSize, enums::Units unit) const -> std::map<uint64_t, Distance>
{
  if(enums::Units::Pixels == unit) {
    return mDistances;
  }

  auto [pxSizeX, pxSizeY, pxSizeZ] = physicalSize.getPixelSize(unit);
  if(pxSizeX != pxSizeY) {
    // joda::log::logWarning("Get distance with rectangle pixels not supported right now!");
    pxSizeX = std::max(pxSizeX, pxSizeY);
  }
  std::map<uint64_t, Distance> realDistance;

  for(const auto &[id, dis] : mDistances) {
    realDistance.emplace(id, Distance{.distanceCentroidToCentroid   = dis.distanceCentroidToCentroid * pxSizeX,
                                      .distanceCentroidToSurfaceMin = dis.distanceCentroidToSurfaceMin * pxSizeX,
                                      .distanceCentroidToSurfaceMax = dis.distanceCentroidToSurfaceMax * pxSizeX,
                                      .distanceSurfaceToSurfaceMin  = dis.distanceSurfaceToSurfaceMin * pxSizeX,
                                      .distanceSurfaceToSurfaceMax  = dis.distanceSurfaceToSurfaceMax * pxSizeX});
  }
  return realDistance;
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
                                        const cv::Size &tileSize, joda::enums::ClassId objectClassIntersectingObjectsShouldBeAssignedTo) const
{
  auto intersectingMask = calcIntersectingMask(roi);

  if(intersectingMask.nrOfIntersectingPixels > 0) {
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Point> contour = {};
    cv::findContours(intersectingMask.intersectedMask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
    if(!contours.empty()) {
      size_t contourSize = contours[0].size();
      contour            = contours[0];
      for(const auto &cont : contours) {
        if(cont.size() > contourSize) {
          contourSize = cont.size();
          contour     = cont;
        }
      }
    }
    if(intersectingMask.intersectionArea >= minIntersection) {
      return ROI{atom::ROI::RoiObjectId{
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
  result.intersectedRect = getBoundingBoxTile() & roi.getBoundingBoxTile();

  if(result.intersectedRect.area() <= 0) {
    return {};
  }
  result.intersectedMask = cv::Mat::zeros(result.intersectedRect.height, result.intersectedRect.width, CV_8UC1);

  const int32_t xM1Base = getBoundingBoxTile().x;
  const int32_t yM1Base = getBoundingBoxTile().y;
  const int32_t xM2Base = roi.getBoundingBoxTile().x;
  const int32_t yM2Base = roi.getBoundingBoxTile().y;

  // Iterate through the pixels in the intersection and set them in the new mask
  for(int y = result.intersectedRect.y; y < result.intersectedRect.y + result.intersectedRect.height; ++y) {
    for(int x = result.intersectedRect.x; x < result.intersectedRect.x + result.intersectedRect.width; ++x) {
      int xM1 = x - xM1Base;
      int yM1 = y - yM1Base;

      bool mask1On = false;
      auto maskTmp = getMask();
      if(xM1 >= 0 && yM1 >= 0 && xM1 < maskTmp.cols && yM1 < maskTmp.rows) {
        mask1On = getMask().at<uchar>(yM1, xM1) > 0;
      }

      int xM2           = x - xM2Base;
      int yM2           = y - yM2Base;
      bool mask2On      = false;
      const auto &maskB = roi.getMask();
      if(xM2 >= 0 && yM2 >= 0 && yM2 < maskB.rows && xM2 < maskB.cols) {
        mask2On = maskB.at<uchar>(yM2, xM2) > 0;
      }

      if(mask1On && mask2On) {
        result.intersectedMask.at<uchar>(y - result.intersectedRect.y, x - result.intersectedRect.x) = 255;
        result.nrOfIntersectingPixels++;
      }
    }
  }

  double smallestArea =
      std::min(getAreaSize(ome::PhyiscalSize::Pixels(), enums::Units::Pixels), roi.getAreaSize(ome::PhyiscalSize::Pixels(), enums::Units::Pixels));
  if(smallestArea > 0) {
    result.intersectionArea = static_cast<float>(static_cast<double>(result.nrOfIntersectingPixels) / static_cast<double>(smallestArea));
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
auto ROI::measureIntensityAndAdd(const enums::ImageId &imageId, const cv::Mat &image) -> Intensity
{
  if(!mIntensity.contains(imageId)) {
    // Just add an empty entry
    mIntensity[imageId].intensitySum = 0;
    mIntensity[imageId].intensityAvg = 0;
    mIntensity[imageId].intensityMax = 0;
    mIntensity[imageId].intensityMin = 0;

    if(!image.empty() && !mBoundingBoxTile.empty() && !mMask.empty()) {
      mIntensity[imageId] = calcIntensity(image);
    }
  } else {
  }
  return mIntensity[imageId];
}

///
/// \brief      Calculate the distance between the given object
/// \author     Joachim Danmayr
/// \param[in]  secondRoi  Object to calc the distance with
/// \return     Calculated distances
///
auto ROI::measureDistanceAndAdd(const ROI &secondRoi) -> Distance
{
  Distance distance;

  distance.distanceSurfaceToSurfaceMin  = std::numeric_limits<double>::max();
  distance.distanceSurfaceToSurfaceMax  = 0;
  distance.distanceCentroidToSurfaceMin = std::numeric_limits<double>::max();
  distance.distanceCentroidToSurfaceMax = 0;

  for(const auto &p2 : secondRoi.getContour()) {
    auto p2Tmp = p2;
    p2Tmp.x    = p2.x + secondRoi.getBoundingBoxReal().x;
    p2Tmp.y    = p2.y + secondRoi.getBoundingBoxReal().y;

    double distPointToSurface = cv::norm(cv::Point2f(getCentroidReal()) - cv::Point2f(p2Tmp));
    if(distPointToSurface < distance.distanceCentroidToSurfaceMin) {
      distance.distanceCentroidToSurfaceMin = distPointToSurface;
    }
    if(distPointToSurface > distance.distanceCentroidToSurfaceMax) {
      distance.distanceCentroidToSurfaceMax = distPointToSurface;
    }

    for(const auto &p1 : getContour()) {
      // Bring into the real scope
      auto p1Tmp = p1;
      p1Tmp.x    = p1.x + mBoundingBoxReal.x;
      p1Tmp.y    = p1.y + mBoundingBoxReal.y;

      double dist = cv::norm(p1Tmp - p2Tmp);
      if(dist < distance.distanceSurfaceToSurfaceMin) {
        distance.distanceSurfaceToSurfaceMin = dist;
      }
      if(dist > distance.distanceSurfaceToSurfaceMax) {
        distance.distanceSurfaceToSurfaceMax = dist;
      }
    }
  }

  distance.distanceCentroidToCentroid = cv::norm(getCentroidReal() - secondRoi.getCentroidReal());
  mDistances[secondRoi.getObjectId()] = distance;
  // distance.print();
  return distance;
}

cv::Mat shiftMask(const cv::Mat &mask, int offsetX, int offsetY)
{
  // Create an output mask filled with zeros (same type and size as input)
  cv::Mat shifted = cv::Mat::zeros(mask.size(), mask.type());

  // Determine source and destination ROIs based on offset
  int srcX   = std::max(0, -offsetX);
  int srcY   = std::max(0, -offsetY);
  int dstX   = std::max(0, offsetX);
  int dstY   = std::max(0, offsetY);
  int width  = mask.cols - std::abs(offsetX);
  int height = mask.rows - std::abs(offsetY);

  // Ensure width and height are valid
  if(width > 0 && height > 0) {
    // Define source and destination ROIs
    cv::Rect srcRoi(srcX, srcY, width, height);
    cv::Rect dstRoi(dstX, dstY, width, height);
    // Copy ROI from source to destination
    mask(srcRoi).copyTo(shifted(dstRoi));
  }

  return shifted;
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
  // Compute the new size
  cv::Size newSize(static_cast<int>(static_cast<float>(mMask.cols) * scaleX), static_cast<int>(static_cast<float>(mMask.rows) * scaleY));
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

  auto oldCentroid = mCentroid;
  auto newCentroid = calcCentroid(mMask);

  auto scaleBoundingBox = [&](Boxes &box, const cv::Size &imgSize) -> std::pair<int32_t, int32_t> {
    std::pair<int32_t, int32_t> centroidOffset = {0, 0};
    int32_t widthDif                           = newCentroid.x - oldCentroid.x;
    int32_t heightDif                          = newCentroid.y - oldCentroid.y;

    auto moveX = static_cast<int32_t>(std::ceil(static_cast<float>(widthDif) / 1.0F));
    auto moveY = static_cast<int32_t>(std::ceil(static_cast<float>(heightDif) / 1.0F));

    box.x = box.x - moveX;
    if(box.x < 0) {
      box.width            = box.width + box.x;
      centroidOffset.first = box.x;
      box.x                = 0;
    }
    if(box.x > imgSize.width) {
      box.x = imgSize.width;
    }
    box.y = box.y - moveY;
    if(box.y < 0) {
      box.height            = box.height + box.y;
      centroidOffset.second = box.y;
      box.y                 = 0;
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
    return centroidOffset;
  };
  auto centroidOffset = scaleBoundingBox(mBoundingBoxTile, mImageSize);
  scaleBoundingBox(mBoundingBoxReal, mOriginalImageSize);

  cv::Rect crop(0, 0, mBoundingBoxTile.width, mBoundingBoxTile.height);
  mMask = shiftMask(mMask, centroidOffset.first, centroidOffset.second);

  mMask = mMask(crop).clone();

  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(mMask, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

  if(!contours.empty()) {
    for(size_t i = 0; i < contours.size(); i++) {
      // Do not paint a contour for elements inside an element.
      // In other words if there is a particle with a hole, ignore the hole.
      // See https://docs.opencv.org/4.x/d9/d8b/tutorial_py_contours_hierarchy.html
      if(hierarchy[i][3] == -1) {
        mMaskContours = contours[i];
      }
    }
  }
  mAreaSize    = static_cast<double>(calcAreaSize());
  mPerimeter   = getTracedPerimeter(mMaskContours);
  mCircularity = calcCircularity();
  mCentroid    = calcCentroid(mMask);
}

///
/// \brief  Resizes the ROI based on the given scale factors
/// \author Joachim Danmayr
/// \todo   If there were still intensity measurements they are not valid any more for the new size what should happen?
///
void ROI::drawCircle(float radius)
{
  if(mIsNull || mBoundingBoxReal.width <= 0 || mBoundingBoxReal.height <= 0) {
    return;
  }
  // Compute the new size
  cv::Size newSize(static_cast<int>(radius * 2), static_cast<int>(radius * 2));
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

  auto oldCentroid = mCentroid;
  auto newCentroid = calcCentroid(mMask);

  auto scaleBoundingBox = [&](Boxes &box, const cv::Size &imgSize) -> std::pair<int32_t, int32_t> {
    std::pair<int32_t, int32_t> centroidOffset = {0, 0};
    int32_t widthDif                           = newCentroid.x - oldCentroid.x;
    int32_t heightDif                          = newCentroid.y - oldCentroid.y;

    auto moveX = static_cast<int32_t>(std::ceil(static_cast<float>(widthDif) / 1.0F));
    auto moveY = static_cast<int32_t>(std::ceil(static_cast<float>(heightDif) / 1.0F));

    box.x = box.x - moveX;
    if(box.x < 0) {
      box.width            = box.width + box.x;
      centroidOffset.first = box.x;
      box.x                = 0;
    }
    if(box.x > imgSize.width) {
      box.x = imgSize.width;
    }
    box.y = box.y - moveY;
    if(box.y < 0) {
      box.height            = box.height + box.y;
      centroidOffset.second = box.y;
      box.y                 = 0;
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
    return centroidOffset;
  };
  auto centroidOffset = scaleBoundingBox(mBoundingBoxTile, mImageSize);
  scaleBoundingBox(mBoundingBoxReal, mOriginalImageSize);

  // Circle parameters
  mMask = 0;
  cv::Point center(newCentroid.x + centroidOffset.first, newCentroid.y + centroidOffset.second);    // Center of the circle
  cv::circle(mMask, center, static_cast<int>(radius), cv::Scalar{255}, -1);

  // Crop
  cv::Rect crop(0, 0, mBoundingBoxTile.width, mBoundingBoxTile.height);
  mMask = mMask(crop).clone();

  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(mMask, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

  if(!contours.empty()) {
    for(size_t i = 0; i < contours.size(); i++) {
      // Do not paint a contour for elements inside an element.
      // In other words if there is a particle with a hole, ignore the hole.
      // See https://docs.opencv.org/4.x/d9/d8b/tutorial_py_contours_hierarchy.html
      if(hierarchy[i][3] == -1) {
        mMaskContours = contours[i];
      }
    }
  }
  mAreaSize    = static_cast<double>(calcAreaSize());
  mPerimeter   = getTracedPerimeter(mMaskContours);
  mCircularity = calcCircularity();
  mCentroid    = calcCentroid(mMask);
}

///
/// \brief  Resizes the ROI based on the given scale factors
/// \author Joachim Danmayr
/// \todo   If there were still intensity measurements they are not valid any more for the new size what should happen?
///
void ROI::fitEllipse()
{
  if(mIsNull || mBoundingBoxReal.width <= 0 || mBoundingBoxReal.height <= 0) {
    return;
  }

  cv::RotatedRect ellipseBox;
  if(mMaskContours.size() >= 5) {    // fitEllipse needs at least 5 points
    ellipseBox = cv::fitEllipse(mMaskContours);
  } else {
    auto radius = static_cast<float>(std::max(getBoundingBoxTile().width, getBoundingBoxTile().height));
    drawCircle(radius);
    return;
  }

  // Compute the new size
  cv::Size newSize(static_cast<int>(ellipseBox.boundingRect().width), static_cast<int>(ellipseBox.boundingRect().height));
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

  auto oldCentroid = mCentroid;
  auto newCentroid = ellipseBox.center;    // calcCentroid(mMask);

  auto scaleBoundingBox = [&](Boxes &box, const cv::Size &imgSize) -> std::pair<int32_t, int32_t> {
    std::pair<int32_t, int32_t> centroidOffset = {0, 0};
    auto widthDif                              = static_cast<int32_t>(newCentroid.x - static_cast<float>(oldCentroid.x));
    auto heightDif                             = static_cast<int32_t>(newCentroid.y - static_cast<float>(oldCentroid.y));

    auto moveX = static_cast<int32_t>(std::ceil(static_cast<double>(widthDif) / 1.0));
    auto moveY = static_cast<int32_t>(std::ceil(static_cast<double>(heightDif) / 1.0));

    box.x = box.x - moveX;
    if(box.x < 0) {
      box.width            = box.width + box.x;
      centroidOffset.first = box.x;
      box.x                = 0;
    }
    if(box.x > imgSize.width) {
      box.x = imgSize.width;
    }
    box.y = box.y - moveY;
    if(box.y < 0) {
      box.height            = box.height + box.y;
      centroidOffset.second = box.y;
      box.y                 = 0;
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
    return centroidOffset;
  };
  // auto centroidOffset = scaleBoundingBox(mBoundingBoxTile, mImageSize);
  scaleBoundingBox(mBoundingBoxReal, mOriginalImageSize);

  // Circle parameters
  mMask = 0;
  cv::ellipse(mMask, ellipseBox, cv::Scalar{255}, -1);

  // Crop
  cv::Rect crop(0, 0, mBoundingBoxTile.width, mBoundingBoxTile.height);
  mMask = mMask(crop).clone();

  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(mMask, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

  if(!contours.empty()) {
    for(size_t i = 0; i < contours.size(); i++) {
      // Do not paint a contour for elements inside an element.
      // In other words if there is a particle with a hole, ignore the hole.
      // See https://docs.opencv.org/4.x/d9/d8b/tutorial_py_contours_hierarchy.html
      if(hierarchy[i][3] == -1) {
        mMaskContours = contours[i];
      }
    }
  }
  mAreaSize    = static_cast<double>(calcAreaSize());
  mPerimeter   = getTracedPerimeter(mMaskContours);
  mCircularity = calcCircularity();
  mCentroid    = calcCentroid(mMask);
}

///
/// \brief      Assigns the linked object id of this ROI to all ROIS in the mLinkedWith list.
///             If the linked object ID is zero a new one is generated.
///             If a linked id is given as parameter this ID is used for the linked objects.
///             The own linked object ID is not changed. Use the setLinkedObjectId to change the own
/// \author     Joachim Danmayr
/// \return
///
void ROI::assignTrackingIdToAllLinkedRois(uint64_t trackingIdForLinked)
{
  uint64_t trackingId = mTrackingId;
  if(trackingIdForLinked != 0) {
    trackingId = trackingIdForLinked;
  }
  if(trackingId == 0) {
    static std::mutex assignMutex;
    std::lock_guard<std::mutex> lock(assignMutex);
    trackingId = generateNewTrackingId();
    setTrackingId(trackingId);
  }

  for(auto *roi : mLinkedWith) {
    roi->setTrackingId(trackingId);
  }
}

}    // namespace joda::atom
