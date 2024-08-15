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
#include <string>
#include "backend/artifacts/object_list/object_list.hpp"
#include <duckdb/common/types/value.hpp>
#include <duckdb/common/types/vector.hpp>
#include <duckdb/main/appender.hpp>

namespace joda::db {

/////////////////////////////////////////////////////
void Database::openDatabase(const std::filesystem::path &pathToDb)
{
  mDbCfg.SetOption("temp_directory", pathToDb.parent_path().string());
  mDb = std::make_unique<duckdb::DuckDB>(pathToDb.string(), &mDbCfg);
  createTables();
}
void Database::closeDatabase()
{
}

void Database::createTables()
{
  // Command to create a table
  const char *create_table_sql =
      "CREATE TABLE IF NOT EXISTS objects ("
      "	image_id UBIGINT,"
      " object_id UINTEGER,"
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
      " meas_mask BOOLEAN[],"
      " meas_contour UINTEGER[]"
      ");"

      "CREATE TABLE IF NOT EXISTS object_measurements ("
      "	image_id UBIGINT,"
      " object_id UINTEGER,"
      " cluster_id USMALLINT,"
      " class_id USMALLINT,"
      " stack_c UINTEGER,"
      " stack_z UINTEGER,"
      " stack_t UINTEGER,"
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
      " object_id UINTEGER,"
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
      " meas_object_id UINTEGER"
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
      objects.Append<uint32_t>(roi.getObjectId());                // " object_id UINTEGER,"
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

      /*auto mask = duckdb::Value::LIST(duckdb::LogicalType(duckdb::LogicalTypeId::UINTEGER),
                                      {roi.getMask().datastart, roi.getMask().dataend});*/
      auto mask = duckdb::Value::LIST(duckdb::LogicalType(duckdb::LogicalTypeId::BOOLEAN), {});
      objects.Append<duckdb::Value>(mask);    // " meas_mask BOOLEAN[]"

      duckdb::vector<duckdb::Value> flattenPoints;
      flatten(roi.getContour(), flattenPoints);
      auto contour = duckdb::Value::LIST(duckdb::LogicalType(duckdb::LogicalTypeId::UINTEGER), flattenPoints);
      objects.Append<duckdb::Value>(contour);    // " meas_contour UINTEGER[]"
      objects.EndRow();

      //
      // Intensities
      //
      for(const auto &[plane, intensity] : roi.getIntensity()) {
        object_measurements.BeginRow();
        // Primary key
        object_measurements.Append<uint64_t>(imgContext.imageId);               //       "	image_id UBIGINT,"
        object_measurements.Append<uint32_t>(roi.getObjectId());                //       " object_id UINTEGER,"
        object_measurements.Append<uint16_t>((uint16_t) roi.getClusterId());    //       " cluster_id USMALLINT,"
        object_measurements.Append<uint16_t>((uint16_t) roi.getClassId());      //       " class_id USMALLINT,"
        object_measurements.Append<uint32_t>(roi.getC());                       //       " stack_c UINTEGER,"
        object_measurements.Append<uint32_t>(roi.getZ());                       //       " stack_z UINTEGER,"
        object_measurements.Append<uint32_t>(roi.getT());                       //       " stack_t UINTEGER,"
        // Data
        object_measurements.Append<uint32_t>(plane.imagePlane.cStack);    //       " meas_stack_c UINTEGER,"
        object_measurements.Append<uint32_t>(plane.imagePlane.zStack);    //       " meas_stack_z UINTEGER,"
        object_measurements.Append<uint32_t>(plane.imagePlane.tStack);    //       " meas_stack_t UINTEGER,"
        object_measurements.Append<uint64_t>(roi.getT());                 //       " meas_intensity_sum UBIGINT,"
        object_measurements.Append<float>(roi.getT());                    //       " meas_intensity_avg float,"
        object_measurements.Append<uint32_t>(roi.getT());                 //       " meas_intensity_min UINTEGER,"
        object_measurements.Append<uint32_t>(roi.getT());                 //       " meas_intensity_max UINTEGER"
        object_measurements.EndRow();
      }

      //
      // Intersections
      //
      for(const auto &intersectingRoi : roi.getIntersections()) {
        object_intersections.BeginRow();
        // Primary key
        object_intersections.Append<uint64_t>(imgContext.imageId);               //       "	image_id UBIGINT,"
        object_intersections.Append<uint32_t>(roi.getObjectId());                //       " object_id UINTEGER,"
        object_intersections.Append<uint16_t>((uint16_t) roi.getClusterId());    //       " cluster_id USMALLINT,"
        object_intersections.Append<uint16_t>((uint16_t) roi.getClassId());      //       " class_id USMALLINT,"
        object_intersections.Append<uint32_t>(roi.getC());                       //       " stack_c UINTEGER,"
        object_intersections.Append<uint32_t>(roi.getZ());                       //       " stack_z UINTEGER,"
        object_intersections.Append<uint32_t>(roi.getT());                       //       " stack_t UINTEGER,"
        // Data
        object_measurements.Append<uint32_t>(intersectingRoi.imagePlane.cStack);       //
        object_measurements.Append<uint32_t>(intersectingRoi.imagePlane.zStack);       //
        object_measurements.Append<uint32_t>(intersectingRoi.imagePlane.tStack);       //
        object_measurements.Append<uint16_t>((uint16_t) intersectingRoi.clusterId);    //
        object_measurements.Append<uint16_t>((uint16_t) intersectingRoi.classId);      //
        object_measurements.Append<uint32_t>(intersectingRoi.objectId);                //
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

void Database::insertProjectSettings(const joda::settings::AnalyzeSettings &)
{
}
void Database::insertImage(const joda::processor::ImageContext &)
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

void Database::insertImagePlane()
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

}    // namespace joda::db
