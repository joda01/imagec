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

namespace joda::cmd {

void VoronoiGrid::applyFilter(processor::ProcessContext &context, const atom::SpheralIndex &voronoiGrid,
                              const atom::SpheralIndex &voronoiPoints, const atom::SpheralIndex &mask,
                              atom::SpheralIndex &response)
{
  auto filterVoronoiAreas = [this, &context, &response, &voronoiPoints, &voronoiGrid,
                             &mask](std::optional<const atom::ROI> toIntersect) {
    for(const atom::ROI &voronoiArea : voronoiGrid) {
      if(voronoiArea.getClassId() == mSettings.pointsClassOut) {
        //
        // Apply filter
        //
        auto applyFilter = [this, &context, &response, &voronoiPoints, &voronoiGrid,
                            &mask](atom::ROI &cutedVoronoiArea) {
          //
          // Areas without point are filtered out
          //
          if(mSettings.excludeAreasWithoutPoint) {
            if(!doesAreaContainsPoint(cutedVoronoiArea, voronoiPoints, mSettings.pointsClassIn)) {
              return;
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
            if(box.x <= 0 || box.y <= 0 || box.x + box.width >= imageSize.width ||
               box.y + box.height >= imageSize.height) {
              // Touches the edge
              return;
            }
          }
          response.push_back(cutedVoronoiArea);
        };

        //
        // Mask if enabled
        //
        if(toIntersect.has_value()) {
          auto [cutedVoronoiArea, ok] = voronoiArea.calcIntersection(
              voronoiArea.getId().imagePlane, toIntersect.value(), voronoiArea.getObjectId(),
              voronoiArea.getSnapAreaRadius(), 0, context.pipelineContext.actImagePlane.tile,
              context.imageContext.tileSize, voronoiArea.getClusterId(), voronoiArea.getClassId());
          if(ok) {
            applyFilter(cutedVoronoiArea);
          }
        } else {
          applyFilter(response.push_back(voronoiArea));
        }
      }
    }
  };

  for(const auto &toIntersect : mask) {
    if(mSettings.maskClasses.contains(toIntersect.getClassId())) {
      filterVoronoiAreas(toIntersect);
    }
  }
}
}    // namespace joda::cmd