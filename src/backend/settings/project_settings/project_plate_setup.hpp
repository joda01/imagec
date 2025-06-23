
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
}    // namespace joda::settings
