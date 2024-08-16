#pragma once

#include <cstdint>
#include <optional>
#include <set>
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/json_optional_parser_helper.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct VoronoiGridSettings
{
public:
  enums::ObjectStoreId objectStoreIn;

  //
  // Cluster where the points which should be used to generate the voronoi grid are stored in
  //
  enums::ClusterIdIn pointsClusterIn;

  //
  // Class which should be used for the points
  //
  std::set<enums::ClassId> pointsClassIn;

  //
  // To which cluster the result should be assigned to
  //
  enums::ClusterIdIn pointsClusterOut;

  //
  // To which class the result should be assigned to
  //
  enums::ClassId pointsClassOut;

  //
  // Cluster where the points which should be used to generate the voronoi grid are stored in
  //
  std::optional<enums::ClusterIdIn> maskCluster = std::nullopt;

  //
  // Class which should be used for the points
  //
  std::optional<std::set<enums::ClassId>> maskClasses = std::nullopt;

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
    if(maskCluster.has_value()) {
      CHECK(maskClasses.has_value(), "A input class for the voronoi mask must be given!");
      CHECK(!maskClasses->empty(), "Input class must be bigger than zero.");
    }
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(VoronoiGridSettings, objectStoreIn, pointsClusterIn,
                                                       pointsClassIn, pointsClusterOut, pointsClassOut, maskCluster,
                                                       maskClasses, excludeAreasWithoutPoint, excludeAreasAtTheEdge,
                                                       maxRadius, minAreaSize, maxAreaSize);
};

}    // namespace joda::settings
