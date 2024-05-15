///
/// \file      database.hpp
/// \author
/// \date      2024-05-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <sqlite3.h>
#include <string>
#include "duckdb.hpp"

namespace joda::db {

class Database
{
public:
  /////////////////////////////////////////////////////
  Database(const std::string &dbFile);
  void open();
  void close();
  void addExperiment(int id, const std::string &name);
  void addImage(int experimentId, int id, const std::string &name);
  void addChannel(int imageId, int id, const std::string &name);
  void addObject(int expId, int imageId, int channelId, int num);

private:
  /////////////////////////////////////////////////////
  std::string mDbFile;
  duckdb_database db;
  duckdb_connection con;
};

}    // namespace joda::db
