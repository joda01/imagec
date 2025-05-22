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

#pragma once

#include <algorithm>
#include <atomic>
#include <bitset>
#include <string>
#include <utility>
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

  struct Distance
  {
    double distanceCentroidToCentroid   = 0;    ///< Euclid distance from centroid to centroid.
    double distanceCentroidToSurfaceMin = 0;    ///< Smallest euclid distance from centroid to surface.
    double distanceCentroidToSurfaceMax = 0;    ///< Highest euclid distance from centroid to surface.
    double distanceSurfaceToSurfaceMin  = 0;    ///< Smallest euclid distance from surface to surface.
    double distanceSurfaceToSurfaceMax  = 0;    ///< Highest euclid distance from surface to surface.
  };

  struct Intersecting
  {
    std::vector<RoiObjectId> roiValid;
  };

  struct IntersectingMask
  {
    uint32_t nrOfIntersectingPixels = 0;
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
      intensity(std::move(input.intensity)), mOriginObjectId(std::move(input.mOriginObjectId)), mCentroid(std::move(input.mCentroid)),
      mParentObjectId(std::move(input.mParentObjectId)), mTrackingId(std::move(input.mTrackingId)), mLinkedWith(std::move(input.mLinkedWith))
  {
  }

  ROI(bool mIsNull, uint64_t mObjectId, RoiObjectId mId, Confidence confidence, Boxes mBoundingBoxTile, Boxes mBoundingBoxReal, cv::Mat mMask,
      std::vector<cv::Point> mMaskContours, cv::Size mImageSize, cv::Size originalImageSize, double mAreaSize, float mPerimeter, float mCircularity,
      std::map<enums::ImageId, Intensity> intensity, uint64_t originObjectId, cv::Point centroid, uint64_t parentObjectId, uint64_t linkedObjectId,
      const std::set<ROI *> &linkedWith) :
      mIsNull(mIsNull),
      mObjectId(mObjectId), mId(mId), confidence(confidence), mBoundingBoxTile(mBoundingBoxTile), mBoundingBoxReal(mBoundingBoxReal), mMask(mMask),
      mMaskContours(mMaskContours), mImageSize(mImageSize), mOriginalImageSize(originalImageSize), mAreaSize(mAreaSize), mPerimeter(mPerimeter),
      mCircularity(mCircularity), intensity(intensity), mOriginObjectId(originObjectId), mCentroid(centroid), mParentObjectId(parentObjectId),
      mTrackingId(linkedObjectId), mLinkedWith(linkedWith)
  {
  }

  bool isNull() const
  {
    return mIsNull;
  }

  [[nodiscard]] ROI clone() const
  {
    return {mIsNull,         mObjectId,          mId,        confidence, mBoundingBoxTile, mBoundingBoxReal, mMask,           mMaskContours,
            mImageSize,      mOriginalImageSize, mAreaSize,  mPerimeter, mCircularity,     intensity,        mOriginObjectId, mCentroid,
            mParentObjectId, mTrackingId,        mLinkedWith};
  }

  [[nodiscard]] ROI clone(enums::ClassId newClassId, uint64_t newParentObjectId) const
  {
    return {mIsNull,
            mObjectId,
            {newClassId, mId.imagePlane},
            confidence,
            mBoundingBoxTile,
            mBoundingBoxReal,
            mMask,
            mMaskContours,
            mImageSize,
            mOriginalImageSize,
            mAreaSize,
            mPerimeter,
            mCircularity,
            intensity,
            mOriginObjectId,
            mCentroid,
            newParentObjectId,
            mTrackingId,
            mLinkedWith};
  }

  [[nodiscard]] ROI copy() const
  {
    return {mIsNull,
            mGlobalUniqueObjectId++,
            mId,
            confidence,
            mBoundingBoxTile,
            mBoundingBoxReal,
            mMask,
            mMaskContours,
            mImageSize,
            mOriginalImageSize,
            mAreaSize,
            mPerimeter,
            mCircularity,
            intensity,
            mObjectId,
            mCentroid,
            mParentObjectId,
            mTrackingId,
            mLinkedWith};
  }

  [[nodiscard]] ROI copy(enums::ClassId newClassId, uint64_t newParentObjectId) const
  {
    return {mIsNull,
            mGlobalUniqueObjectId++,
            {newClassId, mId.imagePlane},
            confidence,
            mBoundingBoxTile,
            mBoundingBoxReal,
            mMask,
            mMaskContours,
            mImageSize,
            mOriginalImageSize,
            mAreaSize,
            mPerimeter,
            mCircularity,
            intensity,
            mObjectId,
            mCentroid,
            newParentObjectId,
            mTrackingId,
            mLinkedWith};
  }

  /// @brief  ATTENTION This method must be done befor the ROI is added to the spheral index
  /// @param classId
  void changeClass(enums::ClassId classId, uint64_t newParentObjectId)
  {
    auto &oId       = const_cast<RoiObjectId &>(mId);
    oId.classId     = classId;
    mParentObjectId = newParentObjectId;
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

  [[nodiscard]] const auto &getDistances() const
  {
    return distances;
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
  auto measureDistanceAndAdd(const ROI &secondRoi) -> Distance;

  [[nodiscard]] bool isIntersecting(const ROI &roi, float minIntersection) const;

  uint64_t getOriginObjectId() const
  {
    return mOriginObjectId;
  }

  void setParentObjectId(uint64_t parentObjectId)
  {
    mParentObjectId = parentObjectId;
  }

  uint64_t getParentObjectId() const
  {
    return mParentObjectId;
  }

  void resize(float scaleX, float scaleY);
  void drawCircle(float radius);
  void addLinkedRoi(ROI *linked)
  {
    mLinkedWith.emplace(linked);
  }
  void addLinkedRoi(const std::set<ROI *> &links)
  {
    mLinkedWith.insert(links.begin(), links.end());
  }
  auto getLinkedRois() const -> const std::set<ROI *> &
  {
    return mLinkedWith;
  }
  void clearLinkedWith()
  {
    mLinkedWith.clear();
  }

  uint64_t getTrackingId() const
  {
    return mTrackingId;
  }

  void setTrackingId(uint64_t trackingId)
  {
    mTrackingId = trackingId;
  }

  static uint64_t generateNewTrackingId()
  {
    uint64_t trackingID = mGlobalUniqueTrackingId;
    mGlobalUniqueTrackingId++;
    return trackingID;
  }

  void assignTrackingIdToAllLinkedRois(uint64_t trackingIdForLinked = 0);

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
  uint64_t mParentObjectId = 0;    // 0 if this object has no parent
  uint64_t mTrackingId     = 0;    // 0 if not linked with anything

  // Measurements ///////////////////////////////////////////////////
  std::map<enums::ImageId, Intensity> intensity;
  std::map<uint64_t, Distance> distances;    ///< Key is the ID of the object the distance was calculated to.
  uint64_t mOriginObjectId = 0;

  static inline std::atomic<uint64_t> mGlobalUniqueObjectId   = 1;
  static inline std::atomic<uint64_t> mGlobalUniqueTrackingId = 1;
  std::set<ROI *> mLinkedWith;    // Temporary object to store linked objects and create a linked object IF afterwards
};
}    // namespace joda::atom
