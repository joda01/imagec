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
#include <chrono>
#include <exception>
#include <stdexcept>
#include <string>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/enums/enums_grouping.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/file_grouper/file_grouper.hpp"
#include "backend/helper/file_grouper/file_grouper_types.hpp"
#include "backend/helper/fnv1a.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/helper/rle/rle.hpp"
#include "backend/helper/uuid.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include <duckdb/common/types/string_type.hpp>
#include <duckdb/common/types/value.hpp>
#include <duckdb/common/types/vector.hpp>
#include <duckdb/main/appender.hpp>
#include <nlohmann/json_fwd.hpp>

namespace joda::db {

/////////////////////////////////////////////////////
void Database::openDatabase(const std::filesystem::path &pathToDb)
{
  mDbCfg.SetOption("temp_directory", pathToDb.parent_path().string());
  mDb = std::make_unique<duckdb::DuckDB>(pathToDb.string(), &mDbCfg);
  createTables();
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
      " time_started TIMESTAMP,"
      " time_finished TIMESTAMP,"
      " settings TEXT,"
      " PRIMARY KEY (job_id),"
      " FOREIGN KEY(experiment_id) REFERENCES experiment(experiment_id)"
      ");"

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

      "CREATE TABLE IF NOT EXISTS clusters ("
      "	cluster_id USMALLINT,"
      " short_name STRING,"
      " name STRING,"
      " notes STRING,"
      " color STRING,"
      " PRIMARY KEY (cluster_id)"
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

      "CREATE TABLE IF NOT EXISTS clusters_planes ("
      " image_id UBIGINT,"
      "	cluster_id USMALLINT,"
      " stack_c UINTEGER, "
      " stack_z UINTEGER, "
      " stack_t UINTEGER, "
      " validity UBIGINT,"
      " PRIMARY KEY (image_id,cluster_id, stack_c, stack_z, stack_t),"
      " FOREIGN KEY(image_id) REFERENCES images(image_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS objects ("
      "	image_id UBIGINT,"
      " object_id UBIGINT,"
      " cluster_id USMALLINT,"
      " class_id USMALLINT,"
      " stack_c UINTEGER,"
      " stack_z UINTEGER,"
      " stack_t UINTEGER,"
      " meas_confidence float,"
      " meas_area_size DOUBLE,"
      " meas_perimeter float,"
      " meas_circularity float,"
      " meas_center_x UINTEGER,"
      " meas_center_y UINTEGER,"
      " meas_center_z UINTEGER,"
      " meas_box_width UINTEGER,"
      " meas_box_height UINTEGER,"
      " meas_box_depth UINTEGER,"
      " meas_mask MAP(UINTEGER,BOOLEAN),"
      " meas_contour UINTEGER[]"
      ");"

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

      "CREATE TABLE IF NOT EXISTS object_intersections ("
      "	image_id UBIGINT,"
      " object_id UBIGINT,"
      " meas_object_id UBIGINT"
      ");"

      "CREATE TABLE IF NOT EXISTS statistics ("
      "	image_id UBIGINT,"
      " cluster_id USMALLINT,"
      " class_id USMALLINT,"
      " stack_c UINTEGER,"
      " stack_z UINTEGER,"
      " stack_t UINTEGER,"

      " meas_cnt UINTEGER,"

      " avg_confidence float,"
      " max_confidence float,"
      " min_confidence float,"
      " sum_confidence float,"
      " median_confidence float,"
      " stddev_confidence float,"

      " avg_area_size float,"
      " max_area_size float,"
      " min_area_size float,"
      " sum_area_size float,"
      " median_area_size float,"
      " stddev_area_size float,"

      " avg_perimeter float,"
      " max_perimeter float,"
      " min_perimeter float,"
      " sum_perimeter float,"
      " median_perimeter float,"
      " stddev_perimeter float,"

      " avg_circularity float,"
      " max_circularity float,"
      " min_circularity float,"
      " sum_circularity float,"
      " median_circularity float,"
      " stddev_circularity float,"
      ");"

      "CREATE TABLE IF NOT EXISTS statistic_measurements ("
      "	image_id UBIGINT,"
      " cluster_id USMALLINT,"
      " class_id USMALLINT,"
      " stack_c UINTEGER,"
      " stack_z UINTEGER,"
      " stack_t UINTEGER,"
      " meas_stack_c UINTEGER,"
      " meas_stack_z UINTEGER,"
      " meas_stack_t UINTEGER,"

      " meas_cnt UINTEGER,"

      " avg_intensity_sum DOUBLE,"
      " avg_intensity_avg DOUBLE,"
      " avg_intensity_min DOUBLE,"
      " avg_intensity_max DOUBLE,"

      " max_intensity_sum DOUBLE,"
      " max_intensity_avg DOUBLE,"
      " max_intensity_min DOUBLE,"
      " max_intensity_max DOUBLE,"

      " min_intensity_sum DOUBLE,"
      " min_intensity_avg DOUBLE,"
      " min_intensity_min DOUBLE,"
      " min_intensity_max DOUBLE,"

      " sum_intensity_sum DOUBLE,"
      " sum_intensity_avg DOUBLE,"
      " sum_intensity_min DOUBLE,"
      " sum_intensity_max DOUBLE,"

      " median_intensity_sum DOUBLE,"
      " median_intensity_avg DOUBLE,"
      " median_intensity_min DOUBLE,"
      " median_intensity_max DOUBLE,"

      " stddev_intensity_sum DOUBLE,"
      " stddev_intensity_avg DOUBLE,"
      " stddev_intensity_min DOUBLE,"
      " stddev_intensity_max DOUBLE"
      ");"

      "CREATE TABLE IF NOT EXISTS statistic_intersections ("
      "	image_id UBIGINT,"
      " cluster_id USMALLINT,"
      " class_id USMALLINT,"
      " stack_c UINTEGER,"
      " stack_z UINTEGER,"
      " stack_t UINTEGER,"
      " meas_stack_c UINTEGER,"
      " meas_stack_z UINTEGER,"
      " meas_stack_t UINTEGER,"
      " meas_cluster_id USMALLINT,"
      " meas_class_id USMALLINT,"

      " meas_cnt UINTEGER,"
      " meas_sum UINTEGER,"
      " meas_avg UINTEGER,"
      " meas_min UINTEGER,"
      " meas_max UINTEGER,"
      " meas_median UINTEGER,"
      " meas_stddev UINTEGER,"
      ");"

      ;

  auto connection = acquire();
  auto result     = connection->Query(create_table_sql);
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
}

void Database::insertObjects(const joda::processor::ImageContext &imgContext, const joda::atom::ObjectList &objectsList)
{
  auto connection = acquire();
  // connection->BeginTransaction();
  auto objects              = duckdb::Appender(*connection, "objects");
  auto object_measurements  = duckdb::Appender(*connection, "object_measurements");
  auto object_intersections = duckdb::Appender(*connection, "object_intersections");

  for(const auto &[_, obj] : objectsList) {
    for(const auto &roi : obj) {
      objects.BeginRow();
      // Primary key
      objects.Append<uint64_t>(imgContext.imageId);               // "	image_id UBIGINT,"
      objects.Append<uint64_t>(roi.getObjectId());                // " object_id UBIGINT,"
      objects.Append<uint16_t>((uint16_t) roi.getClusterId());    // " cluster_id USMALLINT,"
      objects.Append<uint16_t>((uint16_t) roi.getClassId());      // " class_id USMALLINT,"
      objects.Append<uint32_t>(roi.getC());                       // " stack_c UINTEGER,"
      objects.Append<uint32_t>(roi.getZ());                       // " stack_z UINTEGER,"
      objects.Append<uint32_t>(roi.getT());                       // " stack_t UINTEGER,"
      // Data
      objects.Append<float>(roi.getConfidence());                   // " meas_confidence float,"
      objects.Append<double>(roi.getAreaSize());                    // " meas_area_size DOUBLE,"
      objects.Append<float>(roi.getPerimeter());                    // " meas_perimeter float,"
      objects.Append<float>(roi.getCircularity());                  // " meas_circularity float,"
      objects.Append<uint32_t>(roi.getCenterOfMassReal().x);        // " meas_center_x UINTEGER,"
      objects.Append<uint32_t>(roi.getCenterOfMassReal().y);        // " meas_center_y UINTEGER,"
      objects.Append<uint32_t>(0);                                  // " meas_center_z UINTEGER,"
      objects.Append<uint32_t>(roi.getBoundingBoxReal().width);     // " meas_box_width UINTEGER,"
      objects.Append<uint32_t>(roi.getBoundingBoxReal().height);    // " meas_box_height UINTEGER,"
      objects.Append<uint32_t>(0);                                  // " meas_box_depth UINTEGER,"

      auto mask = duckdb::Value::MAP(duckdb::LogicalType(duckdb::LogicalTypeId::UINTEGER),
                                     duckdb::LogicalType(duckdb::LogicalTypeId::BOOLEAN), {}, {});
      objects.Append<duckdb::Value>(mask);
      /* objects.Append<duckdb::Value>(
           joda::rle::rle_encode({roi.getMask().datastart, roi.getMask().dataend}));    // " meas_mask BOOLEAN[]"*/

      duckdb::vector<duckdb::Value> flattenPoints;
      // flatten(roi.getContour(), flattenPoints);
      auto contour = duckdb::Value::LIST(duckdb::LogicalType(duckdb::LogicalTypeId::UINTEGER), flattenPoints);
      objects.Append<duckdb::Value>(contour);    // " meas_contour UINTEGER[]"
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
      // Intersections
      //
      for(const auto &intersectingRoi : roi.getIntersections()) {
        object_intersections.BeginRow();
        // Primary key
        object_intersections.Append<uint64_t>(imgContext.imageId);    //       "	image_id UBIGINT,"
        object_intersections.Append<uint64_t>(roi.getObjectId());     //       " object_id UBIGINT,"
        //  Data
        object_intersections.Append<uint64_t>(intersectingRoi.objectId);    // meas_object_id UBIGINT
        object_intersections.EndRow();
      }
    }
  }
  objects.Close();
  object_measurements.Close();
  object_intersections.Close();

  //
  // Statistics
  //
  // auto statistics             = duckdb::Appender(*connection, "statistics");
  // auto statistic_measurements = duckdb::Appender(*connection, "statistic_measurements");
  // statistics.Close();
  // statistic_measurements.Close();
}

auto Database::prepareImages(uint8_t plateId, enums::GroupBy groupBy, const std::string &filenameRegex,
                             const std::vector<std::filesystem::path> &imagePaths)
    -> std::vector<std::tuple<std::filesystem::path, joda::ome::OmeInfo, uint64_t>>
{
  std::vector<std::tuple<std::filesystem::path, joda::ome::OmeInfo, uint64_t>> imagesToProcess;
  joda::grp::FileGrouper grouper(groupBy, filenameRegex);

  auto connection      = acquire();
  auto groups          = duckdb::Appender(*connection, "groups");
  auto images          = duckdb::Appender(*connection, "images");
  auto images_groups   = duckdb::Appender(*connection, "images_groups");
  auto images_channels = duckdb::Appender(*connection, "images_channels");
  std::set<uint16_t> addedGroups;

  //
  // Preparing -> Insert all images to database
  //
  for(const auto &imagePath : imagePaths) {
    auto ome         = joda::image::reader::ImageReader::getOmeInformation(imagePath);
    uint64_t imageId = joda::helper::fnv1a(imagePath.string());

    imagesToProcess.emplace_back(imagePath, ome, imageId);
    auto groupInfo = grouper.getGroupForFilename(imagePath);
    // Group
    {
      if(!addedGroups.contains(groupInfo.groupId)) {
        groups.BeginRow();
        groups.Append<uint16_t>(plateId);                   //       " plate_id USMALLINT,"
        groups.Append<uint16_t>(groupInfo.groupId);         //       " group_id USMALLINT,"
        groups.Append<std::string>(groupInfo.groupName);    //       " name STRING,"
        groups.Append<std::string>("");                     //       " notes STRING,"
        groups.Append<uint32_t>(groupInfo.wellPosX);        //       " pos_on_plate_x UINTEGER,"
        groups.Append<uint32_t>(groupInfo.wellPosX);        //       " pos_on_plate_y UINTEGER,"
        groups.EndRow();
        addedGroups.emplace(groupInfo.groupId);
      }
    }

    // Image
    {
      images.BeginRow();
      images.Append<uint64_t>(imageId);                             //       " image_id UBIGINT,"
      images.Append<std::string>(imagePath.filename().string());    //       " file_name TEXT,"
      images.Append<std::string>(imagePath.string());               //       " original_file_path TEXT
      images.Append<uint32_t>(ome.getNrOfChannels());               //       " nr_of_c_stacks UINTEGER
      images.Append<uint32_t>(ome.getNrOfZStack());                 //       " nr_of_z_stacks UINTEGER
      images.Append<uint32_t>(ome.getNrOfTStack());                 //       " nr_of_t_stacks UINTEGER
      images.Append<uint32_t>(std::get<0>(ome.getSize()));          //       " width UINTEGER,"
      images.Append<uint32_t>(std::get<1>(ome.getSize()));          //       " height UINTEGER,"
      images.Append<uint64_t>(0);                                   //       " validity UBIGINT,"
      images.Append<bool>(false);                                   //       " processed BOOL,"
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
      for(const auto &[channelId, channel] : ome.getChannelInfos()) {
        images_channels.BeginRow();
        images_channels.Append<uint64_t>(imageId);                      // " image_id UBIGINT,"
        images_channels.Append<uint32_t>(channelId);                    // " stack_c UINTEGER, "
        images_channels.Append<duckdb::string_t>(channel.channelId);    // " channel_id TEXT,"
        images_channels.Append<duckdb::string_t>(channel.name);         // " name TEXT,"
        images_channels.EndRow();
      }
    }
  }

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

  auto [width, heigh] = image.imageMeta.getSize();
  prepare->Execute(image.imageId, image.imagePath.filename().string(), image.imagePath.string(),
                   image.imageMeta.getNrOfChannels(), image.imageLoader.getNrOfZStacksToProcess(),
                   image.imageLoader.getNrOfTStacksToProcess(), width, heigh, 0);
}

///
/// \brief
/// \author    Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Database::insertImageChannels(uint64_t imageId, const joda::ome::OmeInfo &ome)
{
  auto connection = acquire();
  auto channelsDb = duckdb::Appender(*connection, "images_channels");
  for(const auto &[channelId, channel] : ome.getChannelInfos()) {
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
void Database::insertImagePlane(uint64_t imageId, const enums::PlaneId &planeId,
                                const ome::OmeInfo::ImagePlane &planeInfo)
{
  auto connection = acquire();
  auto prepare    = connection->Prepare(
      "INSERT OR IGNORE INTO images_planes (image_id, stack_c, stack_z, stack_t, validity) VALUES (?, ?, ?, ?, ?)");
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
  std::unique_ptr<duckdb::QueryResult> result = select("UPDATE images SET validity = validity | ? WHERE image_id=?",
                                                       static_cast<uint64_t>(validity.to_ullong()), imageId);
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
  std::unique_ptr<duckdb::QueryResult> result = select("UPDATE images SET validity = validity & ~(?) WHERE image_id=?",
                                                       static_cast<uint64_t>(validity.to_ullong()), imageId);
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
  std::unique_ptr<duckdb::QueryResult> result = select(
      "UPDATE images_planes SET validity = validity | ? WHERE image_id=? AND stack_c=? AND stack_z=? AND stack_t=?",
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
void Database::setImagePlaneClusterClusterValidity(uint64_t imageId, const enums::PlaneId &planeId,
                                                   enums::ClusterId clusterId, enums::ChannelValidity validity)
{
  std::unique_ptr<duckdb::QueryResult> result = select(
      "INSERT INTO clusters_planes (image_id, cluster_id, stack_c, stack_z, stack_t, validity) VALUES (?, ?, ?, ?, ?) "
      "ON CONFLICT DO UPDATE SET validity = validity | ?",
      imageId, static_cast<uint16_t>(clusterId), planeId.cStack, planeId.zStack, planeId.tStack,
      static_cast<uint64_t>(validity.to_ullong()), static_cast<uint64_t>(validity.to_ullong()));
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
void Database::insetImageToGroup(uint16_t plateId, uint64_t imageId, uint16_t imageIdx,
                                 const joda::grp::GroupInformation &groupInfo)
{
  auto connection = acquire();
  auto prepare    = connection->Prepare(
      "INSERT OR IGNORE INTO images_groups (plate_id, group_id, image_id, image_group_idx) VALUES (?, ?, ?, ?)");
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
    insertClusters(exp.projectSettings.clusters);
    insertClasses(exp.projectSettings.classes);
  }
  return insertJobAndPlates(exp, jobName);
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
  auto timestampFinished = duckdb::timestamp_t(std::chrono::duration_cast<std::chrono::microseconds>(
                                                   std::chrono::high_resolution_clock::now().time_since_epoch())
                                                   .count());
  std::unique_ptr<duckdb::QueryResult> result =
      select("UPDATE jobs SET time_finished = ? WHERE job_id = ?", duckdb::Value::TIMESTAMP(timestampFinished),
             duckdb::Value::UUID(jobId));
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
  std::chrono::system_clock::time_point timestamp;
  {
    std::unique_ptr<duckdb::QueryResult> result = select("SELECT experiment_id,name,notes FROM experiment");
    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }

    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    if(materializedResult->RowCount() > 0) {
      exp.experimentId   = materializedResult->GetValue(0, 0).GetValue<std::string>();
      exp.experimentName = materializedResult->GetValue(1, 0).GetValue<std::string>();
      exp.experimentName = materializedResult->GetValue(1, 0).GetValue<std::string>();
    }
  }

  {
    std::unique_ptr<duckdb::QueryResult> resultJobs = select("SELECT time_started FROM jobs ORDER BY time_started");
    if(resultJobs->HasError()) {
      throw std::invalid_argument(resultJobs->GetError());
    }
    auto materializedResult = resultJobs->Cast<duckdb::StreamQueryResult>().Materialize();
    if(materializedResult->RowCount() > 0) {
      auto timestampDb = materializedResult->GetValue(0, 0).GetValue<duckdb::timestamp_t>();
      std::chrono::microseconds duration(timestampDb.value);
      timestamp = std::chrono::system_clock::from_time_t(duration.count());
    }
  }

  return {.experiment = exp, .timestamp = timestamp};
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
    auto timestampStart     = duckdb::timestamp_t(std::chrono::duration_cast<std::chrono::microseconds>(
                                                  std::chrono::high_resolution_clock::now().time_since_epoch())
                                                      .count());
    duckdb::timestamp_t nil = {};
    auto prepare            = connection->Prepare(
        "INSERT INTO jobs (experiment_id, job_id,job_name, time_started, time_finished, settings) VALUES (?, ?, ?, ?, "
                   "?, "
                   "?)");

    nlohmann::json json = exp;
    prepare->Execute(duckdb::Value::UUID(exp.projectSettings.experimentSettings.experimentId), jobId, jobName,
                     duckdb::Value::TIMESTAMP(timestampStart), duckdb::Value::TIMESTAMP(nil),
                     static_cast<std::string>(json.dump()));
  } catch(const std::exception &ex) {
    connection->Rollback();
    throw std::runtime_error(ex.what());
  }

  // First try to insert plates
  try {
    auto platesDb = duckdb::Appender(*connection, "plates");
    for(const auto &plate : exp.projectSettings.plates) {
      nlohmann::json groupBy = plate.groupBy;
      platesDb.BeginRow();
      platesDb.Append(jobId);                                  //       " job_id UUID,"
      platesDb.Append<uint16_t>(plate.plateId);                //       " plate_id USMALLINT,"
      platesDb.Append<duckdb::string_t>(plate.name);           //       " name STRING,"
      platesDb.Append<duckdb::string_t>(plate.notes);          //       " notes STRING,"
      platesDb.Append<uint16_t>(plate.rows);                   //       " rows USMALLINT,"
      platesDb.Append<uint16_t>(plate.cols);                   //       " cols USMALLINT,"
      platesDb.Append<duckdb::string_t>(plate.imageFolder);    //       " image_folder STRING,"
      platesDb.Append<duckdb::string_t>(
          settings::vectorToString(plate.wellImageOrder));        //       " well_image_order STRING,"
      platesDb.Append<duckdb::string_t>(std::string(groupBy));    //       " group_by STRING,"
      platesDb.Append<duckdb::string_t>(plate.filenameRegex);     //       " filename_regex STRING,"
      platesDb.EndRow();
    }
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
  std::unique_ptr<duckdb::QueryResult> result = select(
      "SELECT plate_id, name, notes, rows, cols,image_folder,well_image_order,group_by,filename_regex FROM plates");
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();

  std::map<uint16_t, joda::settings::Plate> results;
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    joda::settings::Plate plate;
    plate.plateId          = materializedResult->GetValue(0, n).GetValue<uint16_t>();
    plate.name             = materializedResult->GetValue(1, n).GetValue<std::string>();
    plate.notes            = materializedResult->GetValue(2, n).GetValue<std::string>();
    plate.rows             = materializedResult->GetValue(3, n).GetValue<uint16_t>();
    plate.cols             = materializedResult->GetValue(4, n).GetValue<uint16_t>();
    plate.imageFolder      = materializedResult->GetValue(5, n).GetValue<std::string>();
    plate.wellImageOrder   = joda::settings::stringToVector(materializedResult->GetValue(6, n).GetValue<std::string>());
    nlohmann::json groupBy = materializedResult->GetValue(7, n).GetValue<std::string>();
    plate.groupBy          = groupBy.template get<enums::GroupBy>();
    plate.filenameRegex    = materializedResult->GetValue(8, n).GetValue<std::string>();
    results.try_emplace(plate.plateId, plate);
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
void Database::insertClusters(const std::list<settings::Cluster> &clustersIn)
{
  auto connection = acquire();
  auto clusters   = duckdb::Appender(*connection, "clusters");
  for(const auto &cluster : clustersIn) {
    nlohmann::json j = cluster.clusterId;
    std::string cid  = j;
    clusters.BeginRow();
    clusters.Append<uint16_t>(static_cast<uint16_t>(cluster.clusterId));    //        "	class_id USMALLINT,"
    clusters.Append<duckdb::string_t>(cid);                                 //         " short_name STRING,"
    clusters.Append<duckdb::string_t>(cluster.name);                        //         " name STRING,"
    clusters.Append<duckdb::string_t>(cluster.notes);                       //         " notes STRING"
    clusters.Append<duckdb::string_t>(cluster.color);                       //         " color STRING"
    clusters.EndRow();
  }
  clusters.Close();
}
void Database::insertClasses(const std::list<settings::Class> &classesIn)
{
  auto connection = acquire();
  auto clusters   = duckdb::Appender(*connection, "classes");
  for(const auto &classs : classesIn) {
    nlohmann::json j = classs.classId;
    clusters.BeginRow();
    clusters.Append<uint16_t>(static_cast<uint16_t>(classs.classId));    //        "	class_id USMALLINT,"
    clusters.Append<duckdb::string_t>(std::string(j));                   //         " short_name STRING,"
    clusters.Append<duckdb::string_t>(classs.name);                      //         " name STRING,"
    clusters.Append<duckdb::string_t>(classs.notes);                     //         " notes STRING"
    clusters.Append<duckdb::string_t>(classs.color);                     //         " color STRING"
    clusters.EndRow();
  }
  clusters.Close();
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
auto Database::selectImageInfo(uint64_t imageId) -> ImageInfo
{
  std::unique_ptr<duckdb::QueryResult> result =
      select("SELECT file_name, validity FROM images WHERE image_id = ?", imageId);
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();

  ImageInfo results;
  results.filename = materializedResult->GetValue(0, 0).GetValue<std::string>();
  results.validity = materializedResult->GetValue(1, 0).GetValue<uint64_t>();

  return results;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Database::selectClusters() -> std::map<enums::ClusterId, joda::settings::Cluster>
{
  std::unique_ptr<duckdb::QueryResult> result = select("SELECT cluster_id, name FROM clusters");
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();

  std::map<enums::ClusterId, joda::settings::Cluster> results;
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    joda::settings::Cluster tmp;
    tmp.clusterId = static_cast<enums::ClusterId>(materializedResult->GetValue(0, n).GetValue<uint16_t>());
    tmp.name      = materializedResult->GetValue(1, n).GetValue<std::string>();
    results.try_emplace(tmp.clusterId, tmp);
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
  std::unique_ptr<duckdb::QueryResult> result = select("SELECT class_id, name FROM classes");
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();

  std::map<enums::ClassId, joda::settings::Class> results;
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    joda::settings::Class tmp;
    tmp.classId = static_cast<enums::ClassId>(materializedResult->GetValue(0, n).GetValue<uint16_t>());
    tmp.name    = materializedResult->GetValue(1, n).GetValue<std::string>();
    results.try_emplace(tmp.classId, tmp);
  }

  return results;
}

}    // namespace joda::db
