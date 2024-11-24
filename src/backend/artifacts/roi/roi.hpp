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

#pragma once

#include <atomic>
#include <bitset>
#include <string>
#include <vector>
#include "../image/image.hpp"
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/global_enums.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/opencv.hpp>

namespace joda::atom {

using Boxes      = cv::Rect;
using Confidence = float;
using ClassId    = joda::enums::ClassId;

///
/// \class      ROI
/// \author     Joachim Danmayr
/// \brief      Region of interest implementation
///
class ROI
{
public:
  struct RoiObjectId
  {
    joda::enums::ClassId classId;
    joda::enums::PlaneId imagePlane;
  };

  struct Intensity
  {
    uint64_t intensitySum = 0;    ///< Sum intensity of the masking area
    float intensityAvg    = 0;    ///< Avg intensity of the masking area
    double intensityMin   = 0;    ///< Min intensity of the masking area
    double intensityMax   = 0;    ///< Max intensity of the masking area
  };

  struct Intersecting
  {
    std::vector<RoiObjectId> roiValid;
  };

  struct IntersectingMask
  {
    uint32_t nrOfIntersectingPixels = 0;
    uint32_t nrOfPixelsMask1        = 0;
    uint32_t nrOfPixelsMask2        = 0;
    float intersectionArea          = 0;    // Interecting area in [0-1]
    cv::Rect intersectedRect;
    cv::Mat intersectedMask = {};
  };

  /////////////////////////////////////////////////////

  ROI();
  ROI(const ROI &) = delete;
  ROI(RoiObjectId index, Confidence confidence, const Boxes &boundingBox, const cv::Mat &mask, const std::vector<cv::Point> &contour,
      const cv::Size &imageSize, const cv::Size &originalImageSize, const enums::tile_t &tile, const cv::Size &tileSize);

  ROI(ROI &&input) :
      mIsNull(std::move(input.mIsNull)), mObjectId(std::move(input.mObjectId)), mId(std::move(input.mId)), confidence(std::move(input.confidence)),
      mBoundingBoxTile(std::move(input.mBoundingBoxTile)), mBoundingBoxReal(std::move(input.mBoundingBoxReal)), mMask(std::move(input.mMask)),
      mMaskContours(std::move(input.mMaskContours)), mImageSize(std::move(input.mImageSize)), mOriginalImageSize(std::move(input.mOriginalImageSize)),
      mAreaSize(std::move(input.mAreaSize)), mPerimeter(std::move(input.mPerimeter)), mCircularity(std::move(input.mCircularity)),
      intensity(std::move(input.intensity)), mOriginObjectId(std::move(input.mOriginObjectId)), mCentroid(std::move(input.mCentroid))
  {
  }

  ROI(bool mIsNull, uint64_t mObjectId, RoiObjectId mId, Confidence confidence, Boxes mBoundingBoxTile, Boxes mBoundingBoxReal, cv::Mat mMask,
      std::vector<cv::Point> mMaskContours, cv::Size mImageSize, cv::Size originalImageSize, double mAreaSize, float mPerimeter, float mCircularity,
      std::map<enums::ImageId, Intensity> intensity, uint64_t originObjectId, cv::Point centroid) :
      mIsNull(mIsNull),
      mObjectId(mObjectId), mId(mId), confidence(confidence), mBoundingBoxTile(mBoundingBoxTile), mBoundingBoxReal(mBoundingBoxReal), mMask(mMask),
      mMaskContours(mMaskContours), mImageSize(mImageSize), mOriginalImageSize(originalImageSize), mAreaSize(mAreaSize), mPerimeter(mPerimeter),
      mCircularity(mCircularity), intensity(intensity), mOriginObjectId(originObjectId), mCentroid(centroid)
  {
  }

  bool isNull() const
  {
    return mIsNull;
  }

  [[nodiscard]] ROI clone() const
  {
    return {mIsNull,    mObjectId,          mId,       confidence, mBoundingBoxTile, mBoundingBoxReal, mMask,           mMaskContours,
            mImageSize, mOriginalImageSize, mAreaSize, mPerimeter, mCircularity,     intensity,        mOriginObjectId, mCentroid};
  }

  [[nodiscard]] ROI copy() const
  {
    return {mIsNull,    mGlobalUniqueObjectId++, mId,       confidence, mBoundingBoxTile, mBoundingBoxReal, mMask,     mMaskContours,
            mImageSize, mOriginalImageSize,      mAreaSize, mPerimeter, mCircularity,     intensity,        mObjectId, mCentroid};
  }

  void setClasss(enums::ClassId classId)
  {
    auto &oId   = const_cast<RoiObjectId &>(mId);
    oId.classId = classId;
  }

  void setClass(enums::ClassId classId)
  {
    auto &oId   = const_cast<RoiObjectId &>(mId);
    oId.classId = classId;
  }

  [[nodiscard]] const RoiObjectId &getId() const
  {
    return mId;
  }

  [[nodiscard]] auto getObjectId() const
  {
    return mObjectId;
  }

  [[nodiscard]] auto getClassId() const
  {
    return mId.classId;
  }

  [[nodiscard]] auto getC() const
  {
    return mId.imagePlane.cStack;
  }

  [[nodiscard]] auto getT() const
  {
    return mId.imagePlane.tStack;
  }

  [[nodiscard]] auto getZ() const
  {
    return mId.imagePlane.zStack;
  }

  [[nodiscard]] auto getConfidence() const
  {
    return confidence;
  }

  [[nodiscard]] auto getBoundingBoxReal() const -> const Boxes &
  {
    return mBoundingBoxReal;
  }

  [[nodiscard]] auto getBoundingBoxTile() const -> const Boxes &
  {
    return mBoundingBoxTile;
  }

  [[nodiscard]] auto getCentroidReal() const -> cv::Point
  {
    double cx = mCentroid.x + getBoundingBoxReal().x;
    double cy = mCentroid.y + getBoundingBoxReal().y;

    return cv::Point(cx, cy);
  }

  [[nodiscard]] auto getCentroidTile() const -> cv::Point
  {
    double cx = mCentroid.x + getBoundingBoxTile().x;
    double cy = mCentroid.y + getBoundingBoxTile().y;

    return cv::Point(cx, cy);
  }

  [[nodiscard]] auto getMask() const -> const cv::Mat &
  {
    return mMask;
  }

  [[nodiscard]] auto getContour() const -> const std::vector<cv::Point> &
  {
    return mMaskContours;
  }

  [[nodiscard]] const auto &getIntensity() const
  {
    return intensity;
  }

  [[nodiscard]] double getAreaSize() const
  {
    return mAreaSize;
  }

  [[nodiscard]] float getCircularity() const
  {
    return mCircularity;
  }

  [[nodiscard]] float getPerimeter() const
  {
    return mPerimeter;
  }

  [[nodiscard]] ROI calcIntersection(const enums::PlaneId &iterator, const ROI &roi, float minIntersection, const enums::tile_t &tile,
                                     const cv::Size &tileSize, joda::enums::ClassId objectClassIntersectingObjectsShouldBeAssignedTo) const;

  auto measureIntensityAndAdd(const joda::atom::ImagePlane &image) -> Intensity;

  [[nodiscard]] bool isIntersecting(const ROI &roi, float minIntersection) const;

  uint64_t getOriginObjectId() const
  {
    return mOriginObjectId;
  }

  void resize(float scaleX, float scaleY);

private:
  /////////////////////////////////////////////////////
  [[nodiscard]] uint64_t calcAreaSize() const;
  [[nodiscard]] float calcCircularity() const;
  [[nodiscard]] auto calcCentroid(const cv::Mat &) const -> cv::Point;
  [[nodiscard]] Boxes calcRealBoundingBox(const enums::tile_t &tile, const cv::Size &tileSize);
  [[nodiscard]] std::tuple<int32_t, int32_t, int32_t, int32_t, int32_t> calcCircleRadius(int32_t snapAreaSize) const;

  auto calcIntensity(const cv::Mat &imageOriginal) -> Intensity;
  [[nodiscard]] auto calcIntersectingMask(const ROI &roi) const -> IntersectingMask;
  [[nodiscard]] static double getSmoothedLineLength(const std::vector<cv::Point> &);
  [[nodiscard]] static double getLength(const std::vector<cv::Point> &points, bool closeShape);
  [[nodiscard]] static float getTracedPerimeter(const std::vector<cv::Point> &points);

  // Identification ///////////////////////////////////////////////////
  const bool mIsNull;
  const uint64_t mObjectId;       ///< Global unique object ID
  const RoiObjectId mId;          ///< Unique identification of the this ROI
  const Confidence confidence;    ///< Probability

  // Metrics ///////////////////////////////////////////////////
  Boxes mBoundingBoxTile;    ///< Rectangle around the prediction in tile
  Boxes mBoundingBoxReal;    ///< Rectangle around the prediction with real coordinates
  cv::Mat mMask;             ///< Segmentation mask
  std::vector<cv::Point> mMaskContours;

  const cv::Size mImageSize;
  const cv::Size mOriginalImageSize;
  double mAreaSize;      ///< size of the masking area [px^2 / px^3]
  float mPerimeter;      ///< Perimeter (boundary size) [px]
  float mCircularity;    ///< Circularity of the masking area [0-1]
  cv::Point mCentroid;

  // Measurements ///////////////////////////////////////////////////
  std::map<enums::ImageId, Intensity> intensity;
  uint64 mOriginObjectId = 0;

  static inline std::atomic<uint64_t> mGlobalUniqueObjectId = 0;
};
}    // namespace joda::atom
