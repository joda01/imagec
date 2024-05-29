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

#pragma once

#include <bitset>
#include <string>
#include <vector>
#include "backend/settings/channel/channel_settings_filter.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/opencv.hpp>

namespace joda::image {

using Boxes      = cv::Rect;
using Confidence = float;
using ClassId    = int;

enum class ParticleValidityEnums
{
  UNKNOWN              = 1,
  INVALID              = 2,
  MANUAL_OUT_SORTED    = 3,
  TOO_SMALL            = 4,
  TOO_BIG              = 5,
  TOO_LESS_CIRCULARITY = 6,
  TOO_LESS_OVERLAPPING = 7,
  REFERENCE_SPOT       = 8,
  AT_THE_EDGE          = 9,
};

using ParticleValidity = std::bitset<32>;

///
/// \class      ROI
/// \author     Joachim Danmayr
/// \brief      Region of interest implementation
///
class ROI
{
public:
  struct Intensity
  {
    double intensity    = 0;    ///< Avg intensity of the masking area
    double intensityMin = 0;    ///< Min intensity of the masking area
    double intensityMax = 0;    ///< Max intensity of the masking area
  };

  struct Intersecting
  {
    std::vector<const ROI *> roiValid;
    std::vector<const ROI *> roiInvalid;
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
  ROI(uint32_t index, Confidence confidence, ClassId classId, const Boxes &boundingBox, const cv::Mat &mask,
      const std::vector<cv::Point> &contour);

  ROI(uint32_t index, Confidence confidence, ClassId classId, const Boxes &boundingBox, const cv::Mat &mask,
      const std::vector<cv::Point> &contour, const cv::Mat &imageOriginal, joda::settings::ChannelIndex channelIndex);

  ROI(uint32_t index, Confidence confidence, ClassId classId, const Boxes &boundingBox, const cv::Mat &mask,
      const std::vector<cv::Point> &contour, const cv::Mat &imageOriginal, joda::settings::ChannelIndex channelIndex,
      const joda::settings::ChannelSettingsFilter &filter);

  ROI(uint32_t index, Confidence confidence, ClassId classId, const Boxes &boundingBox, const cv::Mat &mask,
      const std::vector<cv::Point> &contour,
      const std::map<joda::settings::ChannelIndex, const cv::Mat *> &imageOriginal);

  [[nodiscard]] auto getIndex() const
  {
    return index;
  }

  [[nodiscard]] auto getClassId() const
  {
    return classId;
  }

  [[nodiscard]] auto getConfidence() const
  {
    return confidence;
  }

  [[nodiscard]] auto getBoundingBox() const -> const Boxes &
  {
    return mBoundingBox;
  }

  [[nodiscard]] auto getCenterOfMass() const -> cv::Point
  {
    return {(mBoundingBox.x + mBoundingBox.width / 2), (mBoundingBox.y + mBoundingBox.height / 2)};
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
    return mBoundingBox;
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
    return mHasSnapArea;
  }

  [[nodiscard]] const std::map<joda::settings::ChannelIndex, Intensity> &getIntensity(int idx = 0) const
  {
    return intensity;
  }

  [[nodiscard]] const std::map<joda::settings::ChannelIndex, Intersecting> &getIntersectingRois(int idx = 0) const
  {
    return intersectingRois;
  }

  [[nodiscard]] uint64_t getAreaSize() const
  {
    return areaSize;
  }

  [[nodiscard]] float getCircularity() const
  {
    return circularity;
  }

  [[nodiscard]] float getPerimeter() const
  {
    return perimeter;
  }

  [[nodiscard]] bool isValid() const
  {
    return 0 == validity.to_ulong();
  }

  [[nodiscard]] auto getValidity() const
  {
    return validity;
  }

  void setValidity(ParticleValidityEnums valid)
  {
    validity.set(static_cast<size_t>(valid));
  }

  [[nodiscard]] std::tuple<ROI, bool>
  calcIntersection(const ROI &roi, const std::map<joda::settings::ChannelIndex, const cv::Mat *> &imageOriginal,
                   float minIntersection, bool createRoi = true) const;

  void measureAndAddIntensity(joda::settings::ChannelIndex channelIdx, const cv::Mat &imageOriginal);
  void calcIntersectionAndAdd(joda::settings::ChannelIndex channelIdx, const ROI *roi);

  [[nodiscard]] bool isIntersecting(const ROI &roi, float minIntersection) const;

private:
  /////////////////////////////////////////////////////
  void calculateMetrics(const std::map<joda::settings::ChannelIndex, const cv::Mat *> &imageOriginal,
                        const joda::settings::ChannelSettingsFilter *filter);

  auto calcIntensity(const cv::Mat &imageOriginal) -> Intensity;
  auto calcIntersectingMask(const ROI &roi) const -> IntersectingMask;
  void calculateSnapAreaAndContours(float snapAreaSize, int32_t maxWidth, int32_t maxHeight);
  void applyParticleFilter(const joda::settings::ChannelSettingsFilter *filter);
  [[nodiscard]] double calcPerimeter(const std::vector<cv::Point> &) const;
  [[nodiscard]] double getSmoothedLineLength(const std::vector<cv::Point> &) const;
  [[nodiscard]] double getLength(const std::vector<cv::Point> &points, bool closeShape) const;
  [[nodiscard]] double getTracedPerimeter(const std::vector<cv::Point> &points) const;

  /////////////////////////////////////////////////////
  uint32_t index;           ///< Index in the prediction array
  Confidence confidence;    ///< Probability
  ClassId classId = -1;     ///< Class id

  Boxes mBoundingBox;    ///< Rectangle around the prediction
  cv::Mat mMask;         ///< Segmentation mask
  std::vector<cv::Point> mMaskContours;

  Boxes mSnapAreaBoundingBox;    ///< Rectangle around the prediction with snap area
  cv::Mat mSnapAreaMask;         ///< Segmentation mask with snap area
  std::vector<cv::Point> mSnapAreaMaskContours;

  std::map<joda::settings::ChannelIndex, Intensity> intensity;    ///< Key is the channel index
  std::map<joda::settings::ChannelIndex, Intersecting>
      intersectingRois;    ///< Key is the channel index, value an array of ROIs which intersects with this ROI

  uint64_t areaSize = 0;    ///< size of the masking area [px^2 / px^3]
  float perimeter   = 0;    ///< Perimter (boundary size) [px]
  float circularity = 0;    ///< Circularity of the masking area [0-1]
  ParticleValidity validity{};
  bool mHasSnapArea = false;
};
}    // namespace joda::image
