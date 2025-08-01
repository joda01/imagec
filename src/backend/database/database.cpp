///
/// \file      database.cpp
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

#include "database.hpp"
#include <duckdb.h>
#include <chrono>
#include <exception>
#include <filesystem>
#include <mutex>
#include <stdexcept>
#include <string>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_grouping.hpp"
#include "backend/helper/base64.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/file_grouper/file_grouper.hpp"
#include "backend/helper/file_grouper/file_grouper_types.hpp"
#include "backend/helper/fnv1a.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/helper/rle/rle.hpp"
#include "backend/helper/threadpool/thread_pool.hpp"
#include "backend/helper/uuid.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "backend/settings/results_settings/results_settings.hpp"
#include "backend/settings/settings.hpp"
#include <duckdb/common/types.hpp>
#include <duckdb/common/types/string_type.hpp>
#include <duckdb/common/types/uuid.hpp>
#include <duckdb/common/types/value.hpp>
#include <duckdb/common/types/vector.hpp>
#include <duckdb/common/vector.hpp>
#include <duckdb/main/appender.hpp>
#include <nlohmann/json_fwd.hpp>

namespace joda::db {

template <class KEY, class VALUE>
auto duckdbMapToMap(auto &materializedResult) -> std::map<KEY, std::set<VALUE>>
{
  std::map<KEY, std::set<VALUE>> channels;

  if(materializedResult->RowCount() > 0) {
    duckdb::Value value = materializedResult->GetValue(0, 0);
    auto children       = duckdb::MapValue::GetChildren(value);
    for(int n = 0; n < children.size(); n++) {
      auto tuple   = duckdb::ListValue::GetChildren(children[n]);    // LIST<INT32>
      auto key     = tuple[0].GetValue<int32_t>();
      auto values  = duckdb::ListValue::GetChildren(tuple[1]);
      auto &toEdit = channels[static_cast<KEY>(key)];
      for(int m = 0; m < values.size(); m++) {
        toEdit.emplace(static_cast<VALUE>(values[m].GetValue<int32_t>()));
      }
    }
  }
  return channels;
}

/////////////////////////////////////////////////////
void Database::openDatabase(const std::filesystem::path &pathToDb)
{
  mDbCfg = std::make_unique<duckdb::DBConfig>();
  mDbCfg->SetOption("temp_directory", pathToDb.parent_path().string());
  mDb = std::make_unique<duckdb::DuckDB>(pathToDb.string(), mDbCfg.get());
  createTables();
}

void Database::closeDatabase()
{
  mDb.reset();
}

void Database::createTables()
{
  // Command to create a table
  const char *create_table_sql =
      "CREATE TABLE IF NOT EXISTS experiment ("
      "	experiment_id UUID,"
      " name STRING,"
      " notes STRING,"
      " PRIMARY KEY (experiment_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS jobs ("
      "	experiment_id UUID,"
      " job_id UUID,"
      " job_name TEXT, "
      " imagec_version TEXT, "
      " time_started TIMESTAMP,"
      " time_finished TIMESTAMP,"
      " settings TEXT,"
      " settings_results_table TEXT,"
      " settings_results_table_default TEXT,"
      " settings_tile_width UINTEGER,"
      " settings_tile_height UINTEGER,"
      " settings_image_series UINTEGER,"
      " PRIMARY KEY (job_id),"
      " FOREIGN KEY(experiment_id) REFERENCES experiment(experiment_id)"
      ");"

      "ALTER TABLE jobs "
      " ADD COLUMN IF NOT EXISTS settings_results_table TEXT;\n"

      "ALTER TABLE jobs "
      " ADD COLUMN IF NOT EXISTS settings_results_table_default TEXT;\n"

      "ALTER TABLE jobs "
      " ADD COLUMN IF NOT EXISTS settings_tile_width UINTEGER DEFAULT 4096;\n"

      "ALTER TABLE jobs "
      " ADD COLUMN IF NOT EXISTS settings_tile_height UINTEGER DEFAULT 4096;\n"

      "ALTER TABLE jobs "
      " ADD COLUMN IF NOT EXISTS settings_image_series UINTEGER DEFAULT 0;\n"

      "CREATE TABLE IF NOT EXISTS plates ("
      " job_id UUID,"
      " plate_id USMALLINT,"
      " name STRING,"
      " notes STRING,"
      " rows USMALLINT,"
      " cols USMALLINT,"
      " image_folder STRING,"
      " well_image_order STRING,"
      " group_by STRING,"
      " filename_regex STRING,"
      " PRIMARY KEY (plate_id),"
      " FOREIGN KEY(job_id) REFERENCES jobs(job_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS classes ("
      "	class_id USMALLINT,"
      " short_name STRING,"
      " name STRING,"
      " notes STRING,"
      " color STRING,"
      " PRIMARY KEY (class_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS groups ("
      " plate_id USMALLINT,"
      " group_id USMALLINT,"
      " name STRING,"
      " notes STRING,"
      " pos_on_plate_x UINTEGER,"
      " pos_on_plate_y UINTEGER,"
      " PRIMARY KEY (plate_id, group_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS images ("
      " image_id UBIGINT,"
      " file_name TEXT,"
      " original_file_path TEXT,"
      " relative_file_path TEXT,"
      " nr_of_c_stacks UINTEGER,"
      " nr_of_z_stacks UINTEGER,"
      " nr_of_t_stacks UINTEGER,"
      " width UINTEGER,"
      " height UINTEGER,"
      " validity UBIGINT,"
      " processed BOOLEAN,"
      " PRIMARY KEY (image_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS images_groups ("
      " plate_id USMALLINT,"
      " group_id USMALLINT,"
      " image_id UBIGINT,"
      " image_group_idx UINTEGER, "
      " PRIMARY KEY (plate_id, group_id, image_id),"
      " FOREIGN KEY(plate_id, group_id) REFERENCES groups(plate_id, group_id),"
      " FOREIGN KEY(image_id) REFERENCES images(image_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS images_channels ("
      " image_id UBIGINT,"
      " stack_c UINTEGER, "
      " channel_id TEXT,"
      " name TEXT,"
      " PRIMARY KEY (image_id, stack_c),"
      " FOREIGN KEY(image_id) REFERENCES images(image_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS images_planes ("
      " image_id UBIGINT,"
      " stack_c UINTEGER, "
      " stack_z UINTEGER, "
      " stack_t UINTEGER, "
      " validity UBIGINT, "
      " PRIMARY KEY (image_id, stack_c, stack_z, stack_t),"
      " FOREIGN KEY(image_id) REFERENCES images(image_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS classes_planes ("
      " image_id UBIGINT,"
      "	class_id USMALLINT,"
      " stack_c UINTEGER, "
      " stack_z UINTEGER, "
      " stack_t UINTEGER, "
      " validity UBIGINT,"
      " PRIMARY KEY (image_id,class_id, stack_c, stack_z, stack_t),"
      " FOREIGN KEY(image_id) REFERENCES images(image_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS objects ("
      "	image_id UBIGINT,"
      " object_id UBIGINT,"
      " class_id USMALLINT,"
      " stack_c UINTEGER,"
      " stack_z UINTEGER,"
      " stack_t UINTEGER,"
      " meas_confidence float,"
      " meas_area_size DOUBLE,"
      " meas_perimeter float,"
      " meas_circularity float,"
      " meas_center_x UINTEGER,"    // Centroid X (unweighted center of mass)
      " meas_center_y UINTEGER,"    // Centroid Y (unweighted center of mass)
      " meas_box_x UINTEGER,"
      " meas_box_y UINTEGER,"
      " meas_box_width UINTEGER,"
      " meas_box_height UINTEGER,"
      " meas_mask MAP(UINTEGER,BOOLEAN),"
      " meas_contour UINTEGER[],"
      " meas_origin_object_id UBIGINT,"
      " meas_parent_object_id UBIGINT,"
      " meas_parent_class_id USMALLINT DEFAULT NULL,"    // Class ID of the parent object
      " meas_tracking_id UBIGINT"    // Elements having the same linked_object_id represent the same element (e.g used for coloc or object tracking)
      ");"

      "ALTER TABLE objects "
      " ADD COLUMN IF NOT EXISTS meas_tracking_id UBIGINT DEFAULT 0;\n"

      "ALTER TABLE objects "
      " ADD COLUMN IF NOT EXISTS meas_origin_object_id UBIGINT DEFAULT 0;\n"

      "ALTER TABLE objects "
      " ADD COLUMN IF NOT EXISTS meas_parent_object_id UBIGINT DEFAULT 0;\n"

      "ALTER TABLE objects "
      " ADD COLUMN IF NOT EXISTS meas_parent_class_id USMALLINT DEFAULT NULL;\n"

      "CREATE TABLE IF NOT EXISTS object_measurements ("
      "	image_id UBIGINT,"
      " object_id UBIGINT,"
      " meas_stack_c UINTEGER,"
      " meas_stack_z UINTEGER,"
      " meas_stack_t UINTEGER,"
      " meas_intensity_sum UBIGINT,"
      " meas_intensity_avg float,"
      " meas_intensity_min UINTEGER,"
      " meas_intensity_max UINTEGER"
      ");"

      "CREATE TABLE IF NOT EXISTS distance_measurements ("
      "	image_id UBIGINT,"
      " object_id UBIGINT,"
      " class_id USMALLINT,"    // Class ID of the foreign object. We store this to reduce the query complexity.
      " meas_object_id UBIGINT,"
      " meas_class_id USMALLINT,"    // We do this for performance reason to don't need a join in the query
      " meas_stack_c UINTEGER,"
      " meas_stack_z UINTEGER,"
      " meas_stack_t UINTEGER,"
      " meas_distance_center_to_center DOUBLE,"
      " meas_distance_center_to_surface_min DOUBLE,"
      " meas_distance_center_to_surface_max DOUBLE,"
      " meas_distance_surface_to_surface_min DOUBLE,"
      " meas_distance_surface_to_surface_max DOUBLE"
      ");"

      "CREATE TABLE IF NOT EXISTS cache_analyze_settings ("
      " job_id UUID,"
      " output_classes INTEGER[],"                         // A list of output channels
      " measured_channels MAP(INTEGER, INTEGER[]),"        // A map with key is a channel id and value is an array of image channels
      " intersecting_channels MAP(INTEGER, INTEGER[]),"    // A map with key is a channel id and value is an array of channel ids which
                                                           // intersects with this channel
      " distance_from_classes MAP(INTEGER, INTEGER[])"     // A map with key is a channel id and value is an array of channel ids which
                                                           // measures the distance to this channel
      ");";

  auto connection = acquire();
  auto result     = connection->Query(create_table_sql);
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  //
  // Do some migrations
  //
  /// \todo Add relative file path -> This is legacy and cen be removed in further releases
  {
    std::string query =
        "SELECT COUNT(*) FROM information_schema.columns "
        "WHERE table_name = 'images' AND column_name = 'relative_file_path';";
    auto con    = acquire();
    auto result = con->Query(query);
    if(result->GetValue<int64_t>(0, 0) <= 0) {
      std::string alter_sql = "ALTER TABLE images ADD COLUMN relative_file_path TEXT DEFAULT '';";
      con->Query(alter_sql);
      {
        auto plate = selectPlates();
        if(!plate.empty()) {
          auto images           = selectImages();
          auto workingDirectory = std::filesystem::path(plate.begin()->second.imageFolder);
          for(const auto &image : images) {
            auto originalFilePath = std::filesystem::path(image.imageFilePath);
            auto relativePath     = std::filesystem::relative(originalFilePath, workingDirectory);
            std::string alter_sql =
                "UPDATE images SET relative_file_path = '" + relativePath.string() + "' WHERE original_file_path='" + originalFilePath.string() + "'";
            con->Query(alter_sql);
          }
        }
      }
    }
  }

  //
  {
    std::unique_ptr<duckdb::QueryResult> result = select("SELECT job_id FROM cache_analyze_settings\n");
    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }
    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    if(materializedResult->RowCount() <= 0) {
      joda::log::logInfo("Start migration: Create analyze settings cache ...");

      /// \todo Fill the parent object class id
      {
        std::unique_ptr<duckdb::QueryResult> result = select("SELECT class_id,object_id, meas_parent_object_id FROM objects\n");
        if(result->HasError()) {
          throw std::invalid_argument(result->GetError());
        }
        auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
        std::map<uint64_t, enums::ClassId> objectIdClassMapping;
        std::vector<std::pair<uint64_t, uint64_t>> parentIdObjectIdMapping;

        for(size_t n = 0; n < materializedResult->RowCount(); n++) {
          auto classID        = (static_cast<enums::ClassId>(materializedResult->GetValue(0, n).GetValue<uint16_t>()));
          auto objectId       = materializedResult->GetValue(1, n).GetValue<uint64_t>();
          auto parentObjectId = materializedResult->GetValue(2, n).GetValue<uint64_t>();
          objectIdClassMapping.emplace(objectId, classID);
          parentIdObjectIdMapping.emplace_back(parentObjectId, objectId);
        }

        for(const auto &[parentObjectId, objectId] : parentIdObjectIdMapping) {
          if(parentObjectId > 0) {
            std::unique_ptr<duckdb::QueryResult> result = select("UPDATE objects SET meas_parent_class_id=? WHERE object_id=?\n",
                                                                 static_cast<uint16_t>(objectIdClassMapping.at(parentObjectId)), objectId);
          }
        }
      }

      /// \todo Store analyze cache settings
      {
        auto data = selectExperiment();
        if(!data.jobId.empty()) {
          createAnalyzeSettingsCache(data.jobId);
        }
      }
      joda::log::logInfo("Finished migration.");
    }
  }
}

std::unique_ptr<duckdb::QueryResult> Database::select(const std::string &query, const DbArgs_t &args)
{
  auto connection = acquire();
  auto prep       = connection->Prepare(query);
  duckdb::vector<duckdb::Value> argsPrepared;
  for(const auto &arg : args) {
    if(std::holds_alternative<std::string>(arg)) {
      argsPrepared.emplace_back(std::get<std::string>(arg));
    } else if(std::holds_alternative<uint16_t>(arg)) {
      argsPrepared.emplace_back(duckdb::Value::USMALLINT(std::get<uint16_t>(arg)));
    } else if(std::holds_alternative<uint32_t>(arg)) {
      argsPrepared.emplace_back(duckdb::Value::UINTEGER(std::get<uint32_t>(arg)));
    } else if(std::holds_alternative<int32_t>(arg)) {
      argsPrepared.emplace_back(duckdb::Value::INTEGER(std::get<int32_t>(arg)));
    } else if(std::holds_alternative<uint64_t>(arg)) {
      argsPrepared.emplace_back(duckdb::Value::UBIGINT(std::get<uint64_t>(arg)));
    } else if(std::holds_alternative<double>(arg)) {
      argsPrepared.emplace_back(duckdb::Value::DOUBLE(std::get<double>(arg)));
    }
  }
  return prep->Execute(argsPrepared);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Database::insertObjects(const joda::processor::ImageContext &imgContext, const joda::atom::ObjectList &objectsList)
{
  auto connection = acquire();
  // connection->BeginTransaction();
  auto objects               = duckdb::Appender(*connection, "objects");
  auto object_measurements   = duckdb::Appender(*connection, "object_measurements");
  auto distance_measurements = duckdb::Appender(*connection, "distance_measurements");

  for(const auto &[_, obj] : objectsList) {
    for(const auto &roi : *obj) {
      objects.BeginRow();
      // Primary key
      objects.Append<uint64_t>(imgContext.imageId);             // "	image_id UBIGINT,"
      objects.Append<uint64_t>(roi.getObjectId());              // " object_id UBIGINT,"
      objects.Append<uint16_t>((uint16_t) roi.getClassId());    // " class_id USMALLINT,"
      objects.Append<uint32_t>(roi.getC());                     // " stack_c UINTEGER,"
      objects.Append<uint32_t>(roi.getZ());                     // " stack_z UINTEGER,"
      objects.Append<uint32_t>(roi.getT());                     // " stack_t UINTEGER,"
      // Data
      objects.Append<float>(roi.getConfidence());                   // " meas_confidence float,"
      objects.Append<double>(roi.getAreaSize());                    // " meas_area_size DOUBLE,"
      objects.Append<float>(roi.getPerimeter());                    // " meas_perimeter float,"
      objects.Append<float>(roi.getCircularity());                  // " meas_circularity float,"
      objects.Append<uint32_t>(roi.getCentroidReal().x);            // " meas_center_x UINTEGER,"
      objects.Append<uint32_t>(roi.getCentroidReal().y);            // " meas_center_y UINTEGER,"
      objects.Append<uint32_t>(roi.getBoundingBoxReal().x);         // " meas_box_x UINTEGER,"
      objects.Append<uint32_t>(roi.getBoundingBoxReal().y);         // " meas_box_y UINTEGER,"
      objects.Append<uint32_t>(roi.getBoundingBoxReal().width);     // " meas_box_width UINTEGER,"
      objects.Append<uint32_t>(roi.getBoundingBoxReal().height);    // " meas_box_height UINTEGER,"

      auto mask =
          duckdb::Value::MAP(duckdb::LogicalType(duckdb::LogicalTypeId::UINTEGER), duckdb::LogicalType(duckdb::LogicalTypeId::BOOLEAN), {}, {});
      objects.Append<duckdb::Value>(mask);
      /* objects.Append<duckdb::Value>(
           joda::rle::rle_encode({roi.getMask().datastart, roi.getMask().dataend}));    // " meas_mask BOOLEAN[]"*/

      duckdb::vector<duckdb::Value> flattenPoints;
      // flatten(roi.getContour(), flattenPoints);
      auto contour = duckdb::Value::LIST(duckdb::LogicalType(duckdb::LogicalTypeId::UINTEGER), flattenPoints);
      objects.Append<duckdb::Value>(contour);    // " meas_contour UINTEGER[]"

      objects.Append<uint64_t>(roi.getOriginObjectId());    // "	meas_origin_object_id UBIGINT"
      objects.Append<uint64_t>(roi.getParentObjectId());    // "	meas_parent_object_id UBIGINT"
      if(roi.getParentObjectId() > 0 && objectsList.containsObjectById(roi.getParentObjectId())) {
        objects.Append<uint16_t>(
            static_cast<uint16_t>(objectsList.getObjectById(roi.getParentObjectId())->getClassId()));    // "	meas_parent_class_id USMALLINT"
      } else {
        objects.AppendDefault();    // No parent
      }
      objects.Append<uint64_t>(roi.getTrackingId());    // "	meas_tracking_id UBIGINT"

      objects.EndRow();

      //
      // Intensities
      //
      for(const auto &[plane, intensity] : roi.getIntensity()) {
        object_measurements.BeginRow();
        // Primary key
        object_measurements.Append<uint64_t>(imgContext.imageId);    //       "	image_id UBIGINT,"
        object_measurements.Append<uint64_t>(roi.getObjectId());     //       " object_id UBIGINT,"
        //  Data
        object_measurements.Append<uint32_t>(plane.imagePlane.cStack);    //       " meas_stack_c UINTEGER,"
        object_measurements.Append<uint32_t>(plane.imagePlane.zStack);    //       " meas_stack_z UINTEGER,"
        object_measurements.Append<uint32_t>(plane.imagePlane.tStack);    //       " meas_stack_t UINTEGER,"
        object_measurements.Append<uint64_t>(intensity.intensitySum);     //       " meas_intensity_sum UBIGINT,"
        object_measurements.Append<float>(intensity.intensityAvg);        //       " meas_intensity_avg float,"
        object_measurements.Append<uint32_t>(intensity.intensityMin);     //       " meas_intensity_min UINTEGER,"
        object_measurements.Append<uint32_t>(intensity.intensityMax);     //       " meas_intensity_max UINTEGER"
        object_measurements.EndRow();
      }

      //
      // Distance
      //
      for(const auto &[measObjectId, distance] : roi.getDistances()) {
        try {
          distance_measurements.BeginRow();
          // Primary key
          distance_measurements.Append<uint64_t>(imgContext.imageId);                         //       "	image_id UBIGINT,"
          distance_measurements.Append<uint64_t>(roi.getObjectId());                          //       " object_id UBIGINT,"
          distance_measurements.Append<uint16_t>(static_cast<uint16_t>(roi.getClassId()));    //       " meas_class_id USMALLINT,"
          distance_measurements.Append<uint64_t>(measObjectId);                               //       " meas_object_id UBIGINT,"
          distance_measurements.Append<uint16_t>(
              static_cast<uint16_t>(objectsList.getObjectById(measObjectId)->getClassId()));    //       " meas_class_id USMALLINT,"

          //  Data
          distance_measurements.Append<uint32_t>(roi.getC());                             //       " meas_stack_c UINTEGER,"
          distance_measurements.Append<uint32_t>(roi.getZ());                             //       " meas_stack_z UINTEGER,"
          distance_measurements.Append<uint32_t>(roi.getT());                             //       " meas_stack_t UINTEGER,"
          distance_measurements.Append<double>(distance.distanceCentroidToCentroid);      // meas_distance_center_to_center DOUBLE,"
          distance_measurements.Append<double>(distance.distanceCentroidToSurfaceMin);    // meas_distance_center_to_surface_min DOUBLE,"
          distance_measurements.Append<double>(distance.distanceCentroidToSurfaceMax);    // meas_distance_center_to_surface_max DOUBLE,"
          distance_measurements.Append<double>(distance.distanceSurfaceToSurfaceMin);     // meas_distance_surface_to_surface_min DOUBLE,
          distance_measurements.Append<double>(distance.distanceSurfaceToSurfaceMax);     // meas_distance_surface_to_surface_max DOUBLE"

          distance_measurements.EndRow();
        } catch(const std::exception &ex) {
          joda::log::logError("Could not found object with ID >" + std::to_string(measObjectId) + "<");
        }
      }
    }
  }
  objects.Close();
  object_measurements.Close();
  distance_measurements.Close();

  //
  // Statistics
  //
  // auto statistics             = duckdb::Appender(*connection, "statistics");
  // auto statistic_measurements = duckdb::Appender(*connection, "statistic_measurements");
  // statistics.Close();
  // statistic_measurements.Close();
}

auto Database::prepareImages(uint8_t plateId, int32_t series, enums::GroupBy groupBy, const std::string &filenameRegex,
                             const std::vector<std::filesystem::path> &imagePaths, const std::filesystem::path &imagesBasePath,
                             BS::thread_pool &globalThreadPool) -> std::vector<std::tuple<std::filesystem::path, joda::ome::OmeInfo, uint64_t>>
{
  std::vector<std::tuple<std::filesystem::path, joda::ome::OmeInfo, uint64_t>> imagesToProcess;
  joda::grp::FileGrouper grouper(groupBy, filenameRegex);

  auto connection      = acquire();
  auto groups          = duckdb::Appender(*connection, "groups");
  auto images          = duckdb::Appender(*connection, "images");
  auto images_groups   = duckdb::Appender(*connection, "images_groups");
  auto images_channels = duckdb::Appender(*connection, "images_channels");
  std::set<uint16_t> addedGroups;

  std::mutex insertMutex;

  //
  // Preparing -> Insert all images to database
  //
  BS::multi_future<void> prepareFuture;

  for(const auto &imagePath : imagePaths) {
    auto prepareImage = [&groups, &grouper, &addedGroups, &imagesToProcess, &images, &images_groups, &images_channels, &insertMutex, &series, plateId,
                         imagePath, &imagesBasePath]() {
      auto ome         = joda::image::reader::ImageReader::getOmeInformation(imagePath, series);
      uint64_t imageId = joda::helper::fnv1a(imagePath.string());
      auto groupInfo   = grouper.getGroupForFilename(imagePath);

      {
        std::lock_guard<std::mutex> lock(insertMutex);
        imagesToProcess.emplace_back(imagePath, ome, imageId);
        // Group
        {
          if(!addedGroups.contains(groupInfo.groupId)) {
            groups.BeginRow();
            groups.Append<uint16_t>(plateId);                        //       " plate_id USMALLINT,"
            groups.Append<uint16_t>(groupInfo.groupId);              //       " group_id USMALLINT,"
            groups.Append<duckdb::string_t>(groupInfo.groupName);    //       " name STRING,"
            groups.Append<duckdb::string_t>("");                     //       " notes STRING,"
            groups.Append<uint32_t>(groupInfo.wellPosX);             //       " pos_on_plate_x UINTEGER,"
            groups.Append<uint32_t>(groupInfo.wellPosY);             //       " pos_on_plate_y UINTEGER,"
            groups.EndRow();
            addedGroups.emplace(groupInfo.groupId);
          }
        }

        // Image
        {
          auto relativePath = std::filesystem::relative(imagePath, imagesBasePath);
          images.BeginRow();
          images.Append<uint64_t>(imageId);                                  //       " image_id UBIGINT,"
          images.Append<duckdb::string_t>(imagePath.filename().string());    //       " file_name TEXT,"
          images.Append<duckdb::string_t>(imagePath.string());               //       " original_file_path TEXT
          images.Append<duckdb::string_t>(relativePath.string());            //       " relative_file_path TEXT
          images.Append<uint32_t>(ome.getNrOfChannels(series));              //       " nr_of_c_stacks UINTEGER
          images.Append<uint32_t>(ome.getNrOfZStack(series));                //       " nr_of_z_stacks UINTEGER
          images.Append<uint32_t>(ome.getNrOfTStack(series));                //       " nr_of_t_stacks UINTEGER
          images.Append<uint32_t>(std::get<0>(ome.getSize(series)));         //       " width UINTEGER,"
          images.Append<uint32_t>(std::get<1>(ome.getSize(series)));         //       " height UINTEGER,"
          images.Append<uint64_t>(0);                                        //       " validity UBIGINT,"
          images.Append<bool>(false);                                        //       " processed BOOL,"
          images.EndRow();
        }

        // Image Group
        {
          images_groups.BeginRow();
          images_groups.Append<uint16_t>(plateId);               //       " plate_id USMALLINT,"
          images_groups.Append<uint16_t>(groupInfo.groupId);     //       " group_id USMALLINT,"
          images_groups.Append<uint64_t>(imageId);               //       " image_id UBIGINT,"
          images_groups.Append<uint32_t>(groupInfo.imageIdx);    //       " image_group_idx UINTEGER, "
          images_groups.EndRow();
        }

        // Image channel
        {
          for(const auto &[channelId, channel] : ome.getChannelInfos(series)) {
            images_channels.BeginRow();
            images_channels.Append<uint64_t>(imageId);                      // " image_id UBIGINT,"
            images_channels.Append<uint32_t>(channelId);                    // " stack_c UINTEGER, "
            images_channels.Append<duckdb::string_t>(channel.channelId);    // " channel_id TEXT,"
            images_channels.Append<duckdb::string_t>(channel.name);         // " name TEXT,"
            images_channels.EndRow();
          }
        }
      }
    };

    prepareFuture.push_back(globalThreadPool.submit_task(prepareImage));
  }

  prepareFuture.wait();

  groups.Close();
  images.Close();
  images_groups.Close();
  images_channels.Close();

  return imagesToProcess;
}

void Database::insertGroup(uint16_t plateId, const joda::grp::GroupInformation &groupInfo)
{
  try {
    auto connection = acquire();
    auto prepare    = connection->Prepare(
        "INSERT OR IGNORE INTO groups (plate_id, group_id, name, notes, pos_on_plate_x, pos_on_plate_y) VALUES (?, ?, "
           "?, ?, ?, "
           "?)");
    prepare->Execute(plateId, groupInfo.groupId, groupInfo.groupName, "", groupInfo.wellPosX, groupInfo.wellPosY);
  } catch(duckdb::ConstraintException &e) {
    // Handle the constraint violation
    std::cerr << "Constraint Error: " << e.what() << std::endl;
    // Optionally, log the error or take other actions
  } catch(const std::exception &ex) {
    std::cout << "GR: " << ex.what() << std::endl;
  }
}

///
/// \brief
/// \author    Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Database::insertImage(const joda::processor::ImageContext &image, const joda::grp::GroupInformation &groupInfo)
{
  auto connection = acquire();
  auto prepare    = connection->Prepare(
      "INSERT OR IGNORE INTO images (image_id, file_name, original_file_path, nr_of_c_stacks, nr_of_z_stacks, "
         "nr_of_t_stacks,width,height,validity) "
         "VALUES (?, ?, ?, ?, ?, ?, ?, ? ,? )");

  auto [width, heigh] = image.imageMeta.getSize(image.series);
  prepare->Execute(image.imageId, image.imagePath.filename().string(), image.imagePath.string(), image.imageMeta.getNrOfChannels(image.series),
                   image.imageLoader.getNrOfZStacksToProcess(), image.imageLoader.getNrOfTStacksToProcess(), width, heigh, 0);
}

///
/// \brief
/// \author    Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Database::insertImageChannels(uint64_t imageId, int32_t series, const joda::ome::OmeInfo &ome)
{
  auto connection = acquire();
  auto channelsDb = duckdb::Appender(*connection, "images_channels");
  for(const auto &[channelId, channel] : ome.getChannelInfos(series)) {
    channelsDb.BeginRow();
    channelsDb.Append<uint64_t>(imageId);                      // " image_id UBIGINT,"
    channelsDb.Append<uint32_t>(channelId);                    // " stack_c UINTEGER, "
    channelsDb.Append<duckdb::string_t>(channel.channelId);    // " channel_id TEXT,"
    channelsDb.Append<duckdb::string_t>(channel.name);         // " name TEXT,"
    channelsDb.EndRow();
  }
  channelsDb.Close();
}

/*

///
/// \brief      Create control image
/// \author     Joachim Danmayr
///
void Analyzer::markImageChannelAsManualInvalid(const std::string &analyzeId, uint8_t plateId, ChannelIndex channel,
                                               uint64_t imageId)
{
  std::unique_ptr<duckdb::QueryResult> result = mDatabase.select(
      "UPDATE channels_images SET validity = validity | ? WHERE analyze_id=? AND channel_id=? AND image_id=?",
      static_cast<uint64_t>((1 << static_cast<uint32_t>(ObjectValidityEnum::MANUAL_OUT_SORTED))),
      duckdb::Value::UUID(analyzeId), static_cast<uint8_t>(channel), imageId);
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
}

void Analyzer::unMarkImageChannelAsManualInvalid(const std::string &analyzeId, uint8_t plateId, ChannelIndex channel,
                                                 uint64_t imageId)
{
  std::unique_ptr<duckdb::QueryResult> result = mDatabase.select(
      "UPDATE channels_images SET validity = validity & ? WHERE analyze_id=? AND channel_id=? AND image_id=?",
      ~static_cast<uint64_t>((1 << static_cast<uint32_t>(ObjectValidityEnum::MANUAL_OUT_SORTED))),
      duckdb::Value::UUID(analyzeId), static_cast<uint8_t>(channel), imageId);
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
}

*/

///
/// \brief
/// \author    Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Database::insertImagePlane(uint64_t imageId, const enums::PlaneId &planeId, const ome::OmeInfo::ImagePlane &planeInfo)
{
  auto connection = acquire();
  auto prepare    = connection->Prepare("INSERT OR IGNORE INTO images_planes (image_id, stack_c, stack_z, stack_t, validity) VALUES (?, ?, ?, ?, ?)");
  prepare->Execute(imageId, planeId.cStack, planeId.zStack, planeId.tStack, 0);
}

///
/// \brief
/// \author    Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Database::setImageValidity(uint64_t imageId, enums::ChannelValidity validity)
{
  std::unique_ptr<duckdb::QueryResult> result =
      select("UPDATE images SET validity = validity | ? WHERE image_id=?", static_cast<uint64_t>(validity.to_ullong()), imageId);
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
}

///
/// \brief
/// \author    Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Database::setImageProcessed(uint64_t imageId)
{
  std::unique_ptr<duckdb::QueryResult> result = select("UPDATE images SET processed = true  WHERE image_id=?", imageId);
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
}

///
/// \brief
/// \author    Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Database::unsetImageValidity(uint64_t imageId, enums::ChannelValidity validity)
{
  std::unique_ptr<duckdb::QueryResult> result =
      select("UPDATE images SET validity = validity & ~(?) WHERE image_id=?", static_cast<uint64_t>(validity.to_ullong()), imageId);
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
}

///
/// \brief
/// \author    Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Database::setImagePlaneValidity(uint64_t imageId, const enums::PlaneId &planeId, enums::ChannelValidity validity)
{
  std::unique_ptr<duckdb::QueryResult> result =
      select("UPDATE images_planes SET validity = validity | ? WHERE image_id=? AND stack_c=? AND stack_z=? AND stack_t=?",
             static_cast<uint64_t>(validity.to_ullong()), imageId, planeId.cStack, planeId.zStack, planeId.tStack);
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
}

///
/// \brief
/// \author    Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Database::setImagePlaneClasssClasssValidity(uint64_t imageId, const enums::PlaneId &planeId, enums::ClassId classId,
                                                 enums::ChannelValidity validity)
{
  std::unique_ptr<duckdb::QueryResult> result = select(
      "INSERT INTO classes_planes (image_id, class_id, stack_c, stack_z, stack_t, validity) VALUES (?, ?, ?, ?, ?) "
      "ON CONFLICT DO UPDATE SET validity = validity | ?",
      imageId, static_cast<uint16_t>(classId), planeId.cStack, planeId.zStack, planeId.tStack, static_cast<uint64_t>(validity.to_ullong()),
      static_cast<uint64_t>(validity.to_ullong()));
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
}

///
/// \brief     We store the possible class outputs from the pipeline in a database cache
///            This information is needed for result generation to know how which outputs,
///            clocs, measures and distances are available.
/// \author    Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Database::setAnalyzeSettingsCache(const std::string &jobIdStr, const std::set<enums::ClassId> &outputClasses,
                                       const std::map<enums::ClassId, std::set<int32_t>> &measureChannels,
                                       const std::map<enums::ClassId, std::set<enums::ClassId>> &intersectingChannels,
                                       const std::map<enums::ClassId, std::set<enums::ClassId>> &distanceChannels)
{
  //
  //
  //
  duckdb::vector<duckdb::Value> flattenPoints;
  for(const auto &id : outputClasses) {
    flattenPoints.emplace_back(static_cast<int32_t>(id));
  }
  auto outputClassesList = duckdb::Value::LIST(duckdb::LogicalType(duckdb::LogicalTypeId::INTEGER), flattenPoints);

  //
  //
  //
  duckdb::vector<duckdb::Value> classesForImg;
  duckdb::vector<duckdb::Value> valuesImgChannels;
  for(const auto &[classID, channels] : measureChannels) {
    classesForImg.emplace_back(static_cast<int32_t>(classID));
    duckdb::vector<duckdb::Value> channelsInt{channels.begin(), channels.end()};
    valuesImgChannels.emplace_back(duckdb::Value::LIST(duckdb::LogicalType(duckdb::LogicalTypeId::INTEGER), channelsInt));
  }
  duckdb::LogicalType listTypeImgChannel = duckdb::LogicalType::LIST(duckdb::LogicalType::INTEGER);
  auto measuredChannelsMap =
      duckdb::Value::MAP(duckdb::LogicalType(duckdb::LogicalTypeId::INTEGER), listTypeImgChannel, classesForImg, valuesImgChannels);

  //
  //
  //
  duckdb::vector<duckdb::Value> classesForIntersection;
  duckdb::vector<duckdb::Value> classesIntersecting;
  for(const auto &[classID, classIntersect] : intersectingChannels) {
    classesForIntersection.emplace_back(static_cast<int32_t>(classID));
    duckdb::vector<duckdb::Value> channelsInt;
    for(const auto &id : classIntersect) {
      channelsInt.emplace_back(static_cast<int32_t>(id));
    }
    classesIntersecting.emplace_back(duckdb::Value::LIST(duckdb::LogicalType(duckdb::LogicalTypeId::INTEGER), channelsInt));
  }
  duckdb::LogicalType listTypeIntersectingClass = duckdb::LogicalType::LIST(duckdb::LogicalType::INTEGER);
  auto intersectingChannelsMap =
      duckdb::Value::MAP(duckdb::LogicalType(duckdb::LogicalTypeId::INTEGER), listTypeIntersectingClass, classesForIntersection, classesIntersecting);

  //
  //
  //
  duckdb::vector<duckdb::Value> classesForDistance;
  duckdb::vector<duckdb::Value> classesDistances;
  for(const auto &[classID, classIntersect] : distanceChannels) {
    classesForDistance.emplace_back(static_cast<int32_t>(classID));
    duckdb::vector<duckdb::Value> channelsInt;
    for(const auto &id : classIntersect) {
      channelsInt.emplace_back(static_cast<int32_t>(id));
    }
    classesDistances.emplace_back(duckdb::Value::LIST(duckdb::LogicalType(duckdb::LogicalTypeId::INTEGER), channelsInt));
  }

  duckdb::LogicalType listTypeDistanceToClass = duckdb::LogicalType::LIST(duckdb::LogicalType::INTEGER);
  auto distanceFromClassMap =
      duckdb::Value::MAP(duckdb::LogicalType(duckdb::LogicalTypeId::INTEGER), listTypeDistanceToClass, classesForDistance, classesDistances);

  //
  //
  //
  auto jobId = duckdb::Value::UUID(jobIdStr);

  std::unique_ptr<duckdb::QueryResult> result = select(
      "INSERT INTO cache_analyze_settings (job_id, output_classes, measured_channels, intersecting_channels, distance_from_classes) VALUES (?, ?, ?, "
      "?, ?) ",
      jobId, outputClassesList, measuredChannelsMap, intersectingChannelsMap, distanceFromClassMap);

  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
}

///
/// \brief
/// \author    Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Database::createAnalyzeSettingsCache(const std::string &jobId)
{
  auto selectOutputClasses = [this]() -> std::set<enums::ClassId> {
    std::set<enums::ClassId> channels;
    std::unique_ptr<duckdb::QueryResult> result = select(
        "SELECT class_id FROM objects\n"
        "GROUP BY class_id;");
    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }
    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    for(size_t n = 0; n < materializedResult->RowCount(); n++) {
      enums::ClassId classID = ((enums::ClassId) materializedResult->GetValue(0, n).GetValue<uint16_t>());
      channels.emplace(classID);
    }
    return channels;
  };

  auto selectMeasurementChannelsForClasses = [this]() -> std::map<enums::ClassId, std::set<int32_t>> {
    std::map<enums::ClassId, std::set<int32_t>> channels;
    std::unique_ptr<duckdb::QueryResult> result = select(
        "SELECT class_id,object_measurements.meas_stack_c FROM objects\n"
        "JOIN object_measurements ON objects.object_id = object_measurements.object_id AND objects.image_id = object_measurements.image_id\n"
        "GROUP BY object_measurements.meas_stack_c,class_id;");
    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }
    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    for(size_t n = 0; n < materializedResult->RowCount(); n++) {
      enums::ClassId classID = ((enums::ClassId) materializedResult->GetValue(0, n).GetValue<uint16_t>());
      auto channelId         = (int32_t) materializedResult->GetValue(1, n).GetValue<uint32_t>();
      channels[classID].emplace(channelId);
    }
    return channels;
  };

  auto selectIntersectingClassForClasses = [this]() -> std::map<enums::ClassId, std::set<enums::ClassId>> {
    std::map<enums::ClassId, std::set<enums::ClassId>> channels;
    std::unique_ptr<duckdb::QueryResult> result = select(
        "SELECT DISTINCT \n"
        "parent.class_id AS class_id,\n"
        "child.class_id AS child_class_id\n"
        "FROM \n"
        "    objects AS parent\n"
        "JOIN \n"
        "    objects AS child\n"
        "ON \n"
        "    child.meas_parent_object_id = parent.object_id;");
    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }
    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    for(size_t n = 0; n < materializedResult->RowCount(); n++) {
      auto classID   = ((enums::ClassId) materializedResult->GetValue(0, n).GetValue<uint16_t>());
      auto channelId = (enums::ClassId) materializedResult->GetValue(1, n).GetValue<uint16_t>();
      channels[classID].emplace(channelId);
    }
    return channels;
  };

  auto selectDistanceClassForClasses = [this]() -> std::map<enums::ClassId, std::set<enums::ClassId>> {
    std::map<enums::ClassId, std::set<enums::ClassId>> channels;
    std::unique_ptr<duckdb::QueryResult> result = select(
        "SELECT class_id,meas_class_id FROM distance_measurements\n"
        "GROUP BY class_id,meas_class_id;");
    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }
    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    for(size_t n = 0; n < materializedResult->RowCount(); n++) {
      auto classID   = ((enums::ClassId) materializedResult->GetValue(0, n).GetValue<uint16_t>());
      auto channelId = (enums::ClassId) materializedResult->GetValue(1, n).GetValue<uint16_t>();
      channels[classID].emplace(channelId);
    }
    return channels;
  };

  setAnalyzeSettingsCache(jobId, selectOutputClasses(), selectMeasurementChannelsForClasses(), selectIntersectingClassForClasses(),
                          selectDistanceClassForClasses());
}

///
/// \brief
/// \author    Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Database::insetImageToGroup(uint16_t plateId, uint64_t imageId, uint16_t imageIdx, const joda::grp::GroupInformation &groupInfo)
{
  auto connection = acquire();
  auto prepare    = connection->Prepare("INSERT OR IGNORE INTO images_groups (plate_id, group_id, image_id, image_group_idx) VALUES (?, ?, ?, ?)");
  prepare->Execute(plateId, groupInfo.groupId, imageId, imageIdx);
}

///
/// \brief     Should be called at the very beginning of an analysis
/// \author    Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
std::string Database::startJob(const joda::settings::AnalyzeSettings &exp, const std::string &jobName)
{
  if(insertExperiment(exp.projectSettings.experimentSettings)) {
    insertClasses(exp.projectSettings.classification.classes);
  }
  std::string jobId = insertJobAndPlates(exp, jobName);

  setAnalyzeSettingsCache(jobId, exp.getOutputClasses(), exp.getImageChannelsUsedForMeasurement(), exp.getPossibleIntersectingClasses(),
                          exp.getPossibleDistanceClasses());
  return jobId;
}

///
/// \brief     Finish job
/// \author    Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Database::finishJob(const std::string &jobId)
{
  auto timestampFinished =
      duckdb::timestamp_t(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
  std::unique_ptr<duckdb::QueryResult> result =
      select("UPDATE jobs SET time_finished = ? WHERE job_id = ?", duckdb::Value::TIMESTAMP(timestampFinished), duckdb::Value::UUID(jobId));
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
bool Database::insertExperiment(const joda::settings::ExperimentSettings &exp)
{
  auto expIn = selectExperiment().experiment;
  if(exp.experimentId == expIn.experimentId) {
    joda::log::logInfo("Appending to existing experiment!");
    return false;
  }
  if(expIn.experimentId.empty()) {
    auto connection     = acquire();
    auto prepare        = connection->Prepare("INSERT INTO experiment (experiment_id, name, notes) VALUES (?, ?, ?)");
    nlohmann::json json = exp;
    prepare->Execute(duckdb::Value::UUID(exp.experimentId), exp.experimentName, exp.notes);
  } else {
    throw std::runtime_error(
        "It is not allowed to store more than one different experiment in the same database. Choose either a new "
        "database file or select as experiment id >" +
        expIn.experimentId + "<.");
  }
  return true;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectExperiment() -> AnalyzeMeta
{
  joda::settings::ExperimentSettings exp;
  std::chrono::system_clock::time_point timestampStart;
  std::chrono::system_clock::time_point timestampFinish;
  std::string settingsString;
  std::string jobName;
  std::string jobId;
  uint32_t tileWidth  = 0;
  uint32_t tileHeight = 0;
  uint32_t series     = 0;

  {
    std::unique_ptr<duckdb::QueryResult> result = select("SELECT experiment_id,name,notes FROM experiment");
    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }

    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    if(materializedResult->RowCount() > 0) {
      exp.experimentId   = duckdb::UUID::ToString(materializedResult->GetValue(0, 0).GetValue<duckdb::hugeint_t>());
      exp.experimentName = materializedResult->GetValue(1, 0).GetValue<std::string>();
      exp.notes          = materializedResult->GetValue(2, 0).GetValue<std::string>();
    }
  }

  {
    std::unique_ptr<duckdb::QueryResult> resultJobs = select(
        "SELECT time_started,time_finished,settings,job_name,job_id,settings_tile_width,settings_tile_height,settings_image_series FROM jobs ORDER "
        "BY time_started");
    if(resultJobs->HasError()) {
      throw std::invalid_argument(resultJobs->GetError());
    }
    auto materializedResult = resultJobs->Cast<duckdb::StreamQueryResult>().Materialize();
    if(materializedResult->RowCount() > 0) {
      {
        auto timestampDb = materializedResult->GetValue(0, 0).GetValue<duckdb::timestamp_t>();
        time_t epochTime = duckdb::Timestamp::GetEpochSeconds(timestampDb);
        timestampStart   = std::chrono::system_clock::from_time_t(epochTime);
      }
      {
        auto timestampDb = materializedResult->GetValue(1, 0).GetValue<duckdb::timestamp_t>();
        time_t epochTime = duckdb::Timestamp::GetEpochSeconds(timestampDb);
        timestampFinish  = std::chrono::system_clock::from_time_t(epochTime);
      }

      {
        settingsString = helper::base64Decode(materializedResult->GetValue(2, 0).GetValue<std::string>());
      }

      {
        jobName = materializedResult->GetValue(3, 0).GetValue<std::string>();
      }

      {
        jobId = duckdb::UUID::ToString(materializedResult->GetValue(4, 0).GetValue<duckdb::hugeint_t>());
      }

      tileWidth  = materializedResult->GetValue(5, 0).GetValue<uint32_t>();
      tileHeight = materializedResult->GetValue(6, 0).GetValue<uint32_t>();
      series     = materializedResult->GetValue(7, 0).GetValue<uint32_t>();
    }
  }

  return {.experiment                = exp,
          .timestampStart            = timestampStart,
          .timestampFinish           = timestampFinish,
          .jobName                   = jobName,
          .jobId                     = jobId,
          .analyzeSettingsJsonString = settingsString,
          .tileWidth                 = tileWidth,
          .tileHeight                = tileHeight,
          .series                    = series};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::string Database::insertJobAndPlates(const joda::settings::AnalyzeSettings &exp, const std::string &jobName)
{
  auto connection = acquire();
  connection->BeginTransaction();
  std::string jobIdStr = joda::helper::generate_uuid();
  auto jobId           = duckdb::Value::UUID(jobIdStr);

  //
  // If this was successful, insert the job
  //
  try {
    auto timestampStart =
        duckdb::timestamp_t(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    duckdb::timestamp_t nil = {};
    auto prepare            = connection->Prepare(
        "INSERT INTO jobs (experiment_id, job_id, job_name,imagec_version, time_started, time_finished, settings, settings_results_table_default, "
                   "settings_results_table, settings_tile_width, settings_tile_height, settings_image_series) "
                   "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    auto resultsTableSettings = exp.toResultsSettings();
    prepare->Execute(duckdb::Value::UUID(exp.projectSettings.experimentSettings.experimentId), jobId, jobName, std::string(Version::getVersion()),
                     duckdb::Value::TIMESTAMP(timestampStart), duckdb::Value::TIMESTAMP(nil), helper::base64Encode(settings::Settings::toString(exp)),
                     helper::base64Encode(settings::Settings::toString(resultsTableSettings)),
                     helper::base64Encode(settings::Settings::toString(resultsTableSettings)), exp.imageSetup.imageTileSettings.tileWidth,
                     exp.imageSetup.imageTileSettings.tileHeight, exp.imageSetup.series);
  } catch(const std::exception &ex) {
    connection->Rollback();
    throw std::runtime_error(ex.what());
  }

  // First try to insert plates
  try {
    auto platesDb          = duckdb::Appender(*connection, "plates");
    const auto &plate      = exp.projectSettings.plate;
    nlohmann::json groupBy = plate.groupBy;
    platesDb.BeginRow();
    platesDb.Append(jobId);                                                                          //       " job_id UUID,"
    platesDb.Append<uint16_t>(plate.plateId);                                                        //       " plate_id USMALLINT,"
    platesDb.Append<duckdb::string_t>(plate.name);                                                   //       " name STRING,"
    platesDb.Append<duckdb::string_t>(plate.notes);                                                  //       " notes STRING,"
    platesDb.Append<uint16_t>(plate.plateSetup.rows);                                                //       " rows USMALLINT,"
    platesDb.Append<uint16_t>(plate.plateSetup.cols);                                                //       " cols USMALLINT,"
    platesDb.Append<duckdb::string_t>(plate.imageFolder);                                            //       " image_folder STRING,"
    platesDb.Append<duckdb::string_t>(settings::vectorToString(plate.plateSetup.wellImageOrder));    //       " well_image_order STRING,"
    platesDb.Append<duckdb::string_t>(std::string(groupBy));                                         //       " group_by STRING,"
    platesDb.Append<duckdb::string_t>(plate.filenameRegex);                                          //       " filename_regex STRING,"
    platesDb.EndRow();

    platesDb.Close();
  } catch(const std::exception &ex) {
    connection->Rollback();
    throw std::runtime_error("A plate with same ID still exists in this experiment! What: " + std::string(ex.what()));
  }

  connection->Commit();
  return jobIdStr;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectPlates() -> std::map<uint16_t, joda::settings::Plate>
{
  std::unique_ptr<duckdb::QueryResult> result =
      select("SELECT plate_id, name, notes, rows, cols,image_folder,well_image_order,group_by,filename_regex FROM plates");
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();

  std::map<uint16_t, joda::settings::Plate> results;
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    joda::settings::Plate plate;
    plate.plateId                   = materializedResult->GetValue(0, n).GetValue<uint16_t>();
    plate.name                      = materializedResult->GetValue(1, n).GetValue<std::string>();
    plate.notes                     = materializedResult->GetValue(2, n).GetValue<std::string>();
    plate.plateSetup.rows           = materializedResult->GetValue(3, n).GetValue<uint16_t>();
    plate.plateSetup.cols           = materializedResult->GetValue(4, n).GetValue<uint16_t>();
    plate.imageFolder               = materializedResult->GetValue(5, n).GetValue<std::string>();
    plate.plateSetup.wellImageOrder = joda::settings::stringToVector(materializedResult->GetValue(6, n).GetValue<std::string>());
    nlohmann::json groupBy          = materializedResult->GetValue(7, n).GetValue<std::string>();
    plate.groupBy                   = groupBy.template get<enums::GroupBy>();
    plate.filenameRegex             = materializedResult->GetValue(8, n).GetValue<std::string>();
    results.try_emplace(plate.plateId, plate);
  }

  return results;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectGroups() -> std::map<uint16_t, std::string>
{
  std::unique_ptr<duckdb::QueryResult> result = select("SELECT group_id, name FROM groups");
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();

  std::map<uint16_t, std::string> results;
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    uint16_t plateId = materializedResult->GetValue(0, n).GetValue<uint16_t>();
    std::string name = materializedResult->GetValue(1, n).GetValue<std::string>();

    results.emplace(plateId, name);
  }

  return results;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Database::insertClasses(const std::list<settings::Class> &classesIn)
{
  auto connection = acquire();
  auto classes    = duckdb::Appender(*connection, "classes");
  for(const auto &classs : classesIn) {
    nlohmann::json j = classs.classId;
    classes.BeginRow();
    classes.Append<uint16_t>(static_cast<uint16_t>(classs.classId));    //        "	class_id USMALLINT,"
    classes.Append<duckdb::string_t>(std::string(j));                   //         " short_name STRING,"
    classes.Append<duckdb::string_t>(classs.name);                      //         " name STRING,"
    classes.Append<duckdb::string_t>(classs.notes);                     //         " notes STRING"
    classes.Append<duckdb::string_t>(classs.color);                     //         " color STRING"
    classes.EndRow();
  }
  classes.Close();
}

void Database::insertGroup()
{
}

void Database::flatten(const std::vector<cv::Point> &contour, duckdb::vector<duckdb::Value> &flattenPointsOut)
{
  flattenPointsOut.reserve(contour.size() * 2);
  for(const auto &point : contour) {
    flattenPointsOut.push_back(duckdb::Value::UINTEGER(point.x));
    flattenPointsOut.push_back(duckdb::Value::UINTEGER(point.y));
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectImageChannels() -> std::map<uint32_t, joda::ome::OmeInfo::ChannelInfo>
{
  std::unique_ptr<duckdb::QueryResult> result = select("SELECT image_id, stack_c, name FROM images_channels");
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();

  std::map<uint32_t, joda::ome::OmeInfo::ChannelInfo> results;
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    joda::ome::OmeInfo::ChannelInfo tmp;
    uint32_t cidx = materializedResult->GetValue(1, n).GetValue<uint32_t>();
    tmp.name      = materializedResult->GetValue(2, n).GetValue<std::string>();
    results.try_emplace(cidx, tmp);
  }

  return results;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectNrOfTimeStacks() -> int32_t
{
  std::unique_ptr<duckdb::QueryResult> result = select("SELECT MAX(nr_of_t_Stacks) FROM images");
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  if(materializedResult->RowCount() > 0) {
    return materializedResult->GetValue(0, 0).GetValue<uint32_t>();
  }

  return 0;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectGroupInfo(uint64_t groupId) -> GroupInfo
{
  std::unique_ptr<duckdb::QueryResult> result = select(
      "SELECT groups.name, groups.pos_on_plate_x, groups.pos_on_plate_y\n"
      "FROM groups\n"
      "WHERE groups.group_id = ?",
      groupId);
  if(result->HasError()) {
    throw std::invalid_argument("selectGroupInfo:" + result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();

  GroupInfo results;
  if(materializedResult->RowCount() > 0) {
    results.groupName = materializedResult->GetValue(0, 0).GetValue<std::string>();
    results.posX      = materializedResult->GetValue(1, 0).GetValue<uint32_t>();
    results.posY      = materializedResult->GetValue(2, 0).GetValue<uint32_t>();
  }

  return results;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectImageInfo(uint64_t imageId) -> ImageInfo
{
  std::unique_ptr<duckdb::QueryResult> result = select(
      "SELECT images.file_name, images.original_file_path,images.relative_file_path, images.validity, images.width, images.height, groups.name "
      "FROM images "
      "JOIN images_groups ON "
      "     images.image_id = images_groups.image_id "
      "JOIN groups ON "
      "     images_groups.group_id = groups.group_id "
      "WHERE images.image_id = ?",
      imageId);
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();

  ImageInfo results;
  if(materializedResult->RowCount() > 0) {
    results.filename         = materializedResult->GetValue(0, 0).GetValue<std::string>();
    results.imageFilePath    = materializedResult->GetValue(1, 0).GetValue<std::string>();
    results.imageFilePathRel = materializedResult->GetValue(2, 0).GetValue<std::string>();
    results.validity         = materializedResult->GetValue(3, 0).GetValue<uint64_t>();
    results.width            = materializedResult->GetValue(4, 0).GetValue<uint32_t>();
    results.height           = materializedResult->GetValue(5, 0).GetValue<uint32_t>();
    results.imageGroupName   = materializedResult->GetValue(6, 0).GetValue<std::string>();
    results.imageId          = imageId;
  }

  return results;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectImages() -> std::vector<ImageInfo>
{
  std::unique_ptr<duckdb::QueryResult> result = select(
      "SELECT images.file_name,images.original_file_path,images.relative_file_path,images.validity, images.width, images.height, groups.name "
      "FROM images "
      "JOIN images_groups ON "
      "     images.image_id = images_groups.image_id "
      "JOIN groups ON "
      "     images_groups.group_id = groups.group_id ");
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();

  std::vector<ImageInfo> results;
  for(int n = 0; n < materializedResult->RowCount(); n++) {
    ImageInfo info;
    info.filename         = materializedResult->GetValue(0, n).GetValue<std::string>();
    info.imageFilePath    = materializedResult->GetValue(1, n).GetValue<std::string>();
    info.imageFilePathRel = materializedResult->GetValue(2, n).GetValue<std::string>();
    info.validity         = materializedResult->GetValue(3, n).GetValue<uint64_t>();
    info.width            = materializedResult->GetValue(4, n).GetValue<uint32_t>();
    info.height           = materializedResult->GetValue(5, n).GetValue<uint32_t>();
    info.imageGroupName   = materializedResult->GetValue(6, n).GetValue<std::string>();
    results.push_back(info);
  }

  return results;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectObjectInfo(uint64_t objectId) -> ObjectInfo
{
  std::unique_ptr<duckdb::QueryResult> result = select(
      "SELECT stack_c, stack_z, stack_t, meas_center_x, meas_center_y, meas_box_x, meas_box_y, meas_box_width, meas_box_height, image_id\n"
      "FROM objects "
      "WHERE object_id = ?",
      objectId);
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();

  ObjectInfo results;
  if(materializedResult->RowCount() > 0) {
    results.stackC        = materializedResult->GetValue(0, 0).GetValue<uint32_t>();
    results.stackZ        = materializedResult->GetValue(1, 0).GetValue<uint32_t>();
    results.stackT        = materializedResult->GetValue(2, 0).GetValue<uint32_t>();
    results.measCenterX   = materializedResult->GetValue(3, 0).GetValue<uint32_t>();
    results.measCenterY   = materializedResult->GetValue(4, 0).GetValue<uint32_t>();
    results.measBoxX      = materializedResult->GetValue(5, 0).GetValue<uint32_t>();
    results.measBoxY      = materializedResult->GetValue(6, 0).GetValue<uint32_t>();
    results.measBoxWidth  = materializedResult->GetValue(7, 0).GetValue<uint32_t>();
    results.measBoxHeight = materializedResult->GetValue(8, 0).GetValue<uint32_t>();
    results.imageId       = materializedResult->GetValue(9, 0).GetValue<uint64_t>();
  }

  return results;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectClasses() -> std::map<enums::ClassId, joda::settings::Class>
{
  std::unique_ptr<duckdb::QueryResult> result = select("SELECT class_id, name, notes, color FROM classes");
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();

  std::map<enums::ClassId, joda::settings::Class> results;
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    joda::settings::Class tmp;
    tmp.classId = static_cast<enums::ClassId>(materializedResult->GetValue(0, n).GetValue<uint16_t>());
    tmp.name    = materializedResult->GetValue(1, n).GetValue<std::string>();
    tmp.notes   = materializedResult->GetValue(2, n).GetValue<std::string>();
    tmp.color   = materializedResult->GetValue(3, n).GetValue<std::string>();
    results.try_emplace(tmp.classId, tmp);
  }

  return results;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectMeasurementChannelsForClasses() -> std::map<enums::ClassId, std::set<int32_t>>
{
  std::map<enums::ClassId, std::set<int32_t>> channels;
  std::unique_ptr<duckdb::QueryResult> result = select("SELECT measured_channels FROM cache_analyze_settings\n");
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  return duckdbMapToMap<enums::ClassId, int32_t>(materializedResult);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectOutputClasses() -> std::set<enums::ClassId>
{
  std::set<enums::ClassId> channels;
  std::unique_ptr<duckdb::QueryResult> result = select("SELECT output_classes FROM cache_analyze_settings\n");
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  if(materializedResult->RowCount() > 0) {
    duckdb::Value value = materializedResult->GetValue(0, 0);
    auto children       = duckdb::MapValue::GetChildren(value);
    for(int n = 0; n < children.size(); n++) {
      channels.emplace(static_cast<enums::ClassId>(children[n].GetValue<int32_t>()));
    }
  }
  return channels;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectIntersectingClassForClasses() -> std::map<enums::ClassId, std::set<enums::ClassId>>
{
  std::map<enums::ClassId, std::set<enums::ClassId>> channels;
  std::unique_ptr<duckdb::QueryResult> result = select("SELECT intersecting_channels FROM cache_analyze_settings\n");
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  return duckdbMapToMap<enums::ClassId, enums::ClassId>(materializedResult);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectDistanceClassForClasses() -> std::map<enums::ClassId, std::set<enums::ClassId>>
{
  std::map<enums::ClassId, std::set<enums::ClassId>> channels;
  std::unique_ptr<duckdb::QueryResult> result = select("SELECT distance_from_classes FROM cache_analyze_settings\n");
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  return duckdbMapToMap<enums::ClassId, enums::ClassId>(materializedResult);
}

///
/// \brief    Select those classes which have at least one time the same tracking id
/// \todo     Cache the result
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectColocalizingClasses() -> std::set<std::set<enums::ClassId>>
{
  std::map<enums::ClassId, std::set<enums::ClassId>> channels;
  std::unique_ptr<duckdb::QueryResult> result = select(
      "SELECT DISTINCT STRING_AGG(class_id::text,',') as elements FROM objects\n"
      "WHERE meas_tracking_id !=0\n"
      "GROUP BY meas_tracking_id");
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();

  std::set<std::set<enums::ClassId>> ret;
  for(int32_t row = 0; row < materializedResult->RowCount(); row++) {
    auto listOfClasses = materializedResult->GetValue(0, row).GetValue<std::string>();
    auto classesStr    = joda::helper::split(listOfClasses, {','});
    std::set<enums::ClassId> classesHavingCommonTrackingId;
    for(const auto &classStr : classesStr) {
      int32_t classsNr = std::stoi(classStr);
      classesHavingCommonTrackingId.emplace(static_cast<enums::ClassId>(classsNr));
    }
    ret.emplace(classesHavingCommonTrackingId);
  }
  return ret;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Database::updateResultsTableSettings(const std::string &jobId, const std::string &settings)
{
  auto timestampFinished =
      duckdb::timestamp_t(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
  std::unique_ptr<duckdb::QueryResult> result =
      select("UPDATE jobs SET settings_results_table = ? WHERE job_id = ?", helper::base64Encode(settings), duckdb::Value::UUID(jobId));
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectResultsTableSettings(const std::string &jobId) -> std::string
{
  {
    std::unique_ptr<duckdb::QueryResult> resultJobs = select("SELECT settings_results_table FROM jobs WHERE job_id = ?", duckdb::Value::UUID(jobId));
    if(resultJobs->HasError()) {
      throw std::invalid_argument(resultJobs->GetError());
    }
    auto materializedResult = resultJobs->Cast<duckdb::StreamQueryResult>().Materialize();
    if(materializedResult->RowCount() > 0) {
      {
        return helper::base64Decode(materializedResult->GetValue(0, 0).GetValue<std::string>());
      }
    }
  }
  return "";
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectImageIdFromImageFileName(const std::string &imageFileName) -> uint64_t
{
  {
    std::unique_ptr<duckdb::QueryResult> resultJobs = select("SELECT image_id FROM images WHERE file_name = ?", imageFileName);
    if(resultJobs->HasError()) {
      throw std::invalid_argument(resultJobs->GetError());
    }
    auto materializedResult = resultJobs->Cast<duckdb::StreamQueryResult>().Materialize();
    if(materializedResult->RowCount() > 0) {
      {
        return materializedResult->GetValue(0, 0).GetValue<uint64_t>();
      }
    }
  }
  return 0;
}

}    // namespace joda::db
