///
/// \file      archive_reader.cpp
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

#include "archive_reader.hpp"
#include <zip.h>
#include <filesystem>
#include <memory>
#include <thread>
#include "backend/results/results.hpp"

namespace joda::helper::xz {

void checkForError(const std::string &what)
{
  throw std::runtime_error(what);
}

Archive::Archive(const std::filesystem::path &pathToArchive) : mArchivePath(pathToArchive)
{
  mListThread = std::make_shared<std::thread>(&Archive::listFiles, this);
}

Archive::~Archive()
{
  waitForFinishd();
}

void Archive::waitForFinishd()
{
  if(mListThread != nullptr && mListThread->joinable()) {
    mListThread->join();
  }
}

void Archive::listFiles()
{
  // Open the ZIP archive
  struct zip *archive = zip_open(mArchivePath.string().data(), ZIP_CHECKCONS, nullptr);
  if(!archive) {
    checkForError("Could not open zip file!");
  }

  // Get the number of entries (files) in the archive
  int num_entries = zip_get_num_entries(archive, 0);
  if(num_entries < 0) {
    auto err = "Failed to get number of entries in archive: " + std::string(zip_strerror(archive));
    zip_close(archive);
    checkForError(err);
  }

  // Iterate over each file in the archive and print its name
  for(int i = 0; i < num_entries; ++i) {
    struct zip_stat st;
    if(zip_stat_index(archive, i, 0, &st) != 0) {
      auto err = "Failed to get file info for entry " + std::to_string(i) + ": " + zip_strerror(archive);
      zip_close(archive);
      checkForError(err);
    }
    auto fileName = std::filesystem::path(st.name);
    if(fileName.extension().string() == results::MESSAGE_PACK_FILE_EXTENSION) {
      mResultsEntries.emplace(fileName);
    } else if(fileName.extension().string() == results::CONTROL_IMAGES_FILE_EXTENSION) {
      mImageEntries.emplace(fileName);
    }
  }
  // Close the archive
  zip_close(archive);
}

std::string Archive::readFile(const std::filesystem::path &filename) const
{
  // Open the ZIP archive
  struct zip *archive = zip_open(mArchivePath.string().data(), ZIP_CHECKCONS | ZIP_CREATE, nullptr);
  if(!archive) {
    checkForError("Could not open zip file!");
  }

  // Locate the file within the archive
  struct zip_stat st;
  zip_stat_init(&st);
  if(zip_stat(archive, filename.string().data(), 0, &st) != 0) {
    auto err = "Failed to locate file in archive: " + std::string(zip_strerror(archive));
    zip_close(archive);
    checkForError(err);
  }

  // Read the contents of the file
  struct zip_file *file = zip_fopen(archive, filename.string().data(), 0);
  if(!file) {
    auto err = "Failed to open file in archive: " + std::string(zip_strerror(archive));
    zip_close(archive);
    checkForError(err);
  }

  std::string contentRead;
  contentRead.resize(st.size);
  zip_fread(file, contentRead.data(), contentRead.size());
  zip_fclose(file);

  // Close the archive
  zip_close(archive);
  return contentRead;
}

// Function to create an XZ archive and add files to it
int Archive::createAndAddFiles(const std::string &archiveFilename, const std::vector<FolderToAdd> &resultsfolder)
{
  // Create a new ZIP archive
  struct zip *archive = zip_open(archiveFilename.data(), ZIP_CREATE | ZIP_TRUNCATE, nullptr);
  if(!archive) {
    checkForError("Could not open zip file!");
  }

  // Add each file to the ZIP archive
  for(const auto &toAdd : resultsfolder) {
    for(const auto &fileEntry : std::filesystem::recursive_directory_iterator(toAdd.pathToFolderToAdd)) {
      if(fileEntry.is_regular_file() && fileEntry.path().extension() == toAdd.fileExtensionToAdd) {
        // Open the file

        struct zip_source *source = zip_source_file(archive, fileEntry.path().string().data(), 0, 0);
        if(!source) {
          auto err =
              "Failed to open file '" + fileEntry.path().string() + "' for adding to archive: " + zip_strerror(archive);
          zip_close(archive);
          checkForError(err);
        }

        // Add the file to the archive
        std::string pathInArchive = toAdd.subFolderInArchiveToAddTo + "/" +
                                    std::filesystem::relative(fileEntry.path(), toAdd.pathToFolderToAdd).string();
        int64_t error = zip_file_add(archive, pathInArchive.data(), source, ZIP_FL_OVERWRITE);
        if(error < 0) {
          auto err = "Failed to add file '" + pathInArchive + "' to archive: " + zip_strerror(archive);
          zip_source_free(source);
          zip_close(archive);
          checkForError(err);
        }
      }
    }
  }

  // Close the archive
  zip_close(archive);

  return 0;
}

}    // namespace joda::helper::xz
