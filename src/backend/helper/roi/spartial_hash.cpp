

#include "spartial_hash.hpp"
#include <memory>
#include "backend/commands/objects_list.hpp"

namespace joda::roi {

void SpatialHash::calcIntersections(const SpatialHash &other, SpatialHash &result,
                                    const std::map<joda::enums::ClusterId, const cv::Mat *> &imageOriginal,
                                    const std::optional<std::set<joda::enums::ClassId>> objectClassesMe,
                                    const std::set<joda::enums::ClassId> &objectClassesOther,
                                    joda::enums::ClassId objectClasIdOfIntersetingObject, float minIntersecion) const
{
  std::set<ROI *> intersecting;

  // Check for collisions between objects in grid1 and grid2
  for(const auto &cell : grid) {
    const auto &boxes1 = cell.second;
    auto it            = other.grid.find(cell.first);
    if(it != other.grid.end()) {
      const auto &boxes2 = it->second;
      for(const auto &box1 : boxes1) {
        if(box1->isValid() && (!objectClassesMe.has_value() || objectClassesMe->contains(box1->getClassId()))) {
          for(const auto &box2 : boxes2) {
            if(box2->isValid() && objectClassesOther.contains(box2->getClassId())) {
              // Each intersecting particle is only allowed to be counted once
              if(!intersecting.contains(box1) && !intersecting.contains(box2)) {
                if(isCollision(box1, box2)) {
                  auto [colocROI, ok] =
                      box1->calcIntersection(*box2, imageOriginal, minIntersecion, objectClasIdOfIntersetingObject);
                  if(ok) {
                    result.push_back(colocROI);
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

void SpatialHash::cloneFromOther(const SpatialHash &other)
{
  mElements.clear();
  grid.clear();
  mCellSize = other.mCellSize;

  std::map<const ROI *, ROI *> oldNewPtrMap;

  for(const auto &oldRoi : other.mElements) {
    mElements.push_back(oldRoi);
    ROI *newPtr = &mElements.back();
    oldNewPtrMap.emplace(&oldRoi, newPtr);
  }

  for(const auto &[key, valVec] : other.grid) {
    std::vector<ROI *> newVec;
    for(const auto &oldRoi : valVec) {
      newVec.emplace_back(oldNewPtrMap.at(oldRoi));
    }
    grid.emplace(key, newVec);
  }
}

}    // namespace joda::roi
