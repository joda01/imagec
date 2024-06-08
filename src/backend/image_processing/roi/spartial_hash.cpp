

#include <memory>
#include "backend/image_processing/detection/detection_response.hpp"

namespace joda::image {

std::unique_ptr<joda::image::detect::DetectionResults>
SpatialHash::calcIntersections(const std::unique_ptr<detect::DetectionResults> &other,
                               const std::map<joda::settings::ChannelIndex, const cv::Mat *> &imageOriginal,
                               float minIntersecion)
{
  auto potential_collisions = std::make_unique<joda::image::detect::DetectionResults>();

  // Check for collisions between objects in grid1 and grid2
  for(const auto &cell : grid) {
    const auto &boxes1 = cell.second;
    auto it            = other->grid.find(cell.first);
    if(it != other->grid.end()) {
      const auto &boxes2 = it->second;
      for(const auto &box1 : boxes1) {
        for(const auto &box2 : boxes2) {
          if(isCollision(box1, box2)) {
            auto [colocROI, ok] = box1->calcIntersection(*box2, imageOriginal, minIntersecion);
            if(ok) {
              potential_collisions->push_back(colocROI);
              break;
            }
          }
        }
      }
    }
  }

  return potential_collisions;
}

std::unique_ptr<joda::image::detect::DetectionResults> SpatialHash::clone()
{
  std::unique_ptr<joda::image::detect::DetectionResults> clone =
      std::make_unique<joda::image::detect::DetectionResults>();

  std::map<const ROI *, ROI *> oldNewPtrMap;

  for(const auto &oldRoi : mElements) {
    clone->mElements.push_back(oldRoi);
    ROI *newPtr = &clone->mElements.back();
    oldNewPtrMap.emplace(&oldRoi, newPtr);
  }

  for(const auto &[key, valVec] : grid) {
    std::vector<ROI *> newVec;
    for(const auto &oldRoi : valVec) {
      newVec.emplace_back(oldNewPtrMap.at(oldRoi));
    }
    clone->grid.emplace(key, newVec);
  }

  return clone;
}

}    // namespace joda::image
