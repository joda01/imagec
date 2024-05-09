///
/// \file      xz_wrapper.cpp
/// \author    Joachim Danmayr
/// \date      2024-05-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include <filesystem>
#include <string>
#include <vector>

namespace joda::helper::xz {

struct FolderToAdd
{
  std::filesystem::path pathToFolderToAdd;
  std::string fileExtensionToAdd;
  std::string subFolderInArchiveToAddTo;
};

extern int createAndAddFiles(const std::string &archiveFilename, const std::vector<FolderToAdd> &resultsfolder);
extern std::vector<std::filesystem::path> listFiles(const std::string &archiveFilename, const std::string &fileExt);
extern std::string readFile(const std::string &archiveFilename, const std::string &filename);

}    // namespace joda::helper::xz
