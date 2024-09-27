///
/// \file      voronoi_grid.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-16
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "voronoi_grid.hpp"
#include "backend/enums/enums_clusters.hpp"

namespace joda::cmd {

void VoronoiGrid::applyFilter(processor::ProcessContext &context, const atom::SpheralIndex &voronoiGrid, const atom::SpheralIndex &voronoiPoints,
                              atom::SpheralIndex &response, atom::ObjectList &objects)
{
  auto filterVoronoiAreas = [this, &context, &response, &voronoiPoints, &voronoiGrid](const atom::ROI *toIntersect) {
    for(const atom::ROI &voronoiArea : voronoiGrid) {
      if(voronoiArea.getClassId() == context.getClassId(mSettings.outputClustersVoronoi.classId)) {
        //
        // Apply filter
        //
        auto applyFilter = [this, &context, &response, &voronoiPoints, &voronoiGrid](atom::ROI &cutedVoronoiArea) {
          //
          // Areas without point are filtered out
          //
          if(mSettings.excludeAreasWithoutPoint) {
            for(const auto &points : mSettings.inputClustersPoints) {
              if(!doesAreaContainsPoint(cutedVoronoiArea, voronoiPoints, {context.getClassId(points.classId)})) {
                return;
              }
            }
          }

          //
          // Check area sizeW
          //
          if((mSettings.minAreaSize >= 0 && cutedVoronoiArea.getAreaSize() < mSettings.minAreaSize) ||
             (mSettings.maxAreaSize >= 0 && cutedVoronoiArea.getAreaSize() > mSettings.maxAreaSize)) {
            return;
          }

          //
          // Remove area at the edges if filter enabled
          //
          if(mSettings.excludeAreasAtTheEdge) {
            auto box       = cutedVoronoiArea.getBoundingBox();
            auto imageSize = context.getImageSize();
            if(box.x <= 0 || box.y <= 0 || box.x + box.width >= imageSize.width || box.y + box.height >= imageSize.height) {
              // Touches the edge
              return;
            }
          }
          response.push_back(cutedVoronoiArea);
        };

        //
        // Mask if enabled
        //
        if(toIntersect != nullptr) {
          auto cutedVoronoiArea =
              voronoiArea.calcIntersection(voronoiArea.getId().imagePlane, *toIntersect, voronoiArea.getSnapAreaRadius(), 0, context.getActTile(),
                                           context.getTileSize(), voronoiArea.getClusterId(), voronoiArea.getClassId());
          if(!cutedVoronoiArea.isNull()) {
            applyFilter(cutedVoronoiArea);
          }
        } else {
          auto areaClone = voronoiArea.clone();
          applyFilter(areaClone);
        }
      }
    }
  };

  if(!mSettings.inputClustersMask.empty()) {
    for(const auto &maskIn : mSettings.inputClustersMask) {
      const auto *mask = objects.at(context.getClusterId(maskIn.clusterId)).get();
      for(const auto &toIntersect : *mask) {
        if(context.getClassId(maskIn.classId) == toIntersect.getClassId()) {
          filterVoronoiAreas(&toIntersect);
        }
      }
    }
  } else {
    filterVoronoiAreas(nullptr);
  }
}
}    // namespace joda::cmd
