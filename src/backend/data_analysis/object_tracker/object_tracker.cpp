// ===============================
// multi_object_tracker.h
// ===============================

#include "object_tracker.hpp"
#include <algorithm>
#include <limits>
#include <vector>
#include "backend/artifacts/roi/roi.hpp"

namespace joda::data_analyze::object_tracker {

ObjectTracker::ObjectTracker(float iouThreshold, int maxMisses) : mIouThreshold(iouThreshold), maxMisses_(maxMisses)
{
}

///
/// \brief      The tracking ID is used for coloc and video tracking as well
///             When starting the tracker, set the tracking ID to zero to reinit with a video tracking id.
///             Afterwards replace the tracking ID in the database from an object with the new video tracking ID
///             for the corresponding objects.
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void ObjectTracker::runTracker(std::map<int32_t, std::vector<joda::atom::ROI>> &spotsPerFrame)
{
  // We reset possible old tracking IDs
  std::map<uint64_t, uint64_t> objectIdVsTrackerId;
  for(auto &[tStack, frame] : spotsPerFrame) {
    for(auto &roi : frame) {
      objectIdVsTrackerId.emplace(roi.getObjectId(), roi.getTrackingId());
      roi.setTrackingId(0);
    }
  }

  // Run tracking
  for(auto &[frameId, spots] : spotsPerFrame) {
    update(frameId, spots);
  }
  std::cout << "Finished" << std::endl;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::KalmanFilter ObjectTracker::createKF(const cv::Rect &r)
{
  cv::KalmanFilter kf(4, 2);
  kf.transitionMatrix  = (cv::Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);
  kf.measurementMatrix = (cv::Mat_<float>(2, 4) << 1, 0, 0, 0, 0, 1, 0, 0);
  setIdentity(kf.processNoiseCov, cv::Scalar::all(1e-2));
  setIdentity(kf.measurementNoiseCov, cv::Scalar::all(1e-1));
  setIdentity(kf.errorCovPost, cv::Scalar::all(1));
  kf.statePost.at<float>(0) = r.x + r.width / 2.f;
  kf.statePost.at<float>(1) = r.y + r.height / 2.f;
  return kf;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
float ObjectTracker::iou(const cv::Rect &a, const cv::Rect &b)
{
  const int inter = (a & b).area();
  const int uni   = a.area() + b.area() - inter;
  return uni > 0 ? static_cast<float>(inter) / uni : 0.0f;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void ObjectTracker::predict()
{
  for(auto &t : mTracks) {
    auto p   = t.kf.predict();
    t.bbox.x = p.at<float>(0) - t.bbox.width / 2;
    t.bbox.y = p.at<float>(1) - t.bbox.height / 2;
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void ObjectTracker::associate(std::vector<joda::atom::ROI> &detections)
{
  if(mTracks.empty() || detections.empty()) {
    return;
  }

  cv::Mat cost(mTracks.size(), detections.size(), CV_32F);
  for(size_t i = 0; i < mTracks.size(); ++i) {
    for(size_t j = 0; j < detections.size(); ++j) {
      cost.at<float>(i, j) = 1.0f - iou(mTracks[i].bbox, detections[j].getBoundingBoxReal());
    }
  }

  // Simple greedy matching (Hungarian can be plugged here)
  std::vector<bool> used(detections.size(), false);

  for(size_t i = 0; i < mTracks.size(); ++i) {
    int best       = -1;
    float bestCost = 1.0f - mIouThreshold;
    for(size_t j = 0; j < detections.size(); ++j) {
      if(used[j]) {
        continue;
      }
      if(cost.at<float>(i, j) < bestCost) {
        bestCost = cost.at<float>(i, j);
        best     = static_cast<int>(j);
      }
    }
    if(best >= 0) {
      auto &d = detections[best];
      cv::Mat m(2, 1, CV_32F);
      const auto &boundingBox = d.getBoundingBoxReal();
      m.at<float>(0)          = boundingBox.x + boundingBox.width / 2.f;
      m.at<float>(1)          = boundingBox.y + boundingBox.height / 2.f;
      mTracks[i].kf.correct(m);
      mTracks[i].bbox   = boundingBox;
      mTracks[i].missed = 0;
      d.setTrackingId(mTracks[i].id);
      used[best] = true;
    } else {
      mTracks[i].missed++;
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void ObjectTracker::createTracks(std::vector<joda::atom::ROI> &detections)
{
  for(auto &d : detections) {
    if(d.getTrackingId() != 0) {
      continue;
    }
    Track t;
    t.id   = mNextId++;
    t.bbox = d.getBoundingBoxReal();
    t.kf   = createKF(t.bbox);
    d.setTrackingId(t.id);
    mTracks.push_back(t);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void ObjectTracker::cleanup()
{
  mTracks.erase(std::remove_if(mTracks.begin(), mTracks.end(), [&](const Track &t) { return t.missed > maxMisses_; }), mTracks.end());
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void ObjectTracker::update(TimeFrame_t, std::vector<joda::atom::ROI> &detections)
{
  predict();
  associate(detections);
  createTracks(detections);
  cleanup();
}

std::vector<TrackState> ObjectTracker::getActiveTracks() const
{
  std::vector<TrackState> out;
  for(const auto &t : mTracks) {
    out.push_back({t.id, t.bbox, t.missed});
  }
  return out;
}

}    // namespace joda::data_analyze::object_tracker
