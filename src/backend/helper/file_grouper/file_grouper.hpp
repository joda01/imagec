///
/// \file      file_grouper.hpp
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
#include <filesystem>
#include <map>
#include <mutex>
#include <string>
#include "backend/enums/enums_grouping.hpp"
#include "file_grouper_types.hpp"
#include "well_position_generator.hpp"

namespace joda::grp {

///
/// \class      FileGrouper
/// \author     Joachim Danmayr
/// \brief      Assign file to a group based on grouping settings
///
class FileGrouper
{
public:
  /////////////////////////////////////////////////////
  FileGrouper(joda::enums::GroupBy groupBy, const std::string &fileRegex);
  GroupInformation getGroupForFilename(const std::filesystem::path &filePath);
  static GroupInformation applyRegex(const std::string &regex, const std::filesystem::path &imagePath);

private:
  //// SETTINGS /////////////////////////////////////////////////
  const joda::enums::GroupBy mGroupBy;
  const std::string mFileRegex;

  /////////////////////////////////////////////////////
  WellPosGenerator mWellPosGenerator;
  mutable std::mutex mWellGeneratorLock;
};

}    // namespace joda::grp
