///
/// \file      results_settings.hpp
/// \author    Joachim Danmayr
/// \date      2025-03-04
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <stdexcept>
#include <string>
#include <utility>
#include "backend/enums/enum_measurements.hpp"
#include "backend/settings/project_settings/project_plate_setup.hpp"
#include "backend/settings/setting.hpp"
#include "heatmap_settings.hpp"

namespace joda::settings {

///
/// \class      AnalyzeSettingsMeta
/// \author     Joachim Danmayr
/// \brief      Database interface to store some project meta settings needed for settings generation from template
///
struct AnalyzeSettingsMeta
{
  std::set<joda::enums::ClassId> outputClasses;
  std::map<enums::ClassId, std::set<enums::ClassId>> intersectingClasses;
  std::map<enums::ClassId, std::set<int32_t>> measuredChannels;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AnalyzeSettingsMeta, outputClasses, intersectingClasses, measuredChannels);
};

///
/// \class      ResultsSettings
/// \author     Joachim Danmayr
/// \brief      Database interface to store table settings
///
class ResultsSettings
{
public:
  enum MeasureType
  {
    ID,
    OBJECT,
    INTENSITY,
    DISTANCE,
    DISTANCE_ID,
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
      case enums::Measurement::DISTANCE_CENTER_TO_CENTER:
      case enums::Measurement::DISTANCE_CENTER_TO_SURFACE_MIN:
      case enums::Measurement::DISTANCE_CENTER_TO_SURFACE_MAX:
      case enums::Measurement::DISTANCE_SURFACE_TO_SURFACE_MIN:
      case enums::Measurement::DISTANCE_SURFACE_TO_SURFACE_MAX:
        return MeasureType::DISTANCE;
      case enums::Measurement::DISTANCE_FROM_OBJECT_ID:
      case enums::Measurement::DISTANCE_TO_OBJECT_ID:
        return MeasureType::DISTANCE_ID;
      case enums::Measurement::CENTEROID_X:
      case enums::Measurement::CENTEROID_Y:
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
      case enums::Measurement::TRACKING_ID:
        return MeasureType::ID;
    }
    return MeasureType::OBJECT;
  }

  struct ObjectFilter
  {
    enum class TStackHandling
    {
      SLICE,
      INDIVIDUAL
    };

    uint8_t plateId  = 0;
    uint16_t groupId = 0;
    std::set<uint64_t> imageId;
    int32_t tStack                = 0;
    TStackHandling tStackHandling = TStackHandling::SLICE;

    // NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ObjectFilter);
  };

  struct ColumnName
  {
    std::string crossChannelName;
    std::string className;
    std::string intersectingName;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ColumnName, crossChannelName, className, intersectingName);
  };

  struct ColumnIdx
  {
    int32_t colIdx = 0;
    bool operator<(const ColumnIdx &input) const
    {
      auto toInt = [](const ColumnIdx &in) -> uint64_t {
        uint64_t erg = (static_cast<uint64_t>(in.colIdx));
        return erg;
      };
      return toInt(*this) < toInt(input);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ColumnIdx, colIdx);
  };

  struct ColumnKey
  {
    joda::enums::ClassId classId             = joda::enums::ClassId::UNDEFINED;
    enums::Measurement measureChannel        = enums::Measurement::NONE;
    enums::Stats stats                       = enums::Stats::AVG;
    int32_t crossChannelStacksC              = -1;
    joda::enums::ClassId intersectingChannel = joda::enums::ClassId::NONE;
    int32_t zStack                           = 0;

    ColumnName names;

    bool operator<(const ColumnKey &input) const
    {
      auto toInt = [](const ColumnKey &in) {
        auto classClasss         = static_cast<uint16_t>(in.classId);
        auto intersectingChannel = static_cast<uint16_t>(in.intersectingChannel);
        auto measure             = static_cast<uint8_t>(in.measureChannel);
        auto stat                = static_cast<uint8_t>(in.stats);

        stdi::uint128_t erg = (static_cast<stdi::uint128_t>(classClasss) << 112) | (static_cast<stdi::uint128_t>(0) << 80) |
                              (static_cast<stdi::uint128_t>(in.zStack) << 48) | (static_cast<stdi::uint128_t>(measure & 0xFF) << 40) |
                              (static_cast<stdi::uint128_t>(stat) << 32) | (static_cast<stdi::uint128_t>(in.crossChannelStacksC & 0xFFFF) << 16) |
                              (static_cast<stdi::uint128_t>(intersectingChannel) << 0);
        return erg;
      };

      return toInt(*this) < toInt(input);
    }

    bool operator==(const ColumnKey &input) const
    {
      return classId == input.classId && static_cast<int32_t>(measureChannel) == static_cast<int32_t>(input.measureChannel) &&
             static_cast<int32_t>(stats) == static_cast<int32_t>(input.stats) && crossChannelStacksC == input.crossChannelStacksC &&
             intersectingChannel == input.intersectingChannel && zStack == input.zStack;
    }

    std::string createHeader() const;

    enum class HeaderStyle
    {
      FULL,
      WITHOUT_OWN_NAME,
    };
    std::string createHtmlHeader(HeaderStyle style) const;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ColumnKey, classId, measureChannel, stats, crossChannelStacksC, intersectingChannel, zStack, names);
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

  void setFilter(int32_t plateId, int32_t groupId, int32_t tStack, const std::set<uint64_t> &imageId)
  {
    filter.plateId = plateId;
    filter.groupId = groupId;
    filter.imageId = imageId;
    filter.tStack  = tStack;
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
        if(startToReduce) {
          colNew.colIdx--;
        }
        newColumns.emplace(colNew, _);
      }
      columns.clear();
      columns = newColumns;
    }
  }

  void eraseColumn(const ColumnKey &colKey)
  {
    for(const auto &[colIdx, key] : columns) {
      if(colKey == key) {
        eraseColumn(colIdx);
        break;
      }
    }
  }

  [[nodiscard]] auto getColumnIdx(const ColumnKey &colKey) const -> ColumnIdx
  {
    for(const auto &[colIdx, key] : columns) {
      if(colKey == key) {
        return colIdx;
      }
    }
    throw std::out_of_range("Not found!");
  }

  [[nodiscard]] auto containsColumnIdx(const ColumnKey &colKey) const -> bool
  {
    for(const auto &[colIdx, key] : columns) {
      if(colKey == key) {
        return true;
      }
    }
    return false;
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

  void sortColumns()
  {
    std::vector<std::pair<ColumnIdx, ColumnKey>> data{columns.begin(), columns.end()};
    // Sort based on MyStruct.value (i.e., second element in the tuple)
    std::sort(data.begin(), data.end(),
              [](const std::pair<ColumnIdx, ColumnKey> &a, const std::pair<ColumnIdx, ColumnKey> &b) { return std::get<1>(a) < std::get<1>(b); });

    columns.clear();
    int32_t colIdx = 0;
    for(const auto &[colIdxAct, keyIdx] : data) {
      columns.emplace(ColumnIdx{colIdx}, keyIdx);
      colIdx++;
    }
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
