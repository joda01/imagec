

#pragma once

#include <cstdint>
#include "backend/results/analyzer/analyzer.hpp"
#include "backend/results/db_column_ids.hpp"
#include "helper.hpp"

namespace joda::results::analyze::plugins {

class HeatmapForImage
{
public:
  ///
  /// \brief      Get data for well
  /// \author     Joachim Danmayr
  /// \param[in]  wellImageOrder  First dimension of the vector are the rows, second the columns
  ///
  static auto getData(Analyzer &analyzer, uint64_t imageId, ChannelIndex channelId, const MeasureChannelId &measurement,
                      Stats stats, uint32_t areaSize) -> Table
  {
    Table results;
    std::string controlImgPath;

    {
      std::unique_ptr<duckdb::QueryResult> images = analyzer.getDatabase().select(
          "SELECT"
          "  images.image_id as image_id,"
          "  images.width as width,"
          "  images.height as height,"
          "  images.file_name as file_name,"
          "  channels_images.control_image_path as control_image_path "
          "FROM images "
          "INNER JOIN channels_images ON images.image_id=channels_images.image_id "
          "WHERE"
          " channels_images.image_id=$1 AND channels_images.channel_id=$2 ",
          imageId, static_cast<uint8_t>(channelId));

      if(images->HasError()) {
        throw std::invalid_argument(images->GetError());
      }

      auto imageMaterialized = images->Cast<duckdb::StreamQueryResult>().Materialize();
      uint64_t imgWidth      = imageMaterialized->GetValue(1, 0).GetValue<uint64_t>();
      uint64_t imgWeight     = imageMaterialized->GetValue(2, 0).GetValue<uint64_t>();
      controlImgPath         = imageMaterialized->GetValue(4, 0).GetValue<std::string>();

      std::string linkToImage = controlImgPath;
      helper::stringReplace(linkToImage, "${tile_id}", std::to_string(0));

      uint64_t width  = imgWidth / areaSize;
      uint64_t height = imgWeight / areaSize;

      for(uint64_t row = 0; row < height; row++) {
        results.getMutableRowHeader()[row] = std::to_string(row + 1);
        for(uint64_t col = 0; col < width; col++) {
          results.getMutableColHeader()[col] = std::to_string(col + 1);
          results.setData(row, col, TableCell{std::numeric_limits<double>::quiet_NaN(), 0, false, linkToImage});
        }
      }
    }

    {
      std::unique_ptr<duckdb::QueryResult> result = analyzer.getDatabase().select(
          "SELECT "
          "floor(element_at(values, $5)[1] / $4) * $4 AS rectangle_x,"
          "floor(element_at(values, $6)[1] / $4) * $4 AS rectangle_y,"
          "any_value(tile_id) tile_id, " +
              getStatsString(stats) +
              "FROM objects "
              "WHERE"
              " image_id=$2 AND validity=0 AND channel_id=$3 "
              "GROUP BY floor(element_at(values, $5)[1] / $4), floor(element_at(values, $6)[1] / $4)",
          measurement.getKey(), imageId, static_cast<uint8_t>(channelId), duckdb::Value::DOUBLE(areaSize),
          MeasureChannelId(MeasureChannel::CENTER_OF_MASS_X, ChannelIndex::ME).getKey(),
          MeasureChannelId(MeasureChannel::CENTER_OF_MASS_Y, ChannelIndex::ME).getKey());

      if(result->HasError()) {
        throw std::invalid_argument(result->GetError());
      }

      auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
      for(size_t n = 0; n < materializedResult->RowCount(); n++) {
        try {
          uint32_t rectangleX = materializedResult->GetValue(0, n).GetValue<double>();
          uint32_t rectangleY = materializedResult->GetValue(1, n).GetValue<double>();
          uint16_t tileId     = materializedResult->GetValue(2, n).GetValue<uint16_t>();
          double value        = materializedResult->GetValue(3, n).GetValue<double>();

          uint32_t x = rectangleX / areaSize;
          uint32_t y = rectangleY / areaSize;

          std::string linkToImage = controlImgPath;
          helper::stringReplace(linkToImage, "${tile_id}", std::to_string(tileId));
          results.setData(y, x, TableCell{value, tileId, true, linkToImage});
        } catch(const duckdb::InternalException &) {
        }
      }
    }
    return results;
  }
};
}    // namespace joda::results::analyze::plugins
