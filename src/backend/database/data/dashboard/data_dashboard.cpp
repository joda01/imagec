///
/// \file      data_dashboard.cpp
/// \author    Joachim Danmayr
/// \date      2025-07-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "data_dashboard.hpp"
#include <memory>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/base32.hpp"
#include "backend/helper/table/table.hpp"

namespace joda::db::data {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
Dashboard::Dashboard()
{
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto Dashboard::convert(const std::shared_ptr<joda::table::Table> tableIn, const std::set<std::set<enums::ClassId>> &classesWithSameTrackingId,
                        bool isImageView) -> std::map<TabWindowKey, std::shared_ptr<joda::table::Table>>
{
  struct Entry
  {
    std::string colName;
    const table::TableColumn *intersectingCol = nullptr;
    std::vector<const table::TableColumn *> cols;
  };

  std::map<std::set<enums::ClassId>, uint32_t> classesWithSameTrackingIdMappingTable;

  std::map<uint32_t, Entry> dashboards;
  std::map<uint32_t, Entry> intersecting;
  std::map<uint32_t, Entry> distance;
  std::map<uint32_t, Entry> colocalizing;

  auto isDistance = [](enums::Measurement measure) {
    return measure == enums::Measurement::DISTANCE_CENTER_TO_CENTER || measure == enums::Measurement::DISTANCE_CENTER_TO_SURFACE_MAX ||
           measure == enums::Measurement::DISTANCE_CENTER_TO_SURFACE_MIN || measure == enums::Measurement::DISTANCE_FROM_OBJECT_ID ||
           measure == enums::Measurement::DISTANCE_TO_OBJECT_ID || measure == enums::Measurement::DISTANCE_SURFACE_TO_SURFACE_MAX ||
           measure == enums::Measurement::DISTANCE_SURFACE_TO_SURFACE_MIN;
  };

  // ========================================
  // First find the intersecting classes
  // ========================================
  uint32_t mapIdIdx = 0;
  for(const auto &[_, col] : tableIn->columns()) {
    if(col.colSettings.measureChannel == enums::Measurement::INTERSECTING) {
      auto &work           = intersecting[static_cast<uint32_t>(col.colSettings.intersectingChannel)];
      work.intersectingCol = &col;
      work.colName         = col.colSettings.names.intersectingName;
    }

    // Group the colocalizing classes (those with at least one common tracking id) to one table
    for(const auto &colcGroup : classesWithSameTrackingId) {
      if(colcGroup.contains(col.colSettings.classId)) {
        uint32_t mapId = 0;
        if(classesWithSameTrackingIdMappingTable.contains(colcGroup)) {
          mapId = classesWithSameTrackingIdMappingTable.at(colcGroup);
        } else {
          classesWithSameTrackingIdMappingTable.emplace(colcGroup, mapIdIdx);
          mapIdIdx++;
        }
        auto &work = colocalizing[mapIdIdx];
        work.cols.emplace_back(&col);
        work.colName = col.colSettings.names.className;
      }
    }
  }

  // ========================================
  // Now create te other dashboard columns
  // ========================================
  for(const auto &[_, col] : tableIn->columns()) {
    // This is a distance measurement. We create a own dashboard for each distance measure if we are in image view
    if(isDistance(col.colSettings.measureChannel) && isImageView) {
      uint32_t key = (static_cast<uint16_t>(col.colSettings.classId) << 16) | static_cast<uint16_t>(col.colSettings.intersectingChannel);
      auto &ed     = distance[key];
      ed.colName   = "Distance " + col.colSettings.names.className + " to " + col.colSettings.names.intersectingName;
      ed.cols.emplace_back(&col);
    } else if(intersecting.contains(static_cast<uint32_t>(col.colSettings.classId))) {
      auto &ed   = intersecting[static_cast<uint32_t>(col.colSettings.classId)];
      ed.colName = col.colSettings.names.className;
      ed.cols.emplace_back(&col);
    } else {
      auto &ed   = dashboards[static_cast<uint32_t>(col.colSettings.classId)];
      ed.colName = col.colSettings.names.className;
      ed.cols.emplace_back(&col);
    }
  }

  // ========================================
  // Lamda function to create the dashboard
  // ========================================
  std::map<TabWindowKey, std::shared_ptr<joda::table::Table>> tabs;
  auto createDashboards = [&tabs, &isImageView](const std::map<uint32_t, Entry> &entries, DashboardType dashboardType) {
    for(const auto &[key, dashData] : entries) {
      auto midiKey = TabWindowKey{dashboardType, key};
      if(!tabs.contains(midiKey)) {
        tabs[midiKey] = std::make_shared<joda::table::Table>();
      }
      auto element01 = tabs[midiKey];
      setData(element01, dashData.colName, dashData.cols, isImageView, dashboardType == DashboardType::COLOC, dashData.intersectingCol);
    }
  };

  createDashboards(dashboards, DashboardType::NORMAL);
  createDashboards(intersecting, DashboardType::INTERSECTION);
  createDashboards(distance, DashboardType::DISTANCE);
  createDashboards(colocalizing, DashboardType::COLOC);

  return tabs;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Dashboard::setData(const std::shared_ptr<joda::table::Table> &tableToSet, const std::string &description,
                        const std::vector<const table::TableColumn *> &cols, bool isImageView, bool isColoc,
                        const table::TableColumn *intersectingColl)
{
  tableToSet->setTitle(description);
  tableToSet->clear();

  //
  // We can assume that the data are ordered by image_id, parent_object_id, objects_id, t_stack.
  // We want to align first by parent_object_id and afterwards by object_id.
  // Imagine that the parent_object_id is not unique!
  //
  struct RowInfo
  {
    int32_t startingRow = 0;
    joda::table::TableCell::Formating::Color bgColor;
  };
  std::map<uint64_t, RowInfo> startOfNewParent;     // Key is the parent_id and value the row where this parent started
  std::map<uint64_t, RowInfo> trackingIdMapping;    // Key is the tracking_id and value the row where this tracking_id was first placed
  int32_t highestRow = 0;

  // We add a column with the object ID as first column
  int32_t COL_IDX_OBJECT_ID          = 0;
  const int32_t COL_IDX_INTERSECTING = 1;

  const auto LIGHT_BLUE      = joda::table::TableCell::Formating::Color::BASE_1;
  const auto DARK_BLUE       = joda::table::TableCell::Formating::Color::ALTERNATE_1;
  const auto BASE_COLOR      = joda::table::TableCell::Formating::Color::BASE_0;
  const auto ALTERNATE_COLOR = joda::table::TableCell::Formating::Color::ALTERNATE_0;

  // Data

  int32_t alternate                                = 0;
  joda::table::TableCell::Formating::Color bgColor = joda::table::TableCell::Formating::Color::BASE_0;

  settings::ResultsSettings::ColumnKey actColumnKey;
  bool addObjectId = false;
  auto colTableTmp = 0;
  for(const auto &colData : cols) {
    if(actColumnKey.classId != colData->colSettings.classId && isImageView) {
      // Add the object ID again
      actColumnKey      = colData->colSettings;
      COL_IDX_OBJECT_ID = colTableTmp;
      addObjectId       = true;
      if(intersectingColl != nullptr && colTableTmp == 0) {
        colTableTmp += 2;    // We put the parent object id in the second column
      } else {
        colTableTmp++;
      }
    } else {
      actColumnKey = colData->colSettings;
    }

    int row = 0;
    for(const auto &[_, rowData] : colData->rows) {
      if(rowData->getObjectId() == 0) {
        continue;
      }

      // =====================================
      // Row to place in case of coloc
      // =====================================
      int32_t rowToPlace = row;
      if(isColoc && isImageView) {
        if(rowData->getTrackingId() == 0) {
          // This should not happen, but if we continue
          continue;
        }

        if(trackingIdMapping.contains(rowData->getTrackingId())) {
          rowToPlace = trackingIdMapping.at(rowData->getTrackingId()).startingRow;
          bgColor    = trackingIdMapping.at(rowData->getTrackingId()).bgColor;
        } else {
          if(alternate % 2 != 0) {
            bgColor = ALTERNATE_COLOR;
          } else {
            bgColor = BASE_COLOR;
          }
          trackingIdMapping.emplace(rowData->getTrackingId(), RowInfo{highestRow, bgColor});
          highestRow++;
          alternate++;
        }
      }

      // =====================================
      // Alternating row color
      // =====================================
      if(!isColoc) {
        auto key = rowData->getParentId();
        if(key == 0) {
          key = rowData->getObjectId();
        }
        if(!startOfNewParent.contains(key)) {
          if(alternate % 2 != 0) {
            bgColor = ALTERNATE_COLOR;
          } else {
            bgColor = BASE_COLOR;
          }
          alternate++;
          startOfNewParent[key] = {rowToPlace, bgColor};
        } else {
          bgColor = startOfNewParent.at(key).bgColor;
        }
      }

      // =========================================
      // Add object ID
      // =========================================
      if(isImageView) {
        auto colKey           = actColumnKey;
        colKey.measureChannel = enums::Measurement::OBJECT_ID;
        colKey.stats          = enums::Stats::OFF;
        tableToSet->setColHeader(COL_IDX_OBJECT_ID, colKey);

        auto objectIdCell = std::make_shared<joda::table::TableCell>(rowData);
        objectIdCell->setIsObjectIdCell(true);
        if(bgColor == BASE_COLOR) {
          objectIdCell->setBackgroundColor(LIGHT_BLUE);
        } else {
          objectIdCell->setBackgroundColor(DARK_BLUE);
        }
        tableToSet->setData(rowToPlace, COL_IDX_OBJECT_ID, objectIdCell);
      }

      // =========================================
      // Add data
      // =========================================
      tableToSet->setColHeader(colTableTmp, actColumnKey);
      tableToSet->setData(rowToPlace, colTableTmp, rowData);
      rowData->setBackgroundColor(bgColor);

      row++;
    }
    colTableTmp++;
  }

  // =========================================
  // Parent object ID
  // Now put intersecting class the resulting table looks like that
  //
  //  Intersect   |  Data
  // -------------|------------
  // obj=1        |par=1
  //              |par=1
  //              |par=1
  // obj=2        |par=2
  //              |par=2
  //
  //
  // =========================================

  if(nullptr != intersectingColl && isImageView && !isColoc) {
    for(const auto &[_, rowData] : intersectingColl->rows) {
      if(rowData->getObjectId() == 0 || !startOfNewParent.contains(rowData->getObjectId())) {
        continue;
      }

      auto [row, bgColor] = startOfNewParent.at(rowData->getObjectId());
      // We link to the parent. So if the users clicks on this cell, he gets the information about the parent object
      // rowData.getVal() contains the number of elements we have to fill
      for(int n = 0; n < rowData->getVal(); n++) {
        int32_t rowTemp = row + n;
        // Header
        auto colKey           = intersectingColl->colSettings;
        colKey.measureChannel = enums::Measurement::PARENT_OBJECT_ID;
        // colKey.stats          = enums::Stats::OFF;
        tableToSet->setColHeader(COL_IDX_INTERSECTING, colKey);

        // Header is filled out above
        auto objectIdCell = std::make_shared<joda::table::TableCell>(rowData);
        objectIdCell->setBackgroundColor(bgColor);
        objectIdCell->setIsObjectIdCell(true);    // This is special, we print the object ID of the parent which is the paren object ID from this
                                                  // object but the object id from the referencing object
        tableToSet->setData(rowTemp, COL_IDX_INTERSECTING, objectIdCell);
      }
    }
  }

  // adjustSize();
}

}    // namespace joda::db::data
