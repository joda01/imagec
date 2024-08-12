
#pragma once

#include <cmath>
#include <cstddef>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "roi.hpp"

namespace joda::cmd {
class ObjectsListMap;
}

namespace joda::roi {

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

class SpatialHash
{
public:
  /////////////////////////////////////////////////////
  explicit SpatialHash(int cellSize = 100) : mCellSize(cellSize)
  {
  }

  void emplace(const ROI &box)
  {
    insertIntoGrid(box);
  }

  void push_back(const ROI &box)
  {
    insertIntoGrid(box);
  }

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

  void cloneFromOther(const SpatialHash &);

  std::unique_ptr<SpatialHash> clone();

  vector<pair<ROI *, ROI *>> detect_collisions(const SpatialHash &other)
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

  void calcIntersections(const SpatialHash &other, SpatialHash &result,
                         const std::map<joda::enums::ChannelId, const cv::Mat *> &imageOriginal,
                         const std::optional<std::set<joda::enums::ClassId>> objectClassesMe,
                         const std::set<joda::enums::ClassId> &objectClassesOther,
                         joda::enums::ClassId objectClasIdOfIntersetingObject, float minIntersecion) const;

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
  /////////////////////////////////////////////////////
  std::list<ROI> mElements;
  unordered_map<pair<int, int>, std::vector<ROI *>, PairHash> grid;
  int mCellSize;

  void insertIntoGrid(const ROI &box)
  {
    auto &rect = box.getBoundingBox();
    int min_x  = rect.x;
    int min_y  = rect.y;
    int max_x  = rect.x + rect.width;
    int max_y  = rect.y + rect.height;

    std::lock_guard<std::mutex> lock(mInsertLock);
    mElements.emplace_back(box);
    ROI *boxPtr = &mElements.back();

    for(int x = min_x / mCellSize; x <= max_x / mCellSize; ++x) {
      for(int y = min_y / mCellSize; y <= max_y / mCellSize; ++y) {
        grid[{x, y}].emplace_back(boxPtr);
      }
    }
  }

  static bool isCollision(const ROI *box1, const ROI *box2)
  {
    auto &rect1 = box1->getBoundingBox();
    int min01_x = rect1.x;
    int min11_y = rect1.y;
    int max21_x = rect1.x + rect1.width;
    int max31_y = rect1.y + rect1.height;

    auto &rect2 = box2->getBoundingBox();
    int min02_x = rect2.x;
    int min12_y = rect2.y;
    int max22_x = rect2.x + rect2.width;
    int max32_y = rect2.y + rect2.height;

    return max21_x > min02_x && min01_x < max22_x && max31_y > min12_y && min11_y < max32_y;
  }

  std::mutex mInsertLock;
};
}    // namespace joda::roi
