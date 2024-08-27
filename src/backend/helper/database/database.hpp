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
#include "backend/enums/enum_validity.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/file_grouper/file_grouper_types.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include "backend/processor/context/image_context.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/project_settings/experiment_settings.hpp"
#include "backend/settings/project_settings/project_cluster.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "backend/settings/project_settings/project_settings.hpp"
#include <duckdb/main/config.hpp>
#include <duckdb/main/connection.hpp>
#include <duckdb/main/database.hpp>
#include <opencv2/core/types.hpp>

namespace joda::db {

struct AnalyzeMeta
{
  joda::settings::ExperimentSettings experiment;
  std::chrono::system_clock::time_point timestamp;
};

struct ImageInfo
{
  std::string filename;
  enums::ChannelValidity validity;
};

class Database
{
public:
  Database() = default;
  /////////////////////////////////////////////////////
  void openDatabase(const std::filesystem::path &pathToDb);
  std::string startJob(const joda::settings::AnalyzeSettings &);
  void finishJob(const std::string &jobId);

  void insertGroup(uint16_t plateId, const joda::grp::GroupInformation &groupInfo);
  void insertImage(const joda::processor::ImageContext &, const joda::grp::GroupInformation &groupInfo);
  void insetImageToGroup(uint16_t plateId, uint64_t imageId, uint16_t imageIdx,
                         const joda::grp::GroupInformation &groupInfo);

  void insertImageChannels(uint64_t imageId, const joda::ome::OmeInfo &ome);
  void insertImagePlane(uint64_t imageId, const enums::PlaneId &, const ome::OmeInfo::ImagePlane &);

  void setImageValidity(uint64_t imageId, enums::ChannelValidity validity);
  void unsetImageValidity(uint64_t imageId, enums::ChannelValidity validity);
  void setImagePlaneValidity(uint64_t imageId, const enums::PlaneId &, enums::ChannelValidity validity);
  void setImagePlaneClusterClusterValidity(uint64_t imageId, const enums::PlaneId &, enums::ClusterId clusterId,
                                           enums::ChannelValidity validity);

  void insertObjects(const joda::processor::ImageContext &, const joda::atom::ObjectList &);

  auto selectExperiment() -> AnalyzeMeta;
  auto selectPlates() -> std::map<uint16_t, joda::settings::Plate>;
  auto selectImageChannels() -> std::map<uint32_t, joda::ome::OmeInfo::ChannelInfo>;
  auto selectClusters() -> std::map<enums::ClusterId, joda::settings::Cluster>;
  auto selectClasses() -> std::map<enums::ClassId, joda::settings::Class>;

  auto selectImageInfo(uint64_t imageId) -> ImageInfo;

  template <typename... ARGS>
  std::unique_ptr<duckdb::QueryResult> select(const std::string &query, ARGS... args)
  {
    auto connection = acquire();
    auto prep       = connection->Prepare(query);
    return prep->Execute(std::forward<ARGS>(args)...);
  }

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<duckdb::Connection> acquire() const
  {
    std::shared_ptr<duckdb::Connection> connection = std::make_shared<duckdb::Connection>(*mDb);
    return connection;
  }

  void createTables();
  bool insertExperiment(const joda::settings::ExperimentSettings &);
  std::string insertJobAndPlates(const joda::settings::AnalyzeSettings &exp);
  void insertClusters(const std::list<settings::Cluster> &);
  void insertClasses(const std::list<settings::Class> &);
  void insertGroup();
  void flatten(const std::vector<cv::Point> &, duckdb::vector<duckdb::Value> &);

  /////////////////////////////////////////////////////
  duckdb::DBConfig mDbCfg;
  std::unique_ptr<duckdb::DuckDB> mDb;
};

}    // namespace joda::db
