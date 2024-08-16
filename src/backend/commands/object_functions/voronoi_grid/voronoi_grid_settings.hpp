#pragma once

#include <cstdint>
#include <set>
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
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
  enums::ClusterIdIn maskCluster;

  //
  // Class which should be used for the points
  //
  std::set<enums::ClassId> maskClasses;

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
    CHECK(maxRadius < -1, "Max radius must be >=0.");
    CHECK(minAreaSize < -1, "Min area size must be >=0.");
    CHECK(maxAreaSize < -1, "Max area size must be >=0.");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(VoronoiGridSettings, objectStoreIn, pointsClusterIn,
                                                       pointsClassIn, pointsClusterOut, pointsClassOut, maskCluster,
                                                       maskClasses, excludeAreasWithoutPoint, excludeAreasAtTheEdge,
                                                       maxRadius, minAreaSize, maxAreaSize);
};

}    // namespace joda::settings
