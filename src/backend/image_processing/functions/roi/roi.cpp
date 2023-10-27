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
#include <opencv2/core/types.hpp>

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
  intensity    = 0;
  intensityMin = USHRT_MAX;
  intensityMax = 0;
  areaSize     = 0;
  circularity  = 0;
  perimeter    = 0;

  for(int y = 0; y < box.height; y++) {
    for(int x = 0; x < box.width; x++) {
      unsigned char maskPxl = boxMask.at<unsigned char>(y, x);
      if(maskPxl > 0) {
        int imgx = box.x + x;
        int imgy = box.y + y;

        if(imgx < imageOriginal.cols && imgy < imageOriginal.rows) {
          double pixelGrayScale = imageOriginal.at<unsigned short>(imgy, imgx);
          if(pixelGrayScale < intensityMin) {
            intensityMin = pixelGrayScale;
          }
          if(pixelGrayScale > intensityMax) {
            intensityMax = pixelGrayScale;
          }
          intensity += pixelGrayScale;

          // Check the 8-connected neighbors
          bool isBoundary = false;
          for(int dy = -1; dy <= 1; dy++) {
            for(int dx = -1; dx <= 1; dx++) {
              int yB = y + dy;
              int xB = x + dx;
              if(yB <= 0 || xB <= 0 || boxMask.at<uchar>(yB, xB) == 0) {
                perimeter++;
                isBoundary = true;
                break;
              }
            }
            if(isBoundary) {
              break;
            }
          }
          areaSize++;
        }
      }
    }
  }

  float intensityAvg = 0.0f;
  if(areaSize > 0) {
    intensityAvg = intensity / static_cast<float>(areaSize);
    // circularity  = static_cast<float>(pointsInCircle) / static_cast<float>(areaSize);
    float perimterSquare = static_cast<float>(perimeter) * static_cast<float>(perimeter);
    float dividend       = 4.0 * M_PI * static_cast<float>(areaSize);
    if(dividend < perimterSquare) {
      circularity = dividend / perimterSquare;
    } else {
      circularity = 1;
    }

  } else {
    intensityMin = 0;
  }
  intensity = intensityAvg;

  if(filter != nullptr) {
    applyParticleFilter(filter);
  } else {
    validity = ParticleValidity::VALID;
  }
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
