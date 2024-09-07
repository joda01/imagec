

#include "object_list.hpp"
#include <exception>
#include <list>
#include <memory>

namespace joda::atom {

void SpheralIndex::calcColocalization(const enums::PlaneId &iterator, const SpheralIndex *other, SpheralIndex *result,
                                      const std::optional<std::set<joda::enums::ClassId>> objectClassesMe,
                                      const std::set<joda::enums::ClassId> &objectClassesOther,
                                      joda::enums::ClusterId objectClusterIntersectingObjectsShouldBeAssignedTo,
                                      joda::enums::ClassId objectClassIntersectingObjectsShouldBeAssignedTo,
                                      uint32_t snapAreaOfIntersectingRoi, float minIntersecion,
                                      const enums::tile_t &tile, const cv::Size &tileSize) const
{
  std::set<ROI *> intersecting;

  // Check for collisions between objects in grid1 and grid2
  for(const auto &cell : grid) {
    const auto &boxes1 = cell.second;
    auto it            = other->grid.find(cell.first);
    if(it != other->grid.end()) {
      const auto &boxes2 = it->second;
      for(const auto &box1 : boxes1) {
        if((!objectClassesMe.has_value() || objectClassesMe->contains(box1->getClassId()))) {
          for(const auto &box2 : boxes2) {
            if(objectClassesOther.contains(box2->getClassId())) {
              // Each intersecting particle is only allowed to be counted once
              if(!intersecting.contains(box1) && !intersecting.contains(box2)) {
                if(isCollision(box1, box2)) {
                  auto colocROI =
                      box1->calcIntersection(iterator, *box2, snapAreaOfIntersectingRoi, minIntersecion, tile, tileSize,
                                             objectClusterIntersectingObjectsShouldBeAssignedTo,
                                             objectClassIntersectingObjectsShouldBeAssignedTo);
                  if(!colocROI.isNull()) {
                    result->push_back(std::move(colocROI));
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

void SpheralIndex::calcIntersections(joda::settings::IntersectionSettings::Function func, const SpheralIndex *other,
                                     const std::set<joda::enums::ClassId> objectClassesMe,
                                     const std::set<joda::enums::ClassId> &objectClassesOther, float minIntersecion,
                                     joda::enums::ClassId newClassOFIntersectingObject)
{
  std::set<ROI *> intersecting;
  // Check for collisions between objects in grid1 and grid2
  for(const auto &cell : grid) {
    const auto &boxes1 = cell.second;
    auto it            = other->grid.find(cell.first);
    if(it != other->grid.end()) {
      const auto &boxes2 = it->second;
      for(const auto &box1 : boxes1) {
        if((objectClassesMe.contains(box1->getClassId()))) {
          for(const auto &box2 : boxes2) {
            if(objectClassesOther.contains(box2->getClassId())) {
              // Each intersecting particle is only allowed to be counted once
              if(!intersecting.contains(box2)) {
                if(box1->isIntersecting(*box2, minIntersecion)) {
                  intersecting.emplace(box2);
                  switch(func) {
                    case settings::IntersectionSettings::Function::COUNT:
                      box1->addIntersectingRoi(box2);
                      break;
                    case settings::IntersectionSettings::Function::RECLASSIFY:
                      box2->setClass(newClassOFIntersectingObject);
                      break;
                    case settings::IntersectionSettings::Function::UNKNOWN:
                      break;
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

void SpheralIndex::createBinaryImage(cv::Mat &img, const std::set<joda::enums::ClassId> &objectClasses) const
{
  for(const auto &roi : *this) {
    if(objectClasses.contains(roi.getClassId())) {
      int left   = roi.getBoundingBox().x;
      int top    = roi.getBoundingBox().y;
      int width  = roi.getBoundingBox().width;
      int height = roi.getBoundingBox().height;

      if(!roi.getMask().empty() && !roi.getBoundingBox().empty() && roi.getBoundingBox().x >= 0 &&
         roi.getBoundingBox().y >= 0 && roi.getBoundingBox().width >= 0 && roi.getBoundingBox().height >= 0 &&
         roi.getBoundingBox().x + roi.getBoundingBox().width <= img.cols &&
         roi.getBoundingBox().y + roi.getBoundingBox().height <= img.rows) {
        try {
          img(roi.getBoundingBox()).setTo(cv::Scalar(UINT16_MAX), roi.getMask());
        } catch(const std::exception &ex) {
          std::cout << "PA: " << ex.what() << std::endl;
        }
      }
    }
  }
}

std::unique_ptr<SpheralIndex> SpheralIndex::clone()
{
  std::unique_ptr<SpheralIndex> clone = std::make_unique<SpheralIndex>();

  std::map<const ROI *, ROI *> oldNewPtrMap;

  for(const auto &oldRoi : mElements) {
    clone->mElements.push_back(std::move(oldRoi.clone()));
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

void SpheralIndex::cloneFromOther(const SpheralIndex &other)
{
  mElements.clear();
  grid.clear();
  mCellSize = other.mCellSize;

  std::map<const ROI *, ROI *> oldNewPtrMap;

  for(const auto &oldRoi : other.mElements) {
    mElements.push_back(std::move(oldRoi.clone()));
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

}    // namespace joda::atom
