///
/// \file      archive_reader.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <zip.h>
#include <filesystem>
#include <memory>
#include <set>
#include <thread>
#include <vector>

namespace joda::helper::xz {

struct FolderToAdd
{
  std::filesystem::path pathToFolderToAdd;
  std::string fileExtensionToAdd;
  std::string subFolderInArchiveToAddTo;
};

class Archive
{
public:
  /////////////////////////////////////////////////////
  Archive(const std::filesystem::path &pathToArchive);
  ~Archive();
  void waitForFinishd();
  void open();
  void close();
  [[nodiscard]] std::string readFile(const std::filesystem::path &filename) const;
  static int createAndAddFiles(const std::string &archiveFilename, const std::vector<FolderToAdd> &resultsfolder);
  auto getFoundResults() const -> const std::set<std::filesystem::path> &
  {
    return mResultsEntries;
  }
  auto getFoundImages() const -> const std::set<std::filesystem::path> &
  {
    return mImageEntries;
  }

private:
  /////////////////////////////////////////////////////
  void listFiles();

  /////////////////////////////////////////////////////
  bool mStop = false;
  std::filesystem::path mArchivePath;
  std::set<std::filesystem::path> mResultsEntries;
  std::set<std::filesystem::path> mImageEntries;
  std::shared_ptr<std::thread> mListThread;
  zip_t *mArchive = nullptr;
};

}    // namespace joda::helper::xz
