///
/// \file      database.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "database.hpp"
#include <duckdb.h>

namespace joda::db {

/////////////////////////////////////////////////////
void Database::openDatabase(const std::filesystem::path &pathToDb)
{
}
void Database::closeDatabase()
{
}
void Database::insertProjectSettings(const joda::settings::AnalyzeSettings &)
{
}
void Database::insertImage(const joda::processor::ImageContext &)
{
}
void Database::insertObjects(const joda::processor::IterationContext &)
{
}
void Database::createTables()
{
}
void Database::insertPlates()
{
}
void Database::insertClusters(const joda::settings::ProjectSettings &)
{
}
void Database::insertClasses(const joda::settings::ProjectSettings &)
{
}
void Database::insertGroup()
{
}

}    // namespace joda::db
