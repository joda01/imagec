///
/// \file      tools.hpp
/// \author    Joachim Danmayr
/// \date      2023-02-20
/// \brief     C++ implementation of tools lib ported from imageJ
///            https://github.com/imagej/ImageJ/blob/master/ij/util/Tools.java
///

#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>

namespace joda::image::func {

///
/// \brief       Returns a sorted list of indices of the specified double array.
///              Modified from: http://stackoverflow.com/questions/951848 by N.Vischer.
///
inline std::shared_ptr<int> rank(std::shared_ptr<double> values, size_t length)
{
  int n = length;
  std::shared_ptr<int> indexes(new int[n]{0}, [](int *p) { delete[] p; });
  std::shared_ptr<double> data(new double[n]{0}, [](double *p) { delete[] p; });
  for(int i = 0; i < n; i++) {
    indexes.get()[i] = i;
    data.get()[i]    = values.get()[i];
  }
  std::sort(indexes.get(), indexes.get() + n, [&data](int o1, int o2) { return data.get()[o1] < data.get()[o2]; });

  std::shared_ptr<int> indexes2(new int[n]{0}, [](int *p) { delete[] p; });
  for(int i = 0; i < n; i++) {
    indexes2.get()[i] = indexes.get()[i];
  }
  return indexes2;
}

}    // namespace joda::image::func
