

#include "stats_for_well.hpp"
#include <string>

namespace joda::db {

///
/// \class      PositionInWell
/// \author     Joachim Danmayr
/// \brief      Position in well
///
struct ImgPositionInWell
{
  int32_t img = -1;
  int32_t x   = -1;
  int32_t y   = -1;
};

auto transformMatrix(const std::vector<std::vector<int32_t>> &wellImageOrder, int32_t &sizeX, int32_t &sizeY)
    -> std::map<int32_t, ImgPositionInWell>;

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerGroup::toTable(const QueryFilter &filter) -> joda::table::Table
{
  auto materializedResult = getData(filter)->Cast<duckdb::StreamQueryResult>().Materialize();
  table::Table results;
  results.setColHeader({{0, toString(filter.measurementChannel) + "(" + toString(filter.stats) + ")"}});

  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    try {
      uint64_t id = materializedResult->GetValue(0, n).GetValue<uint64_t>();

      results.getMutableRowHeader()[n] = materializedResult->GetValue(2, n).GetValue<std::string>();
      enums::ChannelValidity validity{materializedResult->GetValue(3, n).GetValue<uint64_t>()};

      double value = 0;
      // Valid
      if(!materializedResult->GetValue(5, n).IsNull()) {
        value = materializedResult->GetValue(5, n).GetValue<double>();
      }
      // Invalid
      if(!materializedResult->GetValue(6, n).IsNull()) {
        auto valueTmp = materializedResult->GetValue(6, n).GetValue<double>();
        if(valueTmp > 0) {
          value = valueTmp;
        }
      }
      results.setData(n, 0, table::TableCell{value, id, true, ""});

    } catch(const duckdb::InternalException &) {
    }
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
auto StatsPerGroup::toHeatmap(const QueryFilter &filter) -> joda::table::Table
{
  auto materializedResult = getData(filter)->Cast<duckdb::StreamQueryResult>().Materialize();
  table::Table results;

  int32_t sizeX = 0;
  int32_t sizeY = 0;
  auto wellPos  = transformMatrix(filter.wellImageOrder, sizeX, sizeY);
  for(uint8_t row = 0; row < sizeY; row++) {
    char toWrt[2];
    toWrt[0]                           = row + 'A';
    toWrt[1]                           = 0;
    results.getMutableRowHeader()[row] = std::string(toWrt);
    for(uint8_t col = 0; col < sizeX; col++) {
      results.getMutableColHeader()[col] = std::to_string(col + 1);
      results.setData(row, col, table::TableCell{std::numeric_limits<double>::quiet_NaN(), 0, true, ""});
    }
  }

  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    try {
      uint64_t imageId        = materializedResult->GetValue(0, n).GetValue<uint64_t>();
      uint32_t imgIdx         = materializedResult->GetValue(1, n).GetValue<uint32_t>();
      std::string imgFilename = materializedResult->GetValue(2, n).GetValue<std::string>();
      enums::ChannelValidity validity{materializedResult->GetValue(3, n).GetValue<uint64_t>()};
      auto pos = wellPos[imgIdx];

      double value = 0;
      // Valid
      if(!materializedResult->GetValue(5, n).IsNull()) {
        value = materializedResult->GetValue(5, n).GetValue<double>();
      }
      // Invalid
      if(!materializedResult->GetValue(6, n).IsNull()) {
        auto valueTmp = materializedResult->GetValue(6, n).GetValue<double>();
        if(valueTmp > 0) {
          value = valueTmp;
        }
      }

      results.setData(pos.y, pos.x, table::TableCell{value, imageId, !validity.any(), imgFilename});
    } catch(const duckdb::InternalException &ex) {
    }
  }

  results.print();
  return results;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto StatsPerGroup::getData(const QueryFilter &filter) -> std::unique_ptr<duckdb::QueryResult>
{
  auto buildStats = [&]() {
    return getStatsString(filter.stats) + "(" + getMeasurement(filter.measurementChannel) +
           ") FILTER (images_planes.validity = 0 AND images.validity = 0) as valid, " + getStatsString(filter.stats) +
           "(" + getMeasurement(filter.measurementChannel) +
           ") FILTER (images_planes.validity != 0 OR images.validity != 0) as invalid ";
  };

  auto queryMeasure = [&]() {
    std::unique_ptr<duckdb::QueryResult> result = filter.analyzer->select(
        " SELECT"
        " objects.image_id,"
        " ANY_VALUE(images_groups.image_group_idx),"
        " ANY_VALUE(images.file_name),"
        " ANY_VALUE(images.validity) as validity,"
        " images_groups.group_id as group_id," +
            buildStats() +
            " FROM objects "
            " JOIN images_groups ON objects.image_id = images_groups.image_id "
            " JOIN images ON objects.image_id = images.image_id "
            " JOIN images_planes ON objects.image_id = images_planes.image_id "
            "                    AND images_planes.stack_c = $4"
            " WHERE cluster_id = $1 AND class_id = $2 AND images_groups.group_id = $3"
            " GROUP BY objects.image_id, images_groups.group_id",
        static_cast<uint16_t>(filter.clusterId), static_cast<uint16_t>(filter.classId),
        static_cast<uint16_t>(filter.actGroupId), static_cast<uint32_t>(filter.stack_c));
    return result;
  };

  auto queryIntensityMeasure = [&]() {
    std::unique_ptr<duckdb::QueryResult> result = filter.analyzer->select(
        " SELECT"
        " objects.image_id,"
        " ANY_VALUE(images_groups.image_group_idx),"
        " ANY_VALUE(images.file_name),"
        " ANY_VALUE(images.validity),"
        " images_groups.group_id as group_id," +
            buildStats() +
            " FROM objects "
            " JOIN images_groups ON objects.image_id = images_groups.image_id "
            " JOIN images ON objects.image_id = images.image_id "
            " JOIN images_planes ON objects.image_id = images_planes.image_id "
            "                    AND images_planes.stack_c = $4"
            "JOIN object_measurements ON (objects.object_id = object_measurements.object_id AND "
            "                                  objects.image_id = object_measurements.image_id "
            "                             AND object_measurements.meas_stack_c = $4)"
            " WHERE cluster_id = $1 AND class_id = $2 AND images_groups.group_id = $3"
            " GROUP BY objects.image_id, images_groups.group_id",
        static_cast<uint16_t>(filter.clusterId), static_cast<uint16_t>(filter.classId),
        static_cast<uint16_t>(filter.actGroupId), static_cast<uint32_t>(filter.stack_c));
    return result;
  };

  auto queryIntersectingMeasure = [&]() {
    std::unique_ptr<duckdb::QueryResult> result = filter.analyzer->select(
        "   SELECT "
        "   subquery.image_id,"
        "     ANY_VALUE(subquery.image_group_idx),"
        "     ANY_VALUE(subquery.file_name),"
        "     ANY_VALUE(subquery.validity),"
        "     ANY_VALUE(subquery.group_id) as group_id,"
        "    	AVG(subquery.valid)  as valid,"
        "    	AVG(subquery.invalid)  as invalid"
        "   FROM"
        "   ("
        "    SELECT"
        "     object_intersections.image_id,"
        "     ANY_VALUE(images_groups.image_group_idx) as image_group_idx,"
        "     ANY_VALUE(images.file_name) as file_name,"
        "     ANY_VALUE(images.validity) as validity,"
        "     ANY_VALUE(images_groups.group_id) as group_id,"
        "    	COUNT(object_intersections.meas_object_id) FILTER (images.validity = 0) as valid,"
        "    	COUNT(object_intersections.meas_object_id) FILTER (images.validity != 0) as invalid"
        "    FROM"
        "    	object_intersections"
        "    JOIN images ON object_intersections.image_id = images.image_id"
        "    JOIN images_groups ON object_intersections.image_id = images_groups.image_id "
        "    JOIN objects ON"
        "    	objects.object_id = object_intersections.meas_object_id"
        "    	AND objects.image_id = object_intersections.image_id   "
        "    	AND objects.cluster_id = $1                            "
        "    	AND objects.class_id = $2                            "
        "     AND images_groups.group_id = $3"
        "    WHERE objects.cluster_id = $1 AND objects.class_id = $2 AND images_groups.group_id = $3"
        "    GROUP BY                                                 "
        "    	(objects.object_id, object_intersections.image_id)  "
        "    	) AS subquery"
        "    	GROUP BY image_id",
        static_cast<uint16_t>(filter.clusterId), static_cast<uint16_t>(filter.classId),
        static_cast<uint16_t>(filter.actGroupId));
    return result;
  };

  auto query = [&]() {
    switch(getType(filter.measurementChannel)) {
      case OBJECT:
        return queryMeasure();
      case INTENSITY:
        return queryIntensityMeasure();
      case COUNT:
        return queryIntersectingMeasure();
    }
  };

  auto result = query();

  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
  return result;
}

///
/// \brief      Transforms a 2D Matrix where the elements in the matrix represents an images index
///             and the coordinates of the matrix the position on the well to a map
///             whereby the key is the images index and the values are the coordinates
///              | 0  1  2
///             -|---------
///             0| 1  2  3
///             1| 4  5  6
///             2| 7  8  9
///
///            [1] => {0,0}
///            [2] => {1,0}
///            ...
///            [9] => {2,2}
///
///
/// \author     Joachim Danmayr
///
auto transformMatrix(const std::vector<std::vector<int32_t>> &wellImageOrder, int32_t &sizeX, int32_t &sizeY)
    -> std::map<int32_t, ImgPositionInWell>
{
  sizeY = wellImageOrder.size();
  sizeX = 0;

  std::map<int32_t, ImgPositionInWell> ret;
  for(int y = 0; y < wellImageOrder.size(); y++) {
    for(int x = 0; x < wellImageOrder[y].size(); x++) {
      auto imgNr = wellImageOrder[y][x];
      ret[imgNr] = ImgPositionInWell{.img = imgNr, .x = x, .y = y};
      if(x > sizeX) {
        sizeX = x;
      }
    }
  }
  sizeX++;    // Because we start with zro to count
  return ret;
}

}    // namespace joda::db
