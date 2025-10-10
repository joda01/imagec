
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

  explicit SpheralIndex(bool allowNoneValue = false, int cellSize = 100) : mCellSize(cellSize), mAllowNonValues(allowNoneValue)
  {
  }

  int64_t createBinaryImage(cv::Mat &img, uint16_t pixelClass, ROI::Category categoryFilter) const;

  bool empty() const
  {
    return mElements.empty();
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
  ROI &emplace(const ROI &box, bool &insertedRet)
  {
    return insertIntoGrid(box, insertedRet);
  }

  ROI &push_back(const ROI &box, bool &insertedRet)
  {
    return insertIntoGrid(box, insertedRet);
  }

  void erase(const ROI *eraseRoi)
  {
    for(auto &[_, vec] : grid) {
      auto vecSizeBefore = vec.size();
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
    mElements.remove_if([eraseRoi](const ROI &obj) { return &obj == eraseRoi; });
  }

  /////////////////////////////////////////////////////
  std::list<ROI> mElements;
  unordered_map<pair<int, int>, std::vector<ROI *>, PairHash> grid;
  int mCellSize;

  ROI &insertIntoGrid(const ROI &boxIn, bool &insertedRet)
  {
    // If class id is none, do not enter the ROI
    // if(!mAllowNonValues && boxIn.getClassId() == enums::ClassId::NONE) {
    //  insertedRet = false;
    //  return boxIn;
    //}
    /// \todo generate an object ID

    //
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
    insertedRet = true;
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
  bool mAllowNonValues = false;
};

class SpheralIndexStandAlone : public SpheralIndex
{
public:
  using SpheralIndex::SpheralIndex;

  ROI &emplace(const ROI &box, bool &insertedRet)
  {
    return SpheralIndex::insertIntoGrid(box, insertedRet);
  }

  ROI &push_back(const ROI &box, bool &insertedRet)
  {
    return SpheralIndex::insertIntoGrid(box, insertedRet);
  }

  ROI &emplace(const ROI &box)
  {
    bool insertedRet = false;
    return SpheralIndex::insertIntoGrid(box, insertedRet);
  }

  ROI &push_back(const ROI &box)
  {
    bool insertedRet = false;
    return SpheralIndex::insertIntoGrid(box, insertedRet);
  }
};

using ObjectMap = std::map<enums::ClassId, std::unique_ptr<SpheralIndex>>;

class ObjectList : public ObjectMap
{
public:
  /////////////////////////////////////////////////////
  ObjectList()                                       = default;
  ObjectList(ObjectList &)                           = delete;
  ObjectList(ObjectList &&other) noexcept            = delete;
  ObjectList &operator=(ObjectList &&other) noexcept = delete;
  const ROI &emplace(const ROI &box)                 = delete;

  void push_back(const ROI &roi);
  void erase(const ROI *roi);
  void erase(const std::set<ROI *> &roi);
  void erase(enums::ClassId classToErase);
  void erase(joda::atom::ROI::Category categoryToErase);
  virtual std::unique_ptr<SpheralIndex> &operator[](enums::ClassId classId);
  [[nodiscard]] const ROI *getObjectById(uint64_t objectId) const;
  [[nodiscard]] bool containsObjectById(uint64_t objectId) const;
  [[nodiscard]] size_t sizeList() const;
  [[nodiscard]] size_t sizeClasses() const;
  auto getObjectList() -> const std::map<uint64_t, ROI *> *
  {
    return &objectsOrderedByObjectId;
  }
  void registerOnChangeCallback(const std::function<void()> &cb)
  {
    mChangeCallback.push_back(cb);
  }
  void registerOnStartChangeCallback(const std::function<void()> &cb)
  {
    mStartChangeCallback.push_back(cb);
  }
  void triggerChangeCallback() const
  {
    for(const auto &cb : mChangeCallback) {
      cb();
    }
  }

  void triggerStartChangeCallback() const
  {
    for(const auto &cb : mStartChangeCallback) {
      cb();
    }
  }

private:
  /////////////////////////////////////////////////////
  std::map<uint64_t, ROI *> objectsOrderedByObjectId;
  mutable std::mutex mInsertLock;
  std::vector<std::function<void()>> mChangeCallback;
  std::vector<std::function<void()>> mStartChangeCallback;
};

/*
class ObjectList : public ObjectList
{
public:
  using ObjectList::ObjectList;

  std::unique_ptr<SpheralIndex> &operator[](enums::ClassId classId) override
  {
    if(!contains(classId)) {
      auto newS = std::make_unique<SpheralIndex>(true);
      emplace(classId, std::move(newS));
    }
    return at(classId);
  }
};
*/
}    // namespace joda::atom
