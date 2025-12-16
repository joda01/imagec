///
/// \file      object_tracker.hpp
/// \author    Joachim Danmayr
/// \date      2025-12-16
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include "backend/artifacts/roi/roi.hpp"
#include <opencv2/opencv.hpp>

namespace joda::data_analyze::object_tracker {

// struct Spot
//{
//   int posX;
//   int posY;
//   int width;
//   int height;
//   uint64_t objectId;
//   uint64_t trackingId = 0;
// };

using TimeFrame_t = uint32_t;

struct TrackState
{
  uint64_t id;
  cv::Rect bbox;
  int missed;
};

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class ObjectTracker
{
public:
  /////////////////////////////////////////////////////
  ObjectTracker(float iouThreshold = 0.3f, int maxMisses = 5);

  void runTracker(std::map<int32_t, std::vector<joda::atom::ROI>> &spotsPerFrame);
  void update(TimeFrame_t frame, std::vector<joda::atom::ROI> &detections);
  std::vector<TrackState> getActiveTracks() const;

private:
  /////////////////////////////////////////////////////
  struct Track
  {
    uint64_t id;
    cv::KalmanFilter kf;
    cv::Rect bbox;
    int missed = 0;
  };

  std::vector<Track> mTracks;
  uint64_t mNextId = 1;
  float mIouThreshold;
  int maxMisses_;

  void predict();
  void associate(std::vector<joda::atom::ROI> &detections);
  void createTracks(std::vector<joda::atom::ROI> &detections);
  void cleanup();

  static cv::KalmanFilter createKF(const cv::Rect &r);
  static float iou(const cv::Rect &a, const cv::Rect &b);
};

}    // namespace joda::data_analyze::object_tracker
