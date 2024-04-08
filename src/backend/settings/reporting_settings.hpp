///
/// \file      reporting_settings.hpp
/// \author
/// \date      2024-04-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <cstdint>
#include <set>
#include <nlohmann/json.hpp>

namespace joda::settings::json {

class ReportingSettings
{
public:
  class DetailReport
  {
    std::set<uint32_t> measurement;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(DetailReport, measurement);
  };
  class OverviewReport
  {
    std::set<uint32_t> measurement;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(OverviewReport, measurement);
  };
  class Heatmap
  {
    std::set<uint32_t> measurement;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Heatmap, measurement);
  };

  auto getDetailReportSettings() const -> const DetailReport &
  {
    return detail;
  }

  auto getOverviewReportSettings() const -> const OverviewReport &
  {
    return overview;
  }

  auto getHeatmapSettings() const -> const Heatmap &
  {
    return heatmap;
  }

private:
  /////////////////////////////////////////////////////
  DetailReport detail;
  OverviewReport overview;
  Heatmap heatmap;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ReportingSettings, detail, overview, heatmap);
};

}    // namespace joda::settings::json
