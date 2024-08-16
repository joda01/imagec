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

#include <duckdb.h>
#include <filesystem>
#include <vector>
#include "backend/enums/types.hpp"
#include "backend/processor/context/image_context.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/project_settings/experiment_settings.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "backend/settings/project_settings/project_settings.hpp"
#include <duckdb/main/config.hpp>
#include <duckdb/main/connection.hpp>
#include <duckdb/main/database.hpp>
#include <opencv2/core/types.hpp>

namespace joda::db {

class Database
{
public:
  Database() = default;
  /////////////////////////////////////////////////////
  void openDatabase(const std::filesystem::path &pathToDb);
  std::string startJob(const joda::settings::AnalyzeSettings &);
  void finishJob(const std::string &jobId);

  void insertImage(const joda::processor::ImageContext &);
  void insertImagePlane();
  void insertObjects(const joda::processor::ImageContext &, const joda::atom::ObjectList &);

  auto selectExperiment() -> joda::settings::ExperimentSettings;
  auto selectPlates() -> std::map<uint16_t, joda::settings::Plate>;

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<duckdb::Connection> acquire() const
  {
    std::shared_ptr<duckdb::Connection> connection = std::make_shared<duckdb::Connection>(*mDb);
    return connection;
  }

  template <typename... ARGS>
  std::unique_ptr<duckdb::QueryResult> select(const std::string &query, ARGS... args)
  {
    auto connection = acquire();
    auto prep       = connection->Prepare(query);
    return prep->Execute(std::forward<ARGS>(args)...);
  }

  void createTables();
  bool insertExperiment(const joda::settings::ExperimentSettings &);
  std::string insertJobAndPlates(const joda::settings::AnalyzeSettings &exp);
  void insertClusters(const std::vector<settings::Cluster> &);
  void insertClasses(const std::vector<settings::Class> &);
  void insertGroup();
  void flatten(const std::vector<cv::Point> &, duckdb::vector<duckdb::Value> &);

  /////////////////////////////////////////////////////
  duckdb::DBConfig mDbCfg;
  std::unique_ptr<duckdb::DuckDB> mDb;
};

}    // namespace joda::db
