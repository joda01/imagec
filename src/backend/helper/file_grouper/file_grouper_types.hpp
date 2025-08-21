///
/// \file      file_grouper_types.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-16
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
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
    char al = static_cast<char>(wellPosY - 1 + 'A');
    return std::string(1, al) + "x" + std::to_string(wellPosX);
  }
};

}    // namespace joda::grp
