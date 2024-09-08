///
/// \file      settings_export_data.hpp
/// \author    Joachim Danmayr
/// \date      2024-09-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include <set>
#include <vector>
#include "backend/enums/enum_measurements.hpp"
#include <nlohmann/json.hpp>

namespace joda::ui {

struct SettingsExportData
{
  struct Column
  {
    settings::ClassificatorSettingOut inputCluster;
    std::set<joda::enums::Measurement> measurements;
    std::set<enums::Stats> stats;
    std::set<int32_t> crossChannelIntensity;
    std::set<settings::ClassificatorSettingOut> crossChannelCount;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Column, inputCluster, measurements, stats, crossChannelIntensity,
                                                crossChannelCount);
  };

  std::vector<Column> columns;

private:
  std::string schemaId = "https://imagec.org/schemas/templates/v1/export.json";

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(SettingsExportData, columns, schemaId);
};

}    // namespace joda::ui
