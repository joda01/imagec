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
    uint64_t objectId;
    joda::enums::ClusterId clusterId;
    joda::enums::ClassId classId;
    joda::enums::PlaneId imagePlane;

    bool operator<(const RoiObjectId &in) const
    {
      return objectId < in.objectId;
    }
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
  ROI(RoiObjectId index, Confidence confidence, uint32_t snapAreaSize, const Boxes &boundingBox, const cv::Mat &mask,
      const std::vector<cv::Point> &contour, const cv::Size &imageSize, const enums::tile_t &tile,
      const cv::Size &tileSize);

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
    return mId.objectId;
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

  [[nodiscard]] std::tuple<ROI, bool>
  calcIntersection(const enums::PlaneId &iterator, const ROI &roi, uint64_t indexOfIntersectingRoi,
                   uint32_t snapAreaOfIntersectingRoi, float minIntersection, const enums::tile_t &tile,
                   const cv::Size &tileSize, joda::enums::ClusterId objectClusterIntersectingObjectsShouldBeAssignedTo,
                   joda::enums::ClassId objectClassIntersectingObjectsShouldBeAssignedTo) const;

  auto measureIntensityAndAdd(const joda::atom::ImagePlane &image) -> Intensity;

  [[nodiscard]] bool isIntersecting(const ROI &roi, float minIntersection) const;

  void addIntersectingRoi(const ROI *roi)
  {
    mIntersectingRois.emplace(roi->mId);
  }
  [[nodiscard]] auto getIntersections() const -> const std::set<RoiObjectId> &
  {
    return mIntersectingRois;
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

  const double mAreaSize   = 0;    ///< size of the masking area [px^2 / px^3]
  const float mPerimeter   = 0;    ///< Perimeter (boundary size) [px]
  const float mCircularity = 0;    ///< Circularity of the masking area [0-1]

  // Measurements ///////////////////////////////////////////////////
  std::map<enums::ImageId, Intensity> intensity;
  std::set<RoiObjectId> mIntersectingRois;
};
}    // namespace joda::atom
