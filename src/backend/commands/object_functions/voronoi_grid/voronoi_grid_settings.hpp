#pragma once

#include <cstdint>
#include <optional>
#include <set>
#include <string>
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
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
  // Cluster where the points which should be used to generate the voronoi grid are stored in
  //
  ObjectInputClusters inputClustersPoints;

  //
  // To which cluster the result should be assigned to
  //
  ClassificatorSetting outputClustersVoronoi;

  //
  // Cluster which contains the masking classes
  //
  ObjectInputClusters inputClustersMask;

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

  settings::ObjectInputClusters getInputClustersAndClasses() const override
  {
    settings::ObjectInputClusters clusters;
    for(const auto &in : inputClustersPoints) {
      clusters.emplace(in);
    }

    for(const auto &in : inputClustersMask) {
      clusters.emplace(in);
    }
    return clusters;
  }

  [[nodiscard]] ObjectOutputClusters getOutputClustersAndClasses() const override
  {
    return {outputClustersVoronoi};
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(VoronoiGridSettings, inputClustersPoints, outputClustersVoronoi, inputClustersMask,
                                                       excludeAreasWithoutPoint, excludeAreasAtTheEdge, maxRadius, minAreaSize, maxAreaSize);
};

}    // namespace joda::settings
