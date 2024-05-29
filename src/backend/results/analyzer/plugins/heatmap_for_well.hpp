

#pragma once

#include <cstdint>
#include "backend/results/analyzer/analyzer.hpp"
#include "helper.hpp"

namespace joda::results::analyze::plugins {

class HeatmapForWell
{
private:
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

public:
  ///
  /// \brief      Get data for well
  /// \author     Joachim Danmayr
  /// \param[in]  wellImageOrder  First dimension of the vector are the rows, second the columns
  ///
  static auto getData(Analyzer &analyzer, uint8_t plateId, results::WellId wellId, ChannelIndex channelId,
                      const MeasureChannelId &measurement, Stats stats,
                      const std::vector<std::vector<int32_t>> &wellImageOrder = {{1, 2, 3, 4},
                                                                                 {5, 6, 7, 8},
                                                                                 {9, 10, 11, 12},
                                                                                 {13, 14, 15, 16}}) -> Table
  {
    std::unique_ptr<duckdb::QueryResult> result = analyzer.getDatabase().select(
        "SELECT"
        "  object.image_id as image_id,"
        "  image.image_idx as image_idx,"
        "  any_value(channel_image.control_image_path) as control_image_path, "
        "  any_value(object.tile_id) as tile_id, "
        "  ANY_VALUE(channel_image.validity) as validity,"
        "  image.file_name as file_name," +
            getStatsString(stats) +
            "FROM object "
            "INNER JOIN image_well ON object.image_id=image_well.image_id "
            "INNER JOIN image ON object.image_id=image.image_id "
            "INNER JOIN channel_image ON (object.image_id=channel_image.image_id AND "
            "object.channel_id=channel_image.channel_id)"
            "WHERE"
            " image_well.well_id=$2 AND bit_count(object.validity)=0 AND object.channel_id=$3 "
            "GROUP BY"
            "  (object.image_id, image.file_name, image.image_idx) "
            "ORDER BY image.file_name",
        measurement.getKey(), wellId.well.wellId, static_cast<uint8_t>(channelId));

    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }

    int32_t sizeX = 0;
    int32_t sizeY = 0;
    auto wellPos  = transformMatrix(wellImageOrder, sizeX, sizeY);

    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    Table results;

    for(uint8_t row = 0; row < sizeY; row++) {
      char toWrt[2];
      toWrt[0]                           = row + 'A';
      toWrt[1]                           = 0;
      results.getMutableRowHeader()[row] = std::string(toWrt);
      for(uint8_t col = 0; col < sizeX; col++) {
        results.getMutableColHeader()[col] = std::to_string(col + 1);
        results.setData(row, col, TableCell{std::numeric_limits<double>::quiet_NaN(), 0, true, ""});
      }
    }

    for(size_t n = 0; n < materializedResult->RowCount(); n++) {
      try {
        uint64_t imageId             = materializedResult->GetValue(0, n).GetValue<uint64_t>();
        uint32_t imgIdx              = materializedResult->GetValue(1, n).GetValue<uint32_t>();
        std::string controlImagePath = materializedResult->GetValue(2, n).GetValue<std::string>();
        uint16_t tileId              = materializedResult->GetValue(3, n).GetValue<uint16_t>();
        ChannelValidity validity{materializedResult->GetValue(4, n).GetValue<std::string>()};

        auto pos     = wellPos[imgIdx];
        double value = materializedResult->GetValue(6, n).GetValue<double>();

        helper::stringReplace(controlImagePath, "${tile_id}", std::to_string(tileId));
        results.setData(pos.x, pos.y, TableCell{value, imageId, !validity.any(), controlImagePath});
      } catch(const duckdb::InternalException &) {
      }
    }

    results.print();
    return results;
  }

  ///
  /// \brief      Transforms a 2D Matrix where the elements in the matrix represents an image index
  ///             and the coordinates of the matrix the position on the well to a map
  ///             whereby the key is the image index and the values are the coordinates
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

  static auto transformMatrix(const std::vector<std::vector<int32_t>> &wellImageOrder, int32_t &sizeX, int32_t &sizeY)
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
};
}    // namespace joda::results::analyze::plugins
