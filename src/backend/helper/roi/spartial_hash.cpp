

#include "spartial_hash.hpp"
#include <memory>

namespace joda::image {

std::unique_ptr<joda::cmd::ObjectsList>
SpatialHash::calcIntersections(const std::unique_ptr<SpatialHash> &other,
                               const std::map<joda::settings::ImageChannelIndex, const cv::Mat *> &imageOriginal,
                               float minIntersecion)
{
  auto potential_collisions = std::make_unique<joda::cmd::ObjectsList>();
  std::set<ROI *> intersecting;

  // Check for collisions between objects in grid1 and grid2
  for(const auto &cell : grid) {
    const auto &boxes1 = cell.second;
    auto it            = other->grid.find(cell.first);
    if(it != other->grid.end()) {
      const auto &boxes2 = it->second;
      for(const auto &box1 : boxes1) {
        if(box1->isValid()) {
          for(const auto &box2 : boxes2) {
            if(box2->isValid()) {
              // Each intersecting particle is only allowed to be counted once
              if(!intersecting.contains(box1) && !intersecting.contains(box2)) {
                if(isCollision(box1, box2)) {
                  auto [colocROI, ok] = box1->calcIntersection(*box2, imageOriginal, minIntersecion);
                  if(ok) {
#warning "Fix it"
                    // potential_collisions->push_back(colocROI);
                    intersecting.emplace(box1);
                    intersecting.emplace(box2);
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  return potential_collisions;
}

std::unique_ptr<SpatialHash> SpatialHash::clone()
{
  std::unique_ptr<SpatialHash> clone = std::make_unique<SpatialHash>();

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