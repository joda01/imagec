///
/// \file      file_grouper.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-16
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "file_grouper.hpp"
#include <regex.h>
#include "backend/enums/enums_grouping.hpp"
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"

namespace joda::grp {

FileGrouper::FileGrouper(joda::enums::GroupBy groupBy, const std::string &fileRegex) :
    mGroupBy(groupBy), mFileRegex(fileRegex)
{
}

GroupInformation FileGrouper::getGroupForFilename(const std::filesystem::path &filePath)
{
  GroupInformation groupInfo;
  switch(mGroupBy) {
    case enums::GroupBy::OFF: {
      groupInfo.groupName = "";
      groupInfo.wellPosY  = UINT16_MAX;
      groupInfo.wellPosX  = UINT16_MAX;
      groupInfo.imageIdx  = UINT32_MAX;
    } break;
    case enums::GroupBy::DIRECTORY: {
      groupInfo.groupName = filePath.parent_path().string();
      groupInfo.wellPosY  = UINT16_MAX;
      groupInfo.wellPosX  = UINT16_MAX;
      groupInfo.imageIdx  = UINT32_MAX;
    } break;
    case enums::GroupBy::FILENAME: {
      groupInfo = applyRegex(mFileRegex, filePath);
    } break;
  }
  {
    std::lock_guard<std::mutex> lock(mWellGeneratorLock);
    return mWellPosGenerator.getGroupId(groupInfo);
  }
  return {};
}

///
/// \brief      Apply regex
/// \author     Joachim Danmayr
///
GroupInformation FileGrouper::applyRegex(const std::string &regex, const std::filesystem::path &imagePath)
{
  std::regex pattern(regex);
  std::smatch match;
  GroupInformation result;

  std::string fileName = imagePath.filename().string();
  if(std::regex_search(fileName, match, pattern)) {
    if(match.size() >= 5) {
      result.groupName = match[1].str();
      result.wellPosY  = helper::stringToNumber(match[2].str());
      result.wellPosX  = helper::stringToNumber(match[3].str());
      result.imageIdx  = helper::stringToNumber(match[4].str());
    } else if(match.size() >= 3) {
      result.groupName = match[1].str();
      result.wellPosY  = UINT16_MAX;
      result.wellPosX  = UINT16_MAX;
      result.imageIdx  = helper::stringToNumber(match[2].str());
    } else if(match.size() >= 2) {
      result.groupName = match[1].str();
      result.wellPosY  = UINT16_MAX;
      result.wellPosX  = UINT16_MAX;
      result.imageIdx  = UINT32_MAX;
    } else {
      result.groupName = "";
      result.wellPosY  = UINT16_MAX;
      result.wellPosX  = UINT16_MAX;
      result.imageIdx  = UINT32_MAX;
      joda::log::logWarning("Regex pattern not found. Use fallback!");
    }
  } else {
    result.groupName = "";
    result.wellPosY  = UINT16_MAX;
    result.wellPosX  = UINT16_MAX;
    result.imageIdx  = UINT32_MAX;
    joda::log::logWarning("Regex pattern not found. Use fallback!");
  }
  return result;
}

}    // namespace joda::grp
