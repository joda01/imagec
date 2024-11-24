#pragma once

#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/global_enums.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/settings_types.hpp"

namespace joda::settings {

struct ClusterClassColorAssignment
{
  //
  // Objects to calc the intersection with
  //
  ClassificatorSettingOut clusterClass;

  //
  //  Display color of the object class
  //
  std::string color;

  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ClusterClassColorAssignment, clusterClass, color);
};

struct ClusterClassColorAssignments : std::list<ClusterClassColorAssignment>
{
  static inline const std::vector<std::string> COLORS = {
      "#FF5733",    // - Vibrant Orange
      "#33FF57",    // - Bright Green
      "#FF33FF",    // - Magenta
      "#33A1FF",    // - Sky Blue
      "#FFFF33",    // - Bright Yellow
      "#FF33A1",    // - Hot Pink
      "#33FFD1",    // - Aqua
      "#FF8C33",    // - Deep Orange
      "#9933FF",    // - Purple
      "#33FFCC",    // - Mint Green
      "#FF3380",    // - Electric Pink
      "#33FF88",    // - Neon Green
      "#FFCC33",    // - Golden Yellow
      "#33CFFF",    // - Light Cyan
      "#FF3366",    // - Coral Red
      "#66FF33",    // - Lime Green
      "#FF33C4",    // - Bright Rose
      "#33FF99",    // - Spring Green
      "#FF6F33",    // - Pumpkin Orange
      "#3399FF",    // - Electric Blue
  };

  using ::std::list<ClusterClassColorAssignment>::list;

  [[nodiscard]] std::string getColor(const ClassificatorSettingOut &in) const
  {
    for(const auto &element : *this) {
      if(element.clusterClass == in) {
        return element.color;
      }
    }
    return "";
  }

  void setColor(const ClassificatorSettingOut &in, const std::string &color)
  {
    for(auto &element : *this) {
      if(element.clusterClass == in) {
        // Still exists -> override
        element.color = color;
        return;
      }
    }
    // Does not exist -> add
    emplace_back(ClusterClassColorAssignment{.clusterClass = in, .color = color});
  }

  void getErrorLogRecursive(SettingParserLog_t &settingsParserLog) const
  {
  }
};
}    // namespace joda::settings
