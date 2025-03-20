///
/// \file      results_settings.hpp
/// \author    Joachim Danmayr
/// \date      2025-03-04
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/database/exporter/heatmap/export_heatmap_settings.hpp"
#include "backend/settings/project_settings/project_plate_setup.hpp"
#include "backend/settings/setting.hpp"

namespace joda::settings {

class ResultsSettings
{
public:
  enum MeasureType
  {
    ID,
    OBJECT,
    INTENSITY,
    INTERSECTION
  };

  static MeasureType getType(enums::Measurement measure)
  {
    switch(measure) {
      case enums::Measurement::INTENSITY_SUM:
      case enums::Measurement::INTENSITY_AVG:
      case enums::Measurement::INTENSITY_MIN:
      case enums::Measurement::INTENSITY_MAX:
        return MeasureType::INTENSITY;
      case enums::Measurement::CENTER_OF_MASS_X:
      case enums::Measurement::CENTER_OF_MASS_Y:
      case enums::Measurement::CONFIDENCE:
      case enums::Measurement::AREA_SIZE:
      case enums::Measurement::PERIMETER:
      case enums::Measurement::CIRCULARITY:
      case enums::Measurement::COUNT:
      case enums::Measurement::BOUNDING_BOX_WIDTH:
      case enums::Measurement::BOUNDING_BOX_HEIGHT:
        return MeasureType::OBJECT;
      case enums::Measurement::INTERSECTING:
        return MeasureType::INTERSECTION;
      case enums::Measurement::OBJECT_ID:
      case enums::Measurement::ORIGIN_OBJECT_ID:
      case enums::Measurement::PARENT_OBJECT_ID:
        return MeasureType::ID;
    }
    return MeasureType::OBJECT;
  }

  struct ObjectFilter
  {
    uint8_t plateId  = 0;
    uint16_t groupId = 0;
    uint64_t imageId = 0;

    // NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ObjectFilter);
  };

  struct ColumnName
  {
    std::string crossChannelName;
    std::string className;
    std::string intersectingName;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ColumnName, crossChannelName, className, intersectingName);
  };

  struct ColumnKey
  {
    joda::enums::ClassId classId;
    enums::Measurement measureChannel        = enums::Measurement::NONE;
    enums::Stats stats                       = enums::Stats::AVG;
    int32_t crossChannelStacksC              = -1;
    joda::enums::ClassId intersectingChannel = joda::enums::ClassId::NONE;
    int32_t zStack                           = 0;
    int32_t tStack                           = 0;

    ColumnName names;

    bool operator<(const ColumnKey &input) const
    {
      auto toInt = [](const ColumnKey &in) {
        uint32_t classClasss         = static_cast<uint16_t>(in.classId);
        uint32_t intersectingChannel = static_cast<uint16_t>(in.intersectingChannel);
        auto measure                 = static_cast<uint8_t>(in.measureChannel);
        auto stat                    = static_cast<uint8_t>(in.stats);

        stdi::uint128_t erg = (static_cast<stdi::uint128_t>(intersectingChannel) << 112) | (static_cast<stdi::uint128_t>(classClasss) << 96) |
                              (static_cast<stdi::uint128_t>(in.crossChannelStacksC & 0xFFFF) << 80) |
                              (static_cast<stdi::uint128_t>(in.zStack) << 18) | (static_cast<stdi::uint128_t>(in.tStack) << 16) | (measure << 8) |
                              (stat);
        return erg;
      };

      return toInt(*this) < toInt(input);
    }

    bool operator==(const ColumnKey &input) const
    {
      return classId == input.classId && static_cast<int32_t>(measureChannel) == static_cast<int32_t>(input.measureChannel) &&
             static_cast<int32_t>(stats) == static_cast<int32_t>(input.stats) && crossChannelStacksC == input.crossChannelStacksC &&
             intersectingChannel == input.intersectingChannel && zStack == input.zStack && tStack == input.tStack;
    }

    std::string createHeader() const
    {
      std::map<uint32_t, std::string> columnHeaders;
      std::string stacks = "{Z" + std::to_string(zStack) + "/T" + std::to_string(tStack) + "}";

      if(getType(measureChannel) == MeasureType::INTENSITY) {
        return names.className + "-" + toString(measureChannel) + "[" + enums::toString(stats) + "] " + "(C" + std::to_string(crossChannelStacksC) +
               ")" + stacks;
      }
      if(getType(measureChannel) == MeasureType::INTERSECTION) {
        return "Intersection " + names.intersectingName + " in " + names.className + "[" + enums::toString(stats) + "]" + stacks;
      }
      if(getType(measureChannel) == MeasureType::ID) {
        return names.className + "-" + toString(measureChannel) + "\n" + stacks;
      }
      return names.className + "-" + toString(measureChannel) + "[" + enums::toString(stats) + "]" + stacks;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ColumnKey, classId, measureChannel, stats, crossChannelStacksC, intersectingChannel, zStack, tStack,
                                                names);
  };

  struct ColumnIdx
  {
    int32_t tabIdx = 0;
    int32_t colIdx = 0;
    bool operator<(const ColumnIdx &input) const
    {
      auto toInt = [](const ColumnIdx &in) -> uint64_t {
        uint64_t erg = (static_cast<uint64_t>(in.tabIdx) << 32) | (static_cast<uint64_t>(in.colIdx));
        return erg;
      };
      return toInt(*this) < toInt(input);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ColumnIdx, tabIdx, colIdx);
  };

  explicit ResultsSettings() = default;

  void setFilter(const ObjectFilter &filter, const joda::settings::PlateSetup &plateSetup, const DensityMapSettings &densityMap)
  {
    this->filter             = filter;
    this->plateSetup         = plateSetup;
    this->densityMapSettings = densityMap;
  }

  void setFilter(const joda::settings::PlateSetup &plateSetup)
  {
    this->plateSetup = plateSetup;
  }

  void setFilter(int32_t plateId, int32_t groupId, uint64_t imageId)
  {
    filter.plateId = plateId;
    filter.groupId = groupId;
    filter.imageId = imageId;
  }

  bool addColumn(const ColumnIdx &colIdx, const ColumnKey &key, const ColumnName &names)
  {
    for(const auto &[_, colKey] : columns) {
      if(colKey == key) {
        return false;
      }
    }

    if(!columns.contains(colIdx)) {
      columns.emplace(colIdx, key);
    } else {
      columns[colIdx] = key;
    }

    columns[colIdx].names = names;
    return true;
  }

  void eraseColumn(const ColumnIdx colIdx)
  {
    if(columns.contains(colIdx)) {
      columns.erase(colIdx);
      std::map<ColumnIdx, ColumnKey> newColumns;
      bool startToReduce = false;

      for(const auto &[col, _] : columns) {
        auto colNew = col;
        if(colNew.colIdx > colIdx.colIdx) {
          startToReduce = true;
        }
        if(startToReduce && colIdx.tabIdx == col.tabIdx) {
          colNew.colIdx--;
        }
        newColumns.emplace(colNew, _);
      }
      columns.clear();
      columns = newColumns;
    }
  }

  [[nodiscard]] auto getColumn(const ColumnIdx &colIdx) const -> ColumnKey
  {
    return columns.at(colIdx);
  }

  [[nodiscard]] bool containsColumn(const ColumnIdx &colIdx) const
  {
    return columns.contains(colIdx);
  }

  [[nodiscard]] auto getFilter() const -> const ObjectFilter &
  {
    return filter;
  }

  [[nodiscard]] auto getPlateSetup() const -> const joda::settings::PlateSetup &
  {
    return plateSetup;
  }

  [[nodiscard]] auto getColumns() const -> const std::map<ColumnIdx, ColumnKey> &
  {
    return columns;
  }

  [[nodiscard]] auto mutableColumns() -> std::map<ColumnIdx, ColumnKey> &
  {
    return columns;
  }

  void check() const
  {
  }
  // We don't want to do a error check for the history
  void getErrorLogRecursive(SettingParserLog_t &settingsParserLog) const
  {
  }

  void setDensityMapSettings(const DensityMapSettings &settings)
  {
    densityMapSettings = settings;
  }

  [[nodiscard]] auto getDensityMapSettings() const -> const DensityMapSettings &
  {
    return densityMapSettings;
  }

  [[nodiscard]] auto mutableDensityMapSettings() -> DensityMapSettings &
  {
    return densityMapSettings;
  }

private:
  // TEMPORARY ///////////////////////////////////////////////////
  ObjectFilter filter;

  // PERSISTENT ///////////////////////////////////////////////////
  PlateSetup plateSetup;
  DensityMapSettings densityMapSettings;
  std::map<ColumnIdx, ColumnKey> columns;
  std::string configSchema = "https://imagec.org/schemas/v1/results-settings.json";

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ResultsSettings, columns, plateSetup, densityMapSettings, configSchema);
};

}    // namespace joda::settings
