///
/// \file      calc_intersection.cpp
/// \author    Joachim Danmayr
/// \date      2023-09-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "calc_intersection.hpp"
#include <string>
#include "backend/duration_count/duration_count.h"
#include "backend/image_processing/detection/detection.hpp"
#include "backend/image_processing/roi/roi.hpp"
#include <opencv2/core.hpp>

bool overlap(const joda::func::ROI &r1, const joda::func::ROI &r2)
{
  return !(r1.x2 < r2.x1 || r1.x1 > r2.x2 || r1.y2 < r2.y1 || r1.y1 > r2.y2);
}

std::vector<std::pair<joda::func::ROI, bool>> mergeAndMark(std::vector<joda::func::ROI> &rects)
{
  std::vector<std::pair<joda::func::ROI, bool>> events;
  for(auto &rect : rects) {
    events.emplace_back(rect, true);                                         // start event
    events.emplace_back(Rect(rect.x1, rect.y1, rect.x2, rect.y2), false);    // end event
  }
  sort(events.begin(), events.end(), [](const auto &a, const auto &b) { return a.first.x1 < b.first.x1; });
  return events;
}
namespace joda::pipeline {

CalcIntersection::CalcIntersection(const std::set<joda::settings::ChannelIndex> &indexesToIntersect,
                                   float minIntersection) :
    mIndexesToIntersect(indexesToIntersect.begin(), indexesToIntersect.end()),
    mMinIntersection(minIntersection)
{
}

auto CalcIntersection::execute(
    const settings::AnalyzeSettings &settings,
    const std::map<joda::settings::ChannelIndex, joda::func::DetectionResponse> &detectionResultsIn,
    const std::string &detailoutputPath) const -> joda::func::DetectionResponse
{
  auto id = DurationCount::start("Intersection");

  if(mIndexesToIntersect.empty() || !detectionResultsIn.contains(*mIndexesToIntersect.begin())) {
    return joda::func::DetectionResponse{};
  }

  std::vector<const joda::func::DetectionResponse *> channelsToIntersect;
  std::map<joda::settings::ChannelIndex, const cv::Mat *> channelsToIntersectImages;

  joda::settings::ChannelIndex idx1 = *mIndexesToIntersect.begin();
  joda::settings::ChannelIndex idx2 = *(std::next(mIndexesToIntersect.begin()));

  for(const auto idxToIntersect : mIndexesToIntersect) {
    if(detectionResultsIn.contains(idxToIntersect)) {
      channelsToIntersect.push_back(&detectionResultsIn.at(idxToIntersect));
      channelsToIntersectImages.emplace(idxToIntersect, &detectionResultsIn.at(idxToIntersect).originalImage);
    }
  }

  // Sort in descending order (largest first)
  auto compareByX = [](const joda::func::DetectionResponse *a, const joda::func::DetectionResponse *b) -> bool {
    {
      return a->result.size() > b->result.size();
    }
  };

  std::sort(channelsToIntersect.begin(), channelsToIntersect.end(), compareByX);

  //
  // Calculate the intersection
  //
  std::vector<func::DetectionFunction::OverlaySettings> overlayPainting;
  joda::func::DetectionResponse response;
  response = *channelsToIntersect[0];
  overlayPainting.push_back({.result          = &channelsToIntersect[0]->result,
                             .backgroundColor = cv::Scalar(255, 0, 0),
                             .borderColor     = cv::Scalar(0, 0, 0),
                             .paintRectangel  = false,
                             .opaque          = 0.3});

  for(auto n = 1; n < channelsToIntersect.size(); n++) {
    const auto *ch1 = channelsToIntersect[n];

    overlayPainting.push_back({.result          = &ch1->result,
                               .backgroundColor = cv::Scalar(0, 255, 0),
                               .borderColor     = cv::Scalar(0, 0, 0),
                               .paintRectangel  = false,
                               .opaque          = 0.3});

    joda::func::DetectionResults mergedRectangles(ch1->result.begin(), ch1->result.end());
    mergedRectangles.insert(mergedRectangles.end(), response.result.begin(), response.result.end());

    // Sort the rectangles based on their x-coordinates
    std::sort(mergedRectangles.begin(), mergedRectangles.end(), compareRectangles);

    // std::vector<std::pair<Rectangle, Rectangle>> overlappingRectangles;
    std::set<func::ROI, decltype(compareRectangles) *> activeRectangles(compareRectangles);

    joda::func::DetectionResponse respTmp;

    for(const auto &rect : mergedRectangles) {
      // Remove rectangles that have ended
      auto it = activeRectangles.begin();
      while(it != activeRectangles.end() &&
            it->getBoundingBox().x + it->getBoundingBox().width <= rect.getBoundingBox().x) {
        it = activeRectangles.erase(it);
      }

      // Check for overlaps with active rectangles
      for(const auto &activeRect : activeRectangles) {
        if(activeRect.getBoundingBox().y + activeRect.getBoundingBox().height > rect.getBoundingBox().y &&
           rect.getBoundingBox().y + rect.getBoundingBox().height > activeRect.getBoundingBox().y) {
          // overlappingRectangles.push_back(std::make_pair(activeRect, rect));
          auto [colocROI, ok] = activeRect.calcIntersection(rect, channelsToIntersectImages, mMinIntersection);
          if(ok) {
            respTmp.result.push_back(colocROI);
          }
        }
      }

      // Add the current rectangle to active set
      activeRectangles.insert(rect);
    }
    response = respTmp;
  }

  //
  // Calculate the intersection of the original images
  //
  response.originalImage = cv::Mat::ones(channelsToIntersectImages.begin()->second->rows,
                                         channelsToIntersectImages.begin()->second->cols, CV_16UC1) *
                           65535;
  for(const auto &img : channelsToIntersectImages) {
    response.originalImage = cv::min(*img.second, response.originalImage);
  }

  overlayPainting.insert(overlayPainting.begin(),
                         func::DetectionFunction::OverlaySettings{.result          = &response.result,
                                                                  .backgroundColor = cv::Scalar(0, 0, 255),
                                                                  .borderColor     = cv::Scalar(0, 0, 0),
                                                                  .paintRectangel  = false,
                                                                  .opaque          = 1});

  response.controlImage =
      cv::Mat::zeros(channelsToIntersect[0]->originalImage.rows, channelsToIntersect[0]->originalImage.cols, CV_32FC3);

  joda::func::DetectionFunction::paintOverlay(response.controlImage, overlayPainting);

  DurationCount::stop(id);
  return response;
}

}    // namespace joda::pipeline

/*

#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

struct Rect {
    int x1, y1, x2, y2;
    Rect(int _x1, int _y1, int _x2, int _y2) : x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}
};

bool overlap(const Rect& r1, const Rect& r2) {
    return !(r1.x2 < r2.x1 || r1.x1 > r2.x2 || r1.y2 < r2.y1 || r1.y1 > r2.y2);
}

vector<pair<Rect, bool>> mergeAndMark(vector<Rect>& rects) {
    vector<pair<Rect, bool>> events;
    for (auto& rect : rects) {
        events.emplace_back(rect, true); // start event
        events.emplace_back(Rect(rect.x1, rect.y1, rect.x2, rect.y2), false); // end event
    }
    sort(events.begin(), events.end(), [](const auto& a, const auto& b) {
        return a.first.x1 < b.first.x1;
    });
    return events;
}

void findOverlappingRectangles(const vector<Rect>& rects1, const vector<Rect>& rects2) {
    vector<pair<Rect, bool>> events1 = mergeAndMark(const_cast<vector<Rect>&>(rects1));
    vector<pair<Rect, bool>> events2 = mergeAndMark(const_cast<vector<Rect>&>(rects2));

    // Sweep line
    vector<Rect> activeRects;
    for (auto& event : events1) {
        const Rect& rect = event.first;
        bool isStart = event.second;

        if (isStart) {
            for (const Rect& activeRect : activeRects) {
                for (const Rect& rect2 : rects2) {
                    if (overlap(rect, rect2)) {
                        cout << "Overlap found between rect1 and rect2" << endl;
                        // Do whatever you want with the overlapping rectangles
                    }
                }
            }
            activeRects.push_back(rect);
        } else {
            activeRects.erase(remove_if(activeRects.begin(), activeRects.end(),
                                         [&rect](const Rect& r) { return r.y1 == rect.y1 && r.y2 == rect.y2; }),
                              activeRects.end());
        }
    }
}

int main() {
    vector<Rect> rects1 = {{1, 1, 4, 4}, {2, 2, 5, 5}};
    vector<Rect> rects2 = {{3, 3, 6, 6}, {4, 4, 7, 7}};

    findOverlappingRectangles(rects1, rects2);

    return 0;
}



*/
