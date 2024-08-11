
#pragma once

#include <nlohmann/json.hpp>

namespace joda::settings {

class Voronoi
{
public:
  //
  // Maximum radius of a calculated voronoi area
  //
  int32_t maxVoronoiAreaRadius = -1;

  //
  // Filter out areas which have no center of mass
  //
  bool excludeAreasWithoutCenterOfMass = true;

  //
  // Filter out areas at the edges
  //
  bool excludeAreasAtEdges = false;

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Voronoi, maxVoronoiAreaRadius, excludeAreasWithoutCenterOfMass,
                                              excludeAreasAtEdges);
};

}    // namespace joda::settings
