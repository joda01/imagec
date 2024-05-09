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

#include "xz_wrapper.hpp"
#include <archive.h>
#include <archive_entry.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

namespace joda::helper::xz {

std::mutex mArchivemutex;
namespace fs = std::filesystem;

void checkForError(int retVal)
{
  if(retVal != ARCHIVE_OK) {
    throw std::runtime_error("Error generating archive!");
  }
}

// Function to create an XZ archive and add files to it
int createAndAddFiles(const std::string &archiveFilename, const std::vector<FolderToAdd> &resultsfolder)
{
  std::lock_guard<std::mutex> lock(mArchivemutex);
  char buff[8192]{0};

  // Open the archive for writing
  archive *archive = archive_write_new();
  checkForError(archive_write_add_filter_xz(archive));
  checkForError(archive_write_set_options(archive, "compression-level=6\0"));
  checkForError(archive_write_set_format_pax_restricted(archive));
  checkForError(archive_write_open_filename(archive, archiveFilename.data()));
  time_t timeNow = time(nullptr);
  for(const auto &toAdd : resultsfolder) {
    for(const auto &fileEntry : fs::recursive_directory_iterator(toAdd.pathToFolderToAdd)) {
      if(fileEntry.is_regular_file() && fileEntry.path().extension() == toAdd.fileExtensionToAdd) {
        struct stat st;
        stat(fileEntry.path().string().data(), &st);
        archive_entry *entry      = archive_entry_new();
        std::string pathInArchive = toAdd.subFolderInArchiveToAddTo + "/" +
                                    std::filesystem::relative(fileEntry.path(), toAdd.pathToFolderToAdd).string();
        archive_entry_set_pathname(entry, pathInArchive.data());
        archive_entry_set_size(entry, st.st_size);
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_atime(entry, timeNow, 0);
        archive_entry_set_birthtime(entry, timeNow, 0);
        archive_entry_set_ctime(entry, timeNow, 0);
        archive_entry_set_perm(entry, 0644);
        checkForError(archive_write_header(archive, entry));
        FILE *fd   = fopen(fileEntry.path().string().data(), "r");
        size_t len = fread(buff, 1, sizeof(buff), fd);
        while(len > 0) {
          archive_write_data(archive, buff, len);
          len = fread(buff, 1, sizeof(buff), fd);
        }
        checkForError(fclose(fd));
        archive_entry_free(entry);
      }
    }
  }

  // Finish writing the archive
  checkForError(archive_write_close(archive));
  checkForError(archive_write_free(archive));

  return 0;
}

// Function to list all files within the archive
std::vector<std::string> listFiles(const std::string &archiveFilename)
{
  std::vector<std::string> fileList;

  // Open the archive for reading
  archive *a = archive_read_new();
  checkForError(archive_read_support_filter_xz(a));
  checkForError(archive_read_support_format_all(a));
  checkForError(archive_read_open_filename(a, archiveFilename.data(), 10240));

  // Read entries from the archive
  archive_entry *entry = nullptr;
  while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    const char *filename = archive_entry_pathname(entry);
    fileList.emplace_back(filename);
    archive_read_data_skip(a);
  }

  // Clean up
  checkForError(archive_read_close(a));
  checkForError(archive_read_free(a));

  return fileList;
}

// Function to read the content of a specific file within the archive
std::string readFile(const std::string &archiveFilename, const std::string &filename)
{
  // Open the archive for reading
  archive *a = archive_read_new();
  checkForError(archive_read_support_filter_all(a));
  checkForError(archive_read_support_format_all(a));
  checkForError(archive_read_open_filename(a, archiveFilename.data(), 10240));

  // Find the entry for the specified file
  archive_entry *entry;
  std::string content;
  while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    const char *entryFilename = archive_entry_pathname(entry);
    if(std::strcmp(entryFilename, filename.data()) == 0) {
      // Read the content of the file
      char buff[8192];
      ssize_t len;

      while((len = archive_read_data(a, buff, sizeof(buff))) > 0) {
        content.append(buff, len);
      }
      break;
    }
    archive_read_data_skip(a);
  }

  // Clean up
  checkForError(archive_read_close(a));
  checkForError(archive_read_free(a));

  return content;
}

}    // namespace joda::helper::xz
