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
#include "backend/enums/enums_clusters.hpp"
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
    joda::enums::ClusterId clusterId;
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
  ROI(RoiObjectId index, Confidence confidence, uint32_t snapAreaSize, const Boxes &boundingBox, const cv::Mat &mask,
      const std::vector<cv::Point> &contour, const cv::Size &imageSize, const enums::tile_t &tile, const cv::Size &tileSize);

  ROI(ROI &&input) :
      mIsNull(std::move(input.mIsNull)), mObjectId(std::move(input.mObjectId)), mId(std::move(input.mId)), confidence(std::move(input.confidence)),
      mBoundingBoxTile(std::move(input.mBoundingBoxTile)), mBoundingBoxReal(std::move(input.mBoundingBoxReal)), mMask(std::move(input.mMask)),
      mMaskContours(std::move(input.mMaskContours)), mImageSize(std::move(input.mImageSize)),
      mSnapAreaBoundingBox(std::move(input.mSnapAreaBoundingBox)), mSnapAreaMask(std::move(input.mSnapAreaMask)),
      mSnapAreaMaskContours(std::move(input.mSnapAreaMaskContours)), mSnapAreaRadius(std::move(input.mSnapAreaRadius)),
      mAreaSize(std::move(input.mAreaSize)), mPerimeter(std::move(input.mPerimeter)), mCircularity(std::move(input.mCircularity)),
      intensity(std::move(input.intensity)), mOriginObjectId(std::move(input.mOriginObjectId))
  {
  }

  ROI(bool mIsNull, uint64_t mObjectId, RoiObjectId mId, Confidence confidence, Boxes mBoundingBoxTile, Boxes mBoundingBoxReal, cv::Mat mMask,
      std::vector<cv::Point> mMaskContours, cv::Size mImageSize, Boxes mSnapAreaBoundingBox, cv::Mat mSnapAreaMask,
      std::vector<cv::Point> mSnapAreaMaskContours, uint32_t mSnapAreaRadius, double mAreaSize, float mPerimeter, float mCircularity,
      std::map<enums::ImageId, Intensity> intensity, uint64_t originObjectId) :
      mIsNull(mIsNull),
      mObjectId(mObjectId), mId(mId), confidence(confidence), mBoundingBoxTile(mBoundingBoxTile), mBoundingBoxReal(mBoundingBoxReal), mMask(mMask),
      mMaskContours(mMaskContours), mImageSize(mImageSize), mSnapAreaBoundingBox(mSnapAreaBoundingBox), mSnapAreaMask(mSnapAreaMask),
      mSnapAreaMaskContours(mSnapAreaMaskContours), mSnapAreaRadius(mSnapAreaRadius), mAreaSize(mAreaSize), mPerimeter(mPerimeter),
      mCircularity(mCircularity), intensity(intensity), mOriginObjectId(originObjectId)
  {
  }

  bool isNull() const
  {
    return mIsNull;
  }

  [[nodiscard]] ROI clone() const
  {
    return {mIsNull,         mObjectId,     mId,        confidence,           mBoundingBoxTile, mBoundingBoxReal,
            mMask,           mMaskContours, mImageSize, mSnapAreaBoundingBox, mSnapAreaMask,    mSnapAreaMaskContours,
            mSnapAreaRadius, mAreaSize,     mPerimeter, mCircularity,         intensity,        mOriginObjectId};
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
            mSnapAreaBoundingBox,
            mSnapAreaMask,
            mSnapAreaMaskContours,
            mSnapAreaRadius,
            mAreaSize,
            mPerimeter,
            mCircularity,
            intensity,
            mObjectId};
  }

  void setClusterAndClass(enums::ClusterId clusterId, enums::ClassId classId)
  {
    auto &oId     = const_cast<RoiObjectId &>(mId);
    oId.clusterId = clusterId;
    oId.classId   = classId;
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

  [[nodiscard]] auto getClusterId() const
  {
    return mId.clusterId;
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

  [[nodiscard]] auto getBoundingBox() const -> const Boxes &
  {
    return mBoundingBoxTile;
  }

  [[nodiscard]] auto getCenterOfMassReal() const -> cv::Point
  {
    return {(mBoundingBoxReal.x + mBoundingBoxReal.width / 2), (mBoundingBoxReal.y + mBoundingBoxReal.height / 2)};
  }

  [[nodiscard]] auto getCenterOfMassInTile() const -> cv::Point
  {
    return {(mBoundingBoxTile.x + mBoundingBoxTile.width / 2), (mBoundingBoxTile.y + mBoundingBoxTile.height / 2)};
  }

  [[nodiscard]] auto getMask() const -> const cv::Mat &
  {
    return mMask;
  }

  [[nodiscard]] auto getContour() const -> const std::vector<cv::Point> &
  {
    return mMaskContours;
  }

  [[nodiscard]] auto getSnapAreaBoundingBox() const -> const Boxes &
  {
    if(hasSnapArea()) {
      return mSnapAreaBoundingBox;
    }
    return mBoundingBoxTile;
  }

  [[nodiscard]] auto getSnapAreaMask() const -> const cv::Mat &
  {
    if(hasSnapArea()) {
      return mSnapAreaMask;
    }
    return mMask;
  }

  [[nodiscard]] auto getSnapAreaContour() const -> const std::vector<cv::Point> &
  {
    if(hasSnapArea()) {
      return mSnapAreaMaskContours;
    }
    return mMaskContours;
  }

  [[nodiscard]] auto hasSnapArea() const -> bool
  {
    return !mSnapAreaMaskContours.empty();
  }

  [[nodiscard]] const auto &getIntensity() const
  {
    return intensity;
  }

  [[nodiscard]] double getAreaSize() const
  {
    return mAreaSize;
  }

  [[nodiscard]] uint32_t getSnapAreaRadius() const
  {
    return mSnapAreaRadius;
  }

  [[nodiscard]] float getCircularity() const
  {
    return mCircularity;
  }

  [[nodiscard]] float getPerimeter() const
  {
    return mPerimeter;
  }

  [[nodiscard]] ROI calcIntersection(const enums::PlaneId &iterator, const ROI &roi, uint32_t snapAreaOfIntersectingRoi, float minIntersection,
                                     const enums::tile_t &tile, const cv::Size &tileSize,
                                     joda::enums::ClusterId objectClusterIntersectingObjectsShouldBeAssignedTo,
                                     joda::enums::ClassId objectClassIntersectingObjectsShouldBeAssignedTo) const;

  auto measureIntensityAndAdd(const joda::atom::ImagePlane &image) -> Intensity;

  [[nodiscard]] bool isIntersecting(const ROI &roi, float minIntersection) const;

  uint64_t getOriginObjectId() const
  {
    return mOriginObjectId;
  }

private:
  /////////////////////////////////////////////////////
  [[nodiscard]] uint64_t calcAreaSize() const;
  [[nodiscard]] float calcPerimeter() const;
  [[nodiscard]] float calcCircularity() const;
  [[nodiscard]] Boxes calcSnapAreaBoundingBox(int32_t snapAreaSize, const cv::Size &imageSize) const;
  [[nodiscard]] Boxes calcRealBoundingBox(const enums::tile_t &tile, const cv::Size &tileSize);
  [[nodiscard]] cv::Mat calculateSnapAreaMask(int32_t snapAreaSize) const;
  [[nodiscard]] std::vector<cv::Point> calculateSnapContours(int32_t snapAreaSize) const;
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
  const Boxes mBoundingBoxTile;    ///< Rectangle around the prediction in tile
  const Boxes mBoundingBoxReal;    ///< Rectangle around the prediction with real coordinates
  const cv::Mat mMask;             ///< Segmentation mask
  const std::vector<cv::Point> mMaskContours;

  const cv::Size mImageSize;
  const Boxes mSnapAreaBoundingBox;    ///< Rectangle around the prediction with snap area
  const cv::Mat mSnapAreaMask;         ///< Segmentation mask with snap area
  const std::vector<cv::Point> mSnapAreaMaskContours;
  const uint32_t mSnapAreaRadius = 0;

  const double mAreaSize;      ///< size of the masking area [px^2 / px^3]
  const float mPerimeter;      ///< Perimeter (boundary size) [px]
  const float mCircularity;    ///< Circularity of the masking area [0-1]

  // Measurements ///////////////////////////////////////////////////
  std::map<enums::ImageId, Intensity> intensity;
  uint64 mOriginObjectId = 0;

  static inline std::atomic<uint64_t> mGlobalUniqueObjectId = 0;
};
}    // namespace joda::atom
