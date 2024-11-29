#pragma once

#include <cstdint>
#include <optional>
#include <set>
#include <string>
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_classes.hpp"

#include "backend/helper/json_optional_parser_helper.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include "backend/settings/settings_types.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct VoronoiGridSettings : public SettingBase
{
public:
  //
  // Classs where the points which should be used to generate the voronoi grid are stored in
  //
  ObjectInputClasses inputClassesPoints;

  //
  // To which classs the result should be assigned to
  //
  enums::ClassIdIn outputClassVoronoi = enums::ClassIdIn::$;

  //
  // Classs which contains the masking classes
  //
  ObjectInputClasses inputClassesMask;

  //
  // Exclude voronoi areas with now points after a cut
  //
  bool excludeAreasWithoutPoint = true;

  //
  // Exclude areas which are touching the edge of the image
  //
  bool excludeAreasAtTheEdge = false;

  //
  // Maximum radius the voronoi are is limited to
  //
  int32_t maxRadius = -1;

  //
  // Minimum area size to be a valid area
  //
  int32_t minAreaSize = -1;

  //
  // Maximum area size to be a valid area
  //
  int32_t maxAreaSize = -1;

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  settings::ObjectInputClasses getInputClasses() const override
  {
    settings::ObjectInputClasses classes;
    for(const auto &in : inputClassesPoints) {
      classes.emplace(in);
    }

    for(const auto &in : inputClassesMask) {
      classes.emplace(in);
    }
    return classes;
  }

  [[nodiscard]] ObjectOutputClasses getOutputClasses() const override
  {
    return {outputClassVoronoi};
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(VoronoiGridSettings, inputClassesPoints, outputClassVoronoi, inputClassesMask,
                                                       excludeAreasWithoutPoint, excludeAreasAtTheEdge, maxRadius, minAreaSize, maxAreaSize);
};

}    // namespace joda::settings
