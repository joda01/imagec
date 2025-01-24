

#include "object_list.hpp"
#include <exception>
#include <list>
#include <memory>
#include "backend/commands/classification/classifier_filter.hpp"

namespace joda::atom {

void SpheralIndex::calcColocalization(const enums::PlaneId &iterator, const SpheralIndex *other, SpheralIndex *result,
                                      const std::optional<std::set<joda::enums::ClassId>> objectClassesMe,
                                      const std::set<joda::enums::ClassId> &objectClassesOther,
                                      joda::enums::ClassId objectClassIntersectingObjectsShouldBeAssignedTo, float minIntersecion,
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
                      box1->calcIntersection(iterator, *box2, minIntersecion, tile, tileSize, objectClassIntersectingObjectsShouldBeAssignedTo);
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

void SpheralIndex::calcIntersection(ObjectList *objectList, joda::processor::ProcessContext &context, joda::settings::ReclassifySettings::Mode func,
                                    SpheralIndex *other, const std::set<joda::enums::ClassId> objectClassesMe,
                                    const std::set<joda::enums::ClassId> objectClassesOther, float minIntersecion,
                                    const settings::MetricsFilter &metrics, const settings::IntensityFilter &intensity,
                                    joda::enums::ClassId newClassOFIntersectingObject)
{
  std::set<ROI *> intersecting;
  std::set<ROI *> roisToRemove;

  // Check for collisions between objects in grid1 and grid2
  for(const auto &cell : grid) {
    const auto &boxes1 = cell.second;
    auto box2Iterator  = other->grid.find(cell.first);
    std::vector<ROI> roisToEnter;
    if(box2Iterator != other->grid.end()) {
      auto &boxes2 = box2Iterator->second;
      for(auto *box1 : boxes1) {
        if((objectClassesMe.contains(box1->getClassId()))) {
          for(auto *box2 : boxes2) {
            if(objectClassesOther.contains(box2->getClassId())) {
              // Each intersecting particle is only allowed to be counted once
              if(!intersecting.contains(box1) && !roisToRemove.contains(box1)) {
                if(box1->isIntersecting(*box2, minIntersecion)) {
                  intersecting.emplace(box1);
                  switch(func) {
                    case settings::ReclassifySettings::Mode::RECLASSIFY_MOVE:
                      if(settings::ClassifierFilter::doesFilterMatch(context, *box1, metrics, intensity)) {
                        // We have to reenter to organize correct in the map of objects
                        auto newRoi = box1->clone(newClassOFIntersectingObject);
                        roisToEnter.emplace_back(std::move(newRoi));
                        roisToRemove.emplace(box1);
                      }
                      break;
                    case settings::ReclassifySettings::Mode::RECLASSIFY_COPY: {
                      if(settings::ClassifierFilter::doesFilterMatch(context, *box1, metrics, intensity)) {
                        auto newRoi = box1->copy(newClassOFIntersectingObject);
                        roisToEnter.emplace_back(std::move(newRoi));    // Store the ROIs we want to enter
                      }
                    } break;
                    case settings::ReclassifySettings::Mode::UNKNOWN:
                      break;
                  }
                }
              }
            }
          }
        }
      }
    }
    // Enter the rois from the temp storage
    for(const auto &roi : roisToEnter) {
      objectList->push_back(roi);
    }
  }

  // Remove ROIs
  for(const auto *roi : roisToRemove) {
    this->erase(roi);
  }
}

void SpheralIndex::createBinaryImage(cv::Mat &img) const
{
  for(const auto &roi : *this) {
    int left   = roi.getBoundingBoxTile().x;
    int top    = roi.getBoundingBoxTile().y;
    int width  = roi.getBoundingBoxTile().width;
    int height = roi.getBoundingBoxTile().height;

    if(!roi.getMask().empty() && !roi.getBoundingBoxTile().empty() && roi.getBoundingBoxTile().x >= 0 && roi.getBoundingBoxTile().y >= 0 &&
       roi.getBoundingBoxTile().width >= 0 && roi.getBoundingBoxTile().height >= 0 &&
       roi.getBoundingBoxTile().x + roi.getBoundingBoxTile().width <= img.cols &&
       roi.getBoundingBoxTile().y + roi.getBoundingBoxTile().height <= img.rows) {
      try {
        img(roi.getBoundingBoxTile()).setTo(cv::Scalar(UINT16_MAX), roi.getMask());
      } catch(const std::exception &ex) {
        std::cout << "PA: " << ex.what() << std::endl;
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
