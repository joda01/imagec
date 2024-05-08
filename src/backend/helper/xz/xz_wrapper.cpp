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

#include <archive.h>
#include <archive_entry.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

namespace joda::helper::xz {

std::mutex mArchivemutex;
namespace fs = std::filesystem;

// Function to create an XZ archive and add files to it
int createAndAddFiles(const std::string &archiveFilename, const std::string &pathToResultsFolder,
                      const std::string &fileExtension)
{
  std::lock_guard<std::mutex> lock(mArchivemutex);
  struct archive *a;
  struct archive_entry *entry;
  int r;
  char buff[8192];
  FILE *fd;
  int len;
  struct stat st;
  int compression_level = 6;    // Set compression level, range from 0 to 9

  // Open the archive for writing
  a        = archive_write_new();
  int okay = archive_write_add_filter_xz(a);
  std::cout << "OK " << std::to_string(okay) << std::endl;
  okay = archive_write_set_options(a, "compression-level=6\0");
  std::cout << "OK " << std::to_string(okay) << std::endl;
  archive_write_set_format_pax_restricted(a);
  archive_write_open_filename(a, archiveFilename.data());
  auto timeNow = time(0);
  for(const auto &fileEntry : fs::directory_iterator(pathToResultsFolder)) {
    if(fileEntry.is_regular_file() && fileEntry.path().extension() == fileExtension) {
      stat(fileEntry.path().string().data(), &st);
      entry = archive_entry_new();    // Note 2
      archive_entry_set_pathname(entry, fileEntry.path().filename().string().data());
      archive_entry_set_size(entry, st.st_size);    // Note 3
      archive_entry_set_filetype(entry, AE_IFREG);
      archive_entry_set_birthtime(entry, timeNow, 0);
      archive_entry_set_ctime(entry, timeNow, 0);
      archive_entry_set_perm(entry, 0644);
      archive_write_header(a, entry);
      fd  = fopen(fileEntry.path().string().data(), "r");
      len = fread(buff, 1, sizeof(buff), fd);
      while(len > 0) {
        archive_write_data(a, buff, len);
        len = fread(buff, 1, sizeof(buff), fd);
      }
      fclose(fd);
      archive_entry_free(entry);
    }
  }

  // Finish writing the archive
  archive_write_close(a);

  archive_write_free(a);

  return 0;
}

// Function to list all files within the archive
std::vector<std::string> listFiles(const std::string &archiveFilename)
{
  std::vector<std::string> fileList;
  struct archive *a;
  struct archive_entry *entry;
  int r;

  // Open the archive for reading
  a = archive_read_new();
  archive_read_support_filter_all(a);
  archive_read_support_format_all(a);
  r = archive_read_open_filename(a, archiveFilename.data(), 10240);
  if(r != ARCHIVE_OK) {
    return fileList;
  }

  // Read entries from the archive
  while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    const char *filename = archive_entry_pathname(entry);
    fileList.push_back(filename);
    archive_read_data_skip(a);
  }

  // Clean up
  // archive_read_close(a);
  archive_read_free(a);

  return fileList;
}

// Function to read the content of a specific file within the archive
std::string readFile(const std::string &archiveFilename, const std::string &filename)
{
  std::string content;
  struct archive *a;
  struct archive_entry *entry;
  int r;

  // Open the archive for reading
  a = archive_read_new();
  archive_read_support_filter_all(a);
  archive_read_support_format_all(a);
  r = archive_read_open_filename(a, archiveFilename.data(), 10240);
  if(r != ARCHIVE_OK) {
    return content;
  }

  // Find the entry for the specified file
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
    } else {
      archive_read_data_skip(a);
    }
  }

  // Clean up
  archive_read_close(a);
  archive_read_free(a);

  return content;
}

}    // namespace joda::helper::xz
