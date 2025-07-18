
///
/// \file      project_plate_setup.hpp
/// \author    Joachim Danmayr
/// \date      2024-04-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///

#pragma once

#include <set>
#include <string>
#include <vector>
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct PlateSetup
{
  //
  // Number of rows the plate has
  //
  int32_t rows = 0;

  //
  // Number of cols the plate has
  //
  int32_t cols = 0;

  //
  // Matrix of image numbers how the images are ordered in a map.
  // First dimension of the vector are the rows, second the columns
  //
  std::vector<std::vector<int32_t>> wellImageOrder = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};

  std::pair<int32_t, int32_t> getRowsAndColsOfWell() const
  {
    int32_t sizeY = wellImageOrder.size();
    int32_t sizeX = 0;

    for(int y = 0; y < wellImageOrder.size(); y++) {
      for(int x = 0; x < wellImageOrder[y].size(); x++) {
        auto imgNr = wellImageOrder[y][x];
        if(x > sizeX) {
          sizeX = x;
        }
      }
    }
    sizeX++;    // Because we start with zro to count

    return {sizeY, sizeX};
  }

  void check() const
  {
  }
  // We don't want to do a error check for the history
  void getErrorLogRecursive(SettingParserLog_t &settingsParserLog) const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PlateSetup, rows, cols, wellImageOrder);
};

///
/// \brief      Transforms a 2D Matrix where the elements in the matrix represents an images index
///             and the coordinates of the matrix the position on the well to a map
///             whereby the key is the images index and the values are the coordinates
///              | 0  1  2
///             -|---------
///             0| 1  2  3
///             1| 4  5  6
///             2| 7  8  9
///
///            [1] => {0,0}
///            [2] => {1,0}
///            ...
///            [9] => {2,2}
///
///
/// \author     Joachim Danmayr
///
///

struct ImgPositionInWell
{
  int32_t img = -1;
  int32_t x   = -1;
  int32_t y   = -1;
};

inline auto transformMatrix(const std::vector<std::vector<int32_t>> &wellImageOrder, int32_t &sizeX, int32_t &sizeY)
    -> std::map<int32_t, ImgPositionInWell>
{
  sizeY = wellImageOrder.size();
  sizeX = 0;

  std::map<int32_t, ImgPositionInWell> ret;
  for(int y = 0; y < wellImageOrder.size(); y++) {
    for(int x = 0; x < wellImageOrder[y].size(); x++) {
      auto imgNr = wellImageOrder[y][x];
      ret[imgNr] = ImgPositionInWell{.img = imgNr, .x = x + 1, .y = y + 1};    // We start with 1 not with zero
      if(x > sizeX) {
        sizeX = x;
      }
    }
  }
  sizeX++;    // Because we start with zro to count
  return ret;
}

}    // namespace joda::settings
