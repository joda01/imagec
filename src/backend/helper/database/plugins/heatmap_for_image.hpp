

#pragma once

#include <cstdint>
#include "../database.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/table/table.hpp"
#include "helper.hpp"

namespace joda::db {

class HeatmapForImage
{
public:
  ///
  /// \brief      Get data for well
  /// \author     Joachim Danmayr
  /// \param[in]  wellImageOrder  First dimension of the vector are the rows, second the columns
  ///
  static auto getData(Database &analyzer, uint8_t plateId, uint8_t plateRows, uint8_t plateCols,
                      enums::ClusterId clusterId, enums::ClassId classId, enums::Measurement measurement,
                      int32_t imageChannelId, enums::Stats stats, uint64_t imageId, uint32_t areaSize) -> table::Table
  {
    table::Table results;
    std::string controlImgPath;

    {
      std::unique_ptr<duckdb::QueryResult> images = analyzer.select(
          "SELECT"
          "  images.image_id as image_id,"
          "  images.width as width,"
          "  images.height as height,"
          "  images.file_name as file_name "
          "FROM images "
          "WHERE"
          " image_id=$1",
          static_cast<uint64_t>(imageId));
      if(images->HasError()) {
        throw std::invalid_argument("S:" + images->GetError());
      }

      auto imageMaterialized = images->Cast<duckdb::StreamQueryResult>().Materialize();
      uint64_t imgWidth      = imageMaterialized->GetValue(1, 0).GetValue<uint64_t>();
      uint64_t imgWeight     = imageMaterialized->GetValue(2, 0).GetValue<uint64_t>();
      controlImgPath         = imageMaterialized->GetValue(3, 0).GetValue<std::string>();

      std::string linkToImage = controlImgPath;
      helper::stringReplace(linkToImage, "${tile_id}", std::to_string(0));

      uint64_t width  = imgWidth / areaSize;
      uint64_t height = imgWeight / areaSize;

      for(uint64_t row = 0; row < height; row++) {
        results.getMutableRowHeader()[row] = std::to_string(row + 1);
        for(uint64_t col = 0; col < width; col++) {
          results.getMutableColHeader()[col] = std::to_string(col + 1);
          results.setData(row, col, table::TableCell{std::numeric_limits<double>::quiet_NaN(), 0, false, linkToImage});
        }
      }
    }
    auto buildStats = [&]() { return getStatsString(stats) + "(" + getMeasurement(measurement) + ") as val"; };

    {
      std::unique_ptr<duckdb::QueryResult> result = analyzer.select(
          "SELECT "
          "floor(meas_center_x / $3) * $3 AS rectangle_x,"
          "floor(meas_center_y / $3) * $3 AS rectangle_y," +
              buildStats() +
              " FROM objects "
              " WHERE"
              "  image_id=$1 AND cluster_id=$2 AND class_id=$4 "
              "GROUP BY floor(meas_center_x / $3), floor(meas_center_y / $3)",
          imageId, static_cast<uint16_t>(clusterId), duckdb::Value::DOUBLE(areaSize), static_cast<uint16_t>(classId));

      if(result->HasError()) {
        throw std::invalid_argument(result->GetError());
      }

      auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
      for(size_t n = 0; n < materializedResult->RowCount(); n++) {
        try {
          uint32_t rectangleX = materializedResult->GetValue(0, n).GetValue<double>();
          uint32_t rectangleY = materializedResult->GetValue(1, n).GetValue<double>();
          double value        = materializedResult->GetValue(2, n).GetValue<double>();

          uint32_t x = rectangleX / areaSize;
          uint32_t y = rectangleY / areaSize;

          std::string linkToImage = controlImgPath;
          results.setData(y, x, table::TableCell{value, 0, true, linkToImage});
        } catch(const duckdb::InternalException &) {
        }
      }
    }
    return results;
  }
};
}    // namespace joda::db
