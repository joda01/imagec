///
/// \file      database.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <duckdb.h>
#include <exception>
#include <filesystem>
#include <memory>
#include <vector>
#include "backend/enums/enum_validity.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/file_grouper/file_grouper_types.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include "backend/processor/context/image_context.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/project_settings/experiment_settings.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "backend/settings/project_settings/project_settings.hpp"
#include <duckdb/main/config.hpp>
#include <duckdb/main/connection.hpp>
#include <duckdb/main/database.hpp>
#include <opencv2/core/types.hpp>
#include "database_interface.hpp"
#include <BS_thread_pool.hpp>

namespace joda::db {

using DbArgs_t = std::vector<std::variant<std::string, uint16_t, uint32_t, uint64_t, double, int32_t>>;

class Database : public DatabaseInterface
{
public:
  Database()
  {
  }
  /////////////////////////////////////////////////////
  void openDatabase(const std::filesystem::path &pathToDb) override;
  void closeDatabase() override;
  std::string startJob(const joda::settings::AnalyzeSettings &, const std::string &jobName) override;
  void finishJob(const std::string &jobId) override;

  auto prepareImages(uint8_t plateId, int32_t series, enums::GroupBy groupBy, const std::string &filenameRegex,
                     const std::vector<std::filesystem::path> &imagePaths, const std::filesystem::path &imagesBasePath,
                     const joda::settings::ProjectImageSetup::PhysicalSizeSettings &defaultPhysicalSizeSettings,
                     BS::light_thread_pool &globalThreadPool)
      -> std::vector<std::tuple<std::filesystem::path, joda::ome::OmeInfo, uint64_t>> override;
  void setImageProcessed(uint64_t) override;

  void insertGroup(uint16_t plateId, const joda::grp::GroupInformation &groupInfo);
  void insertImage(const joda::processor::ImageContext &, const joda::grp::GroupInformation &groupInfo);
  void insetImageToGroup(uint16_t plateId, uint64_t imageId, uint16_t imageIdx, const joda::grp::GroupInformation &groupInfo);

  void insertImageChannels(uint64_t imageId, int32_t series, const joda::ome::OmeInfo &ome);
  void insertImagePlane(uint64_t imageId, const enums::PlaneId &, const ome::OmeInfo::ImagePlane &) override;

  void setImageValidity(uint64_t imageId, enums::ChannelValidity validity) override;
  void unsetImageValidity(uint64_t imageId, enums::ChannelValidity validity) override;
  void setImagePlaneValidity(uint64_t imageId, const enums::PlaneId &, enums::ChannelValidity validity) override;
  void setImagePlaneClasssClasssValidity(uint64_t imageId, const enums::PlaneId &, enums::ClassId classId, enums::ChannelValidity validity) override;

  void setAnalyzeSettingsCache(const std::string &jobID, const std::set<enums::ClassId> &outputClasses,
                               const std::map<enums::ClassId, std::set<int32_t>> &measureChannels,
                               const std::map<enums::ClassId, std::set<enums::ClassId>> &intersectingChannels,
                               const std::map<enums::ClassId, std::set<enums::ClassId>> &distanceChannels);

  void insertObjects(const joda::processor::ImageContext &, enums::Units, const joda::atom::ObjectList &) override;

  auto selectExperiment() -> AnalyzeMeta;
  auto selectPlates() -> std::map<uint16_t, joda::settings::Plate>;
  auto selectGroups() -> std::map<uint16_t, std::string>;

  auto selectImageChannels() -> std::map<uint32_t, joda::ome::OmeInfo::ChannelInfo>;
  auto selectNrOfTimeStacks() -> int32_t;

  auto selectClasses() -> std::map<enums::ClassId, joda::settings::Class>;

  auto selectGroupInfo(uint64_t groupId) -> GroupInfo;
  auto selectImageInfo(uint64_t imageId) -> ImageInfo;
  auto selectObjectInfo(uint64_t objectId) -> ObjectInfo;
  auto selectImages() -> std::vector<ImageInfo>;

  auto selectOutputClasses() -> std::set<enums::ClassId>;
  auto selectMeasurementChannelsForClasses() -> std::map<enums::ClassId, std::set<int32_t>>;
  auto selectIntersectingClassForClasses() -> std::map<enums::ClassId, std::set<enums::ClassId>>;
  auto selectDistanceClassForClasses() -> std::map<enums::ClassId, std::set<enums::ClassId>>;
  auto selectColocalizingClasses() -> std::set<std::set<enums::ClassId>>;

  void updateResultsTableSettings(const std::string &jobId, const std::string &settings);
  auto selectResultsTableSettings(const std::string &jobId) -> std::string;
  auto selectImageIdFromImageFileName(const std::string &imageFileName) -> uint64_t;
  auto selectGroupIdFromGroupName(const std::string &groupName) -> uint16_t;

  template <typename... ARGS>
  std::unique_ptr<duckdb::QueryResult> select(const std::string &query, ARGS... args)
  {
    auto connection = acquire();
    auto prep       = connection->Prepare(query);
    return prep->Execute(std::forward<ARGS>(args)...);
  }

  std::unique_ptr<duckdb::QueryResult> select(const std::string &query, const DbArgs_t &args);

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<duckdb::Connection> acquire() const
  {
    return std::make_unique<duckdb::Connection>(*mDb);
  }

  void createTables();
  bool insertExperiment(const joda::settings::ExperimentSettings &);
  std::string insertJobAndPlates(const joda::settings::AnalyzeSettings &exp, const std::string &jobName);
  void insertClasses(const std::list<settings::Class> &);
  void insertGroup();
  void flatten(const std::vector<cv::Point> &, duckdb::vector<duckdb::Value> &);
  void createAnalyzeSettingsCache(const std::string &jobId);

  /////////////////////////////////////////////////////
  std::unique_ptr<duckdb::DBConfig> mDbCfg;
  std::unique_ptr<duckdb::DuckDB> mDb;
};

}    // namespace joda::db
