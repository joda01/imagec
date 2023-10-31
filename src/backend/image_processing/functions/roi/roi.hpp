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

#include <string>
#include <vector>
#include "../../../settings/channel_settings.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/opencv.hpp>

namespace joda::func {

using Boxes      = cv::Rect;
using Confidence = float;
using ClassId    = int;

enum class ParticleValidity : int
{
  UNKNOWN              = 0,
  VALID                = 0x01,
  TOO_SMALL            = 0x02,
  TOO_BIG              = 0x04,
  TOO_LESS_CIRCULARITY = 0x08,
  TOO_LESS_OVERLAPPING = 0x10,
  REFERENCE_SPOT       = 0x20,
};

///
/// \class      ROI
/// \author     Joachim Danmayr
/// \brief      Region of interest implementation
///
class ROI
{
public:
  /////////////////////////////////////////////////////
  ROI(uint32_t index, Confidence confidence, ClassId classId, const Boxes &boundingBox, const cv::Mat &mask);
  ROI(uint32_t index, Confidence confidence, ClassId classId, const Boxes &boundingBox, const cv::Mat &mask,
      const cv::Mat &imageOriginal, const joda::settings::json::ChannelFiltering *filter);

  ROI(uint32_t index, Confidence confidence, ClassId classId, const Boxes &boundingBox, const cv::Mat &mask,
      const cv::Mat &imageOriginal);

  void calculateMetrics(const cv::Mat &imageOriginal, const joda::settings::json::ChannelFiltering *filter);

  [[nodiscard]] auto getIndex() const
  {
    return index;
  }

  [[nodiscard]] auto getConfidence() const
  {
    return confidence;
  }

  [[nodiscard]] auto getBoundingBox() const -> const Boxes &
  {
    return box;
  }

  [[nodiscard]] auto getMask() const -> const cv::Mat &
  {
    return boxMask;
  }

  [[nodiscard]] auto getIntensity() const
  {
    return intensity;
  }

  [[nodiscard]] auto getIntensityMin() const
  {
    return intensityMin;
  }

  [[nodiscard]] auto getIntensityMax() const
  {
    return intensityMax;
  }

  [[nodiscard]] auto getAreaSize() const
  {
    return areaSize;
  }

  [[nodiscard]] auto getCircularity() const
  {
    return circularity;
  }

  [[nodiscard]] auto getPerimeter() const
  {
    return perimeter;
  }

  [[nodiscard]] bool isValid() const
  {
    return validity == ParticleValidity::VALID;
  }

  [[nodiscard]] auto getValidity() const
  {
    return validity;
  }

  void setValidity(ParticleValidity valid)
  {
    validity = valid;
  }

  [[nodiscard]] std::tuple<ROI, bool> calcIntersection(const ROI &roi, const cv::Mat &imageOriginal,
                                                       float minIntersection, bool createRoi = true) const;

  [[nodiscard]] bool isIntersecting(const ROI &roi, float minIntersection) const;

private:
  /////////////////////////////////////////////////////
  void applyParticleFilter(const joda::settings::json::ChannelFiltering *filter);
  [[nodiscard]] double calcPerimeter(const std::vector<cv::Point> &) const;
  [[nodiscard]] double getSmoothedLineLength(const std::vector<cv::Point> &) const;
  [[nodiscard]] double getLength(const std::vector<cv::Point> &points, bool closeShape) const;
  [[nodiscard]] double getTracedPerimeter(const std::vector<cv::Point> &points) const;
  /////////////////////////////////////////////////////
  uint32_t index;                     ///< Index in the prediction array
  Confidence confidence;              ///< Probability
  ClassId classId;                    ///< Class id
  Boxes box;                          ///< Rectangle around the prediction
  cv::Mat boxMask;                    ///< Segmentation mask
  double intensity    = 0;            ///< Avg intensity of the masking area
  double intensityMin = USHRT_MAX;    ///< Min intensity of the masking area
  double intensityMax = 0;            ///< Max intensity of the masking area
  uint64_t areaSize   = 0;            ///< size of the masking area [px^2 / px^3]
  double perimeter    = 0;            ///< Perimter (boundary size) [px]
  float circularity{};                ///< Circularity of the masking area [0-1]
  ParticleValidity validity = ParticleValidity::UNKNOWN;
};
}    // namespace joda::func
