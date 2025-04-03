///
/// \file      well_position_generator.hpp
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

#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include "file_grouper_types.hpp"

namespace joda::grp {

class WellPosGenerator
{
public:
  GroupInformation getGroupId(const GroupInformation &groupInfo)
  {
    // This group still exists
    if(mGroups.contains(groupInfo.groupName)) {
      auto newPos = mGroups.at(groupInfo.groupName);
      if(groupInfo.imageIdx == UINT32_MAX) {
        newPos.imageIdx = nextFreeImgIdx();
      } else {
        newPos.imageIdx = groupInfo.imageIdx;
      }
      return newPos;
    }
    GroupInformation newPos;
    // This group does not yet exist
    if(groupInfo.wellPosX == UINT16_MAX || groupInfo.wellPosY == UINT16_MAX) {
      newPos = GroupInformation{.groupName = groupInfo.groupName,
                                .groupId   = actGroupId,
                                .wellPosX  = static_cast<uint8_t>((actWellPos % MAX_COLS) + 1),
                                .wellPosY  = static_cast<uint8_t>((actWellPos / MAX_COLS) + 1)};
    } else {
      newPos = GroupInformation{.groupName = groupInfo.groupName,
                                .groupId   = actGroupId,
                                .wellPosX  = groupInfo.wellPosX,
                                .wellPosY  = groupInfo.wellPosY};
    }

    if(groupInfo.imageIdx == UINT32_MAX) {
      newPos.imageIdx = nextFreeImgIdx();
    } else {
      newPos.imageIdx = groupInfo.imageIdx;
    }

    actWellPos++;
    actGroupId++;

    mGroups.emplace(groupInfo.groupName, newPos);

    return newPos;
  }

private:
  /////////////////////////////////////////////////////
  static const inline int32_t MAX_COLS = 24;
  static const inline int32_t MAX_ROWS = 16;

  uint32_t nextFreeImgIdx()
  {
    uint32_t toReturn = imgIdx;
    imgIdx++;
    return toReturn;
  }

  /////////////////////////////////////////////////////
  std::map<std::string, GroupInformation> mGroups;
  uint32_t actWellPos = 0;
  uint32_t imgIdx     = 1;
  uint16_t actGroupId = 0;
};

}    // namespace joda::grp
