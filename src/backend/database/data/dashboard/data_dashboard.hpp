///
/// \file      data_dashboard.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <memory>
#include "backend/helper/table/table.hpp"

namespace joda::db::data {

class Dashboard
{
public:
  enum class DashboardType
  {
    UNKNOWN,
    NORMAL,
    INTERSECTION,
    DISTANCE,
    COLOC
  };

  struct TabWindowKey
  {
    DashboardType type = DashboardType::UNKNOWN;
    uint32_t key       = 0;
    bool operator<(const TabWindowKey &in) const
    {
      return ((static_cast<uint64_t>(type) << 32) | key) < ((static_cast<uint64_t>(in.type) << 32) | in.key);
    }
  };

  /////////////////////////////////////////////////////
  Dashboard();

  auto convert(const std::shared_ptr<joda::table::Table> tableIn, const std::set<std::set<enums::ClassId>> &classesWithSameTrackingId,
               bool isImageView) -> std::map<TabWindowKey, std::shared_ptr<joda::table::Table>>;

private:
  /////////////////////////////////////////////////////
  static void setData(const std::shared_ptr<joda::table::Table> &tableToSet, const std::string &description,
                      const std::vector<const table::TableColumn *> &cols, bool isImageView, bool isColoc,
                      const table::TableColumn *intersectingColl);
};

}    // namespace joda::db::data
