///
/// \file      project_object_class.hpp
/// \author    Joachim Danmayr
/// \date      2024-04-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///

#pragma once

#include <set>
#include <vector>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

static inline const std::vector<std::string> COLORS = {

    "#9933FF",    // - Purple

    // Blue tones
    "#3399FF",    // - Electric Blue
    "#33A1FF",    // - Sky Blue
    "#33CFFF",    // - Light Cyan
    "#33FFD1",    // - Aqua

    // Green tones
    "#33FFCC",    // - Mint Green
    "#33FF99",    // - Spring Green
    "#33FF88",    // - Neon Green
    "#33FF57",    // - Bright Green
    "#66FF33",    // - Lime Green

    // Yellow tones
    "#5EFF00",    // - Green yellow
    "#D2FF00",    // - Green yellow
    "#FFFF33",    // - Bright Yellow
    "#F6FF00",    // - Yellow
    "#FFCC33",    // - Golden Yellow

    // Orange tones
    "#FFD200",    // - Light orange
    "#FF8C33",    // - Deep Orange
    "#FF9B00",    // - Deeper orange
    "#FF6F33",    // - Pumpkin Orange
    "#FF5733",    // - Vibrant Orange
    "#FF33FF",    // - Magenta
    "#FF33C4",    // - Bright Rose
    "#FF33A1",    // - Hot Pink
    "#FF3380",    // - Electric Pink
    "#FF3333",    // - Neon red
    "#FF3366",    // - Coral Red
    "#640000",    // - Dark red

    // Gray
    "#BFBFBF"    // -Light gray
};

///
/// \class      ResultsTemplate
/// \author     Joachim Danmayr
/// \brief      Template for results
///
struct ResultsTemplate
{
  enums::Measurement measureChannel = enums::Measurement::NONE;
  std::set<enums::Stats> stats;
  // int32_t crossChannelStacksC              = -1;
  // joda::enums::ClassId intersectingChannel = joda::enums::ClassId::NONE;
  // int32_t zStack                           = 0;
  // int32_t tStack                           = 0;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ResultsTemplate, measureChannel, stats);

  void check() const
  {
  }
  // We don't want to do a error check for the history
  void getErrorLogRecursive(SettingParserLog_t &settingsParserLog) const
  {
  }
};

struct Class
{
  joda::enums::ClassId classId = joda::enums::ClassId::UNDEFINED;

  //
  //  Display name of the object class
  //
  std::string name;

  //
  //  Display color of the object class
  //
  std::string color;

  //
  //  Further notes to the object class
  //
  std::string notes;

  //
  // Default measurements for this class
  //
  std::vector<ResultsTemplate> defaultMeasurements;

  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Class, classId, name, notes, color, defaultMeasurements);
};

}    // namespace joda::settings
