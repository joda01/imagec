#pragma once

#include <map>

namespace joda::helper {

template <typename KEY_T, typename VALUE_T>
class TwoWayMap
{
public:
  TwoWayMap(const std::map<KEY_T, VALUE_T> &in)
  {
    for(const auto &[key, val] : in) {
      Insert(key, val);
    }
  }

  void Insert(const KEY_T &key, const VALUE_T &val)
  {
    map1[key] = val;
    map2[val] = key;
  }

  VALUE_T operator[](const KEY_T &key)
  {
    return map1[key];
  }

  KEY_T operator[](const VALUE_T &key)
  {
    return map2[key];
  }

private:
  std::map<KEY_T, VALUE_T> map1;
  std::map<VALUE_T, KEY_T> map2;
};
}    // namespace joda::helper
