
#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "../roi/roi.hpp"
#include "backend/commands/classification/reclassify/reclassify_settings.hpp"
#include "backend/enums/enums_classes.hpp"

namespace joda::atom {

class ObjectList;

using namespace std;

// Define a hash function for a pair of integers (x, y) to be used in the unordered_map

struct PairHash
{
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &p) const
  {
    auto hash1 = std::hash<T1>{}(p.first);
    auto hash2 = std::hash<T2>{}(p.second);
    return hash1 ^ hash2;
  }
};

class SpheralIndex
{
  friend class ObjectList;
  friend class SpheralIndexStandAlone;

public:
  /////////////////////////////////////////////////////

  explicit SpheralIndex(int cellSize = 100) : mCellSize(cellSize)
  {
  }

  void createBinaryImage(cv::Mat &img) const;

  bool empty() const
  {
    return grid.empty();
  }

  void clear()
  {
    grid.clear();
    mElements.clear();
  }

  size_t size() const
  {
    return mElements.size();
  }

  void cloneFromOther(const SpheralIndex &);

  std::unique_ptr<SpheralIndex> clone();

  vector<pair<ROI *, ROI *>> detect_collisions(const SpheralIndex &other)
  {
    vector<pair<ROI *, ROI *>> potential_collisions;

    // Check for collisions between objects in grid1 and grid2
    for(const auto &cell : grid) {
      const auto &boxes1 = cell.second;
      auto it            = other.grid.find(cell.first);
      if(it != other.grid.end()) {
        const auto &boxes2 = it->second;
        for(const auto &box1 : boxes1) {
          for(const auto &box2 : boxes2) {
            if(isCollision(box1, box2)) {
              potential_collisions.emplace_back(box1, box2);
            }
          }
        }
      }
    }

    return potential_collisions;
  }

  void calcColocalization(const enums::PlaneId &iterator, const SpheralIndex *other, SpheralIndex *result,
                          const std::optional<std::set<joda::enums::ClassId>> objectClassesMe,
                          const std::set<joda::enums::ClassId> &objectClassesOther,
                          joda::enums::ClassId objectClassIntersectingObjectsShouldBeAssignedTo, float minIntersecion, const enums::tile_t &tile,
                          const cv::Size &tileSize) const;

  void calcIntersection(ObjectList *objectList, joda::processor::ProcessContext &context, joda::settings::ReclassifySettings::Mode func,
                        joda::settings::ReclassifySettings::FilterLogic filterLogic,
                        joda::settings::ReclassifySettings::HierarchyHandling hierarchyMode, SpheralIndex *other,
                        const std::set<joda::enums::ClassId> objectClassesMe, const std::set<joda::enums::ClassId> objectClassesOther,
                        float minIntersecion, const settings::MetricsFilter &metrics, const settings::IntensityFilter &intensity,
                        joda::enums::ClassId newClassOFIntersectingObject = joda::enums::ClassId::NONE);

  auto begin() const
  {
    return mElements.begin();
  }

  auto end() const
  {
    return mElements.end();
  }

  auto begin()
  {
    return mElements.begin();
  }

  auto end()
  {
    return mElements.end();
  }

private:
  ROI &emplace(const ROI &box)
  {
    return insertIntoGrid(box);
  }

  ROI &push_back(const ROI &box)
  {
    return insertIntoGrid(box);
  }

  void erase(const ROI *eraseRoi)
  {
    mElements.remove_if([eraseRoi](const ROI &obj) { return &obj == eraseRoi; });
    for(auto &[_, vec] : grid) {
      int32_t vecSizeBefore = vec.size();
      vec.erase(std::remove_if(vec.begin(), vec.end(), [eraseRoi](const ROI *obj) { return obj == eraseRoi; }), vec.end());
      if(vec.size() < vecSizeBefore) {
        // We removed something
        if(vec.empty()) {
          // If the vector is empty, we can remove the grid element form the spheral index
          grid.erase(_);
        }
        break;
      }
    }
  }

  /////////////////////////////////////////////////////
  std::list<ROI> mElements;
  unordered_map<pair<int, int>, std::vector<ROI *>, PairHash> grid;
  int mCellSize;

  ROI &insertIntoGrid(const ROI &boxIn)
  {
    ROI cloned       = boxIn.clone();
    const auto &rect = cloned.getBoundingBoxTile();
    int min_x        = rect.x;
    int min_y        = rect.y;
    int max_x        = rect.x + rect.width;
    int max_y        = rect.y + rect.height;

    std::lock_guard<std::mutex> lock(mInsertLock);
    ROI &inserted = mElements.emplace_back(std::move(cloned));

    for(int x = min_x / mCellSize; x <= max_x / mCellSize; ++x) {
      for(int y = min_y / mCellSize; y <= max_y / mCellSize; ++y) {
        grid[{x, y}].emplace_back(&inserted);
      }
    }
    return inserted;
  }

  static bool isCollision(const ROI *box1, const ROI *box2)
  {
    auto &rect1 = box1->getBoundingBoxTile();
    int min01_x = rect1.x;
    int min11_y = rect1.y;
    int max21_x = rect1.x + rect1.width;
    int max31_y = rect1.y + rect1.height;

    auto &rect2 = box2->getBoundingBoxTile();
    int min02_x = rect2.x;
    int min12_y = rect2.y;
    int max22_x = rect2.x + rect2.width;
    int max32_y = rect2.y + rect2.height;

    return max21_x > min02_x && min01_x < max22_x && max31_y > min12_y && min11_y < max32_y;
  }

  std::mutex mInsertLock;
};

class SpheralIndexStandAlone : public SpheralIndex
{
public:
  using SpheralIndex::SpheralIndex;

  ROI &emplace(const ROI &box)
  {
    return SpheralIndex::insertIntoGrid(box);
  }

  ROI &push_back(const ROI &box)
  {
    return SpheralIndex::insertIntoGrid(box);
  }
};

class ObjectList : public std::map<enums::ClassId, std::unique_ptr<SpheralIndex>>
{
public:
  void push_back(const ROI &roi)
  {
    if(!contains(roi.getClassId())) {
      SpheralIndex idx{};
      operator[](roi.getClassId())->cloneFromOther(idx);
    }
    auto &inserted                              = at(roi.getClassId())->emplace(roi);
    objectsOrderedByObjectId[roi.getObjectId()] = &inserted;
  }

  void erase(const ROI *roi)
  {
    if(contains(roi->getClassId())) {
      at(roi->getClassId())->erase(roi);
    }
    objectsOrderedByObjectId.erase(roi->getObjectId());
  }

  void erase(enums::ClassId classToErase)
  {
    std::map<enums::ClassId, std::unique_ptr<SpheralIndex>>::erase(classToErase);

    for(auto it = objectsOrderedByObjectId.begin(); it != objectsOrderedByObjectId.end();) {
      if(it->second->getClassId() == classToErase) {
        it = objectsOrderedByObjectId.erase(it);    // erase returns the next valid iterator
      } else {
        ++it;
      }
    }
  }

  std::unique_ptr<SpheralIndex> &operator[](enums::ClassId classId)
  {
    if(!contains(classId)) {
      auto newS = std::make_unique<SpheralIndex>();
      emplace(classId, std::move(newS));
    }
    return at(classId);
  }

  const ROI *getObjectById(uint64_t objectId) const
  {
    return objectsOrderedByObjectId.at(objectId);
  }

  std::map<uint64_t, const ROI *> objectsOrderedByObjectId;
};

}    // namespace joda::atom
