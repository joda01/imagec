///
/// \file      database.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <filesystem>
#include "backend/enums/types.hpp"
#include "backend/processor/context/image_context.hpp"
#include "backend/processor/context/iteration_context.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/project_settings/project_settings.hpp"

namespace joda::db {

class Database
{
public:
  Database() = default;
  /////////////////////////////////////////////////////
  void openDatabase(const std::filesystem::path &pathToDb);
  void closeDatabase();
  void insertProjectSettings(const joda::settings::AnalyzeSettings &);
  void insertImage(const joda::processor::ImageContext &);
  void insertImagePlane();
  void insertObjects(const joda::processor::IterationContext &);

private:
  /////////////////////////////////////////////////////
  void createTables();
  void insertPlates();
  void insertClusters(const joda::settings::ProjectSettings &);
  void insertClasses(const joda::settings::ProjectSettings &);
  void insertGroup();
};

}    // namespace joda::db
