///
/// \file      file_grouper_types.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-16
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <cstdint>
#include <string>

namespace joda::grp {

struct GroupInformation
{
  std::string groupName;
  uint16_t groupId  = 0;
  uint32_t imageIdx = 0;
  uint16_t wellPosX = UINT16_MAX;
  uint16_t wellPosY = UINT16_MAX;

  [[nodiscard]] std::string toString() const
  {
    char al = (wellPosY - 1 + 'A');
    return std::string(1, al) + "x" + std::to_string(wellPosX);
  }
};

}    // namespace joda::grp
