///
/// \file      voronoi_grid.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-16
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "voronoi_grid.hpp"

namespace joda::cmd {

void VoronoiGrid::applyFilter(processor::ProcessContext &context, const atom::SpheralIndex &voronoiGrid, const atom::SpheralIndex &voronoiPoints,
                              atom::ObjectList &objects)
{
  const atom::SpheralIndex *response = objects[context.getClassId(mSettings.outputClassVoronoi)].get();

  auto filterVoronoiAreas = [this, &context, &response, &voronoiPoints, &voronoiGrid, &objects](const atom::ROI *toIntersect) {
    for(const atom::ROI &voronoiArea : voronoiGrid) {
      if(voronoiArea.getClassId() == context.getClassId(mSettings.outputClassVoronoi)) {
        //
        // Apply filter
        //
        auto applyFilter = [this, &context, &response, &voronoiPoints, &voronoiGrid, &objects](atom::ROI &cutedVoronoiArea) {
          //
          // Areas without point are filtered out
          //
          if(mSettings.excludeAreasWithoutPoint) {
            for(const auto &points : mSettings.inputClassesPoints) {
              if(!doesAreaContainsPoint(cutedVoronoiArea, voronoiPoints, {context.getClassId(points)})) {
                return;
              }
            }
          }

          //
          // Check area sizeW
          //
          const auto &physicalSize = context.getPhysicalPixelSIzeOfImage();
          if((mSettings.minAreaSize >= 0 &&
              cutedVoronoiArea.getAreaSize(physicalSize, context.getPipelineRealValuesUnit()) < static_cast<double>(mSettings.minAreaSize)) ||
             (mSettings.maxAreaSize >= 0 &&
              cutedVoronoiArea.getAreaSize(physicalSize, context.getPipelineRealValuesUnit()) > static_cast<double>(mSettings.maxAreaSize))) {
            return;
          }

          //
          // Remove area at the edges if filter enabled
          //
          if(mSettings.excludeAreasAtTheEdge) {
            auto box       = cutedVoronoiArea.getBoundingBoxTile();
            auto imageSize = context.getImageSize();
            if(box.x <= 0 || box.y <= 0 || box.x + box.width >= imageSize.width || box.y + box.height >= imageSize.height) {
              // Touches the edge
              return;
            }
          }
          objects.push_back(cutedVoronoiArea);
        };

        //
        // Mask if enabled
        //
        if(toIntersect != nullptr) {
          auto cutedVoronoiArea = voronoiArea.calcIntersection(voronoiArea.getId().imagePlane, *toIntersect, 0, context.getActTile(),
                                                               context.getTileSize(), voronoiArea.getClassId());
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

  if(!mSettings.inputClassesMask.empty()) {
    for(const auto &maskIn : mSettings.inputClassesMask) {
      const auto *mask = objects.at(context.getClassId(maskIn)).get();
      for(const auto &toIntersect : *mask) {
        if(context.getClassId(maskIn) == toIntersect.getClassId()) {
          filterVoronoiAreas(&toIntersect);
        }
      }
    }
  } else {
    filterVoronoiAreas(nullptr);
  }
}
}    // namespace joda::cmd
