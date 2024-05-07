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
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace joda::helper::xz {

// Function to create an XZ archive and add files to it
int createAndAddFile(const std::string &archiveFilename, const std::string &filename1, const std::string &dataToWrite)
{
  struct archive *a;
  struct archive_entry *entry;
  int r;

  // Open the archive for writing
  a = archive_write_new();
  archive_write_add_filter_xz(a);
  archive_write_set_format_pax_restricted(a);
  archive_write_open_filename(a, archiveFilename.data());

  // Add the first file (a.txt) to the archive
  entry = archive_entry_new();
  archive_entry_set_pathname(entry, filename1.data());
  archive_entry_set_filetype(entry, AE_IFREG);
  archive_entry_set_perm(entry, 0644);

  r = archive_write_header(a, entry);
  if(r != ARCHIVE_OK) {
    return 1;
  }

  archive_write_data(a, dataToWrite.data(), dataToWrite.size());

  archive_entry_free(entry);

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
  archive_read_close(a);
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
