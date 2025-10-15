

#include "object_list.hpp"
#include <exception>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/commands/classification/classifier_filter.hpp"
#include "backend/helper/cereal_cv_mat.hpp"
#include <cereal/archives/binary.hpp>

namespace joda::atom {

void SpheralIndex::calcColocalization(const enums::PlaneId &iterator, const SpheralIndex *other, SpheralIndex *result,
                                      const std::optional<std::set<joda::enums::ClassId>> objectClassesMe,
                                      const std::set<joda::enums::ClassId> &objectClassesOther,
                                      joda::enums::ClassId objectClassIntersectingObjectsShouldBeAssignedTo, float minIntersecion,
                                      const enums::tile_t &tile, const cv::Size &tileSize) const
{
  std::set<uint64_t> intersecting;
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
              if(!intersecting.contains(box1->getObjectId()) && !intersecting.contains(box2->getObjectId())) {
                if(isCollision(box1, box2)) {
                  auto colocROI =
                      box1->calcIntersection(iterator, *box2, minIntersecion, {tile, tileSize}, objectClassIntersectingObjectsShouldBeAssignedTo);
                  if(!colocROI.isNull()) {
                    intersecting.emplace(box1->getObjectId());
                    intersecting.emplace(box2->getObjectId());
                    colocROI.addLinkedRoi(box1);
                    colocROI.addLinkedRoi(box2);
                    // Keep the links from a possible old round
                    colocROI.addLinkedRoi(box1->getLinkedRois());
                    colocROI.addLinkedRoi(box2->getLinkedRois());
                    bool inserRet = false;
                    result->push_back(std::move(colocROI), inserRet);
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
                                    joda::settings::ReclassifySettings::FilterLogic filterLogic,
                                    joda::settings::ReclassifySettings::HierarchyHandling hierarchyMode, SpheralIndex *other,
                                    const std::set<joda::enums::ClassId> objectClassesMe, const std::set<joda::enums::ClassId> objectClassesOther,
                                    float minIntersecion, const settings::MetricsFilter &metrics, const settings::IntensityFilter &intensity,
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
        auto applyCopyOrMove = [&](uint64_t box2ObjectId) {
          intersecting.emplace(box1);
          uint64_t parentObjectId = 0;
          switch(hierarchyMode) {
            case settings::ReclassifySettings::HierarchyHandling::CREATE_TREE:
              parentObjectId = box2ObjectId;
              break;
            case settings::ReclassifySettings::HierarchyHandling::KEEP_EXISTING:
              parentObjectId = box1->getParentObjectId();
              break;
            case settings::ReclassifySettings::HierarchyHandling::REMOVE:
              parentObjectId = 0;
              break;
          }

          switch(func) {
            case settings::ReclassifySettings::Mode::RECLASSIFY_MOVE:
              if(settings::ClassifierFilter::doesFilterMatch(context, *box1, metrics, intensity)) {
                // We have to reenter to organize correct in the map of objects
                auto newRoi = box1->clone(newClassOFIntersectingObject, parentObjectId);
                roisToEnter.emplace_back(std::move(newRoi));
                roisToRemove.emplace(box1);
              }
              break;
            case settings::ReclassifySettings::Mode::RECLASSIFY_COPY: {
              if(settings::ClassifierFilter::doesFilterMatch(context, *box1, metrics, intensity)) {
                auto newRoi = box1->copy(newClassOFIntersectingObject, parentObjectId);
                roisToEnter.emplace_back(std::move(newRoi));    // Store the ROIs we want to enter
              }
            } break;
            case settings::ReclassifySettings::Mode::UNKNOWN:
              break;
          }
        };
        bool isIntersecting = false;
        if((objectClassesMe.contains(box1->getClassId()))) {
          for(auto *box2 : boxes2) {
            if(objectClassesOther.contains(box2->getClassId())) {
              isIntersecting = box1->isIntersecting(*box2, minIntersecion);
              if(isIntersecting && filterLogic == joda::settings::ReclassifySettings::FilterLogic::APPLY_IF_NOT_MATCH) {
                break;
              }
              // Each intersecting particle is only allowed to be counted once
              if(!intersecting.contains(box1) && !roisToRemove.contains(box1)) {
                if(isIntersecting && filterLogic == joda::settings::ReclassifySettings::FilterLogic::APPLY_IF_MATCH) {
                  applyCopyOrMove(box2->getObjectId());
                  // break;
                }
              }
            }
          }
        }

        // If not intersecing do something
        if(!isIntersecting && filterLogic == joda::settings::ReclassifySettings::FilterLogic::APPLY_IF_NOT_MATCH) {
          // Create tree is not possible for not interecting
          applyCopyOrMove(box1->getParentObjectId());
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

int64_t SpheralIndex::createBinaryImage(cv::Mat &img, uint16_t pixelClass, ROI::Category categoryFilter, const joda::enums::TileInfo &tileInfo) const
{
  int64_t addedRois = 0;
  for(const auto &roi : *this) {
    if(categoryFilter != ROI::Category::ANY && roi.getCategory() != categoryFilter) {
      continue;
    }

    //   int left   = roi.getBoundingBoxTile().x;
    //   int top    = roi.getBoundingBoxTile().y,,;
    //   int width  = roi.getBoundingBoxTile().width;
    //   int height = roi.getBoundingBoxTile().height;

    const auto &boundigBox = roi.getBoundingBoxTile(tileInfo);
    if(!roi.getMask().empty() && !boundigBox.empty() && boundigBox.x >= 0 && boundigBox.y >= 0 && boundigBox.width >= 0 && boundigBox.height >= 0 &&
       boundigBox.x + boundigBox.width <= img.cols && boundigBox.y + boundigBox.height <= img.rows) {
      try {
        img(boundigBox).setTo(cv::Scalar(pixelClass), roi.getMask());
        addedRois++;
      } catch(const std::exception &ex) {
        std::cout << "PA: " << ex.what() << std::endl;
      }
    }
  }
  return addedRois;
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

/////////////////////////////////////////////////////

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void ObjectList::push_back(const ROI &roi)
{
  if(!contains(roi.getClassId())) {
    SpheralIndex idx{};
    operator[](roi.getClassId())->cloneFromOther(idx);
  }
  bool insertedRet = false;
  auto &inserted   = at(roi.getClassId())->emplace(roi, insertedRet);
  if(insertedRet) {
    if(0 != inserted.getObjectId()) {
      std::lock_guard<std::mutex> lock(mInsertLock);
      objectsOrderedByObjectId[inserted.getObjectId()] = &inserted;
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
void ObjectList::erase(const ROI *roi)
{
  {
    std::lock_guard<std::mutex> lock(mInsertLock);
    objectsOrderedByObjectId.erase(roi->getObjectId());
  }

  if(contains(roi->getClassId())) {
    at(roi->getClassId())->erase(roi);
    if(at(roi->getClassId())->empty()) {
      ObjectMap::erase(roi->getClassId());
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
void ObjectList::erase(enums::ClassId classToErase)
{
  {
    std::lock_guard<std::mutex> lock(mInsertLock);
    for(auto it = objectsOrderedByObjectId.begin(); it != objectsOrderedByObjectId.end();) {
      if((it->second != nullptr) && it->second->getClassId() == classToErase) {
        it = objectsOrderedByObjectId.erase(it);    // erase returns the next valid iterator
      } else {
        ++it;
      }
    }
  }

  ObjectMap::erase(classToErase);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void ObjectList::erase(joda::atom::ROI::Category categoryToErase)
{
  std::vector<ROI *> toErase;
  for(const auto &[classId, rois] : *this) {
    for(auto &roi : *rois) {
      if(roi.getCategory() == categoryToErase) {
        toErase.emplace_back(&roi);
      }
    }
  }

  for(auto *roi : toErase) {
    erase(roi);
  }
}
///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void ObjectList::clearAll()
{
  // triggerStartChangeCallback();
  std::lock_guard<std::mutex> lock(mInsertLock);
  std::map<enums::ClassId, std::unique_ptr<SpheralIndex>>::clear();
  objectsOrderedByObjectId.clear();
  // triggerChangeCallback();
}

void ObjectList::erase(const std::set<ROI *> &rois)
{
  triggerStartChangeCallback();
  for(const joda::atom::ROI *roi : rois) {
    erase(roi);
  }
  triggerChangeCallback();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
std::unique_ptr<SpheralIndex> &ObjectList::operator[](enums::ClassId classId)
{
  if(!contains(classId)) {
    auto newS = std::make_unique<SpheralIndex>();
    ObjectMap::emplace(classId, std::move(newS));
  }
  return at(classId);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] const ROI *ObjectList::getObjectById(uint64_t objectId) const
{
  std::lock_guard<std::mutex> lock(mInsertLock);
  return objectsOrderedByObjectId.at(objectId);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] bool ObjectList::containsObjectById(uint64_t objectId) const
{
  std::lock_guard<std::mutex> lock(mInsertLock);
  return objectsOrderedByObjectId.contains(objectId);
}

[[nodiscard]] size_t ObjectList::sizeList() const
{
  std::lock_guard<std::mutex> lock(mInsertLock);
  return objectsOrderedByObjectId.size();
}

[[nodiscard]] size_t ObjectList::sizeClasses() const
{
  return ObjectMap::size();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void ObjectList::serialize(const std::filesystem::path &filename)
{
  std::ofstream os(filename.string(), std::ios::binary);
  cereal::BinaryOutputArchive archive(os);

  // Save number of entries first (so we know how many to read back)
  size_t count = objectsOrderedByObjectId.size();
  archive(count);

  // Write each pointed object (not the key)
  for(const auto &[_, ptr] : objectsOrderedByObjectId) {
    archive(*ptr);    // directly serialize the object
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void ObjectList::deserialize(const std::filesystem::path &filename)
{
  clearAll();
  std::ifstream is(filename.string(), std::ios::binary);
  cereal::BinaryInputArchive archive(is);

  size_t count;
  archive(count);

  for(size_t i = 0; i < count; ++i) {
    ROI roi;
    archive(roi);    // directly deserialize into allocated object
    push_back(roi);
  }
}

}    // namespace joda::atom
