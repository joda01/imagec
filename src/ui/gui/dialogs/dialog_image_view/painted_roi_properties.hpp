///
/// \file      painted_roi_properties.hpp
/// \author    Joachim Danmayr
/// \date      2025-09-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qcolor.h>
#include <qgraphicsitem.h>
#include <qpolygon.h>
#include <qtypes.h>
#include <vector>
#include "backend/artifacts/roi/roi.hpp"
#include <opencv2/core/types.hpp>

namespace joda::ui::gui {

struct PaintedRoiProperties
{
  int32_t pixelClass = 0;
  QColor pixelClassColor;
  QGraphicsPolygonItem *item;

  joda::atom::ROI qPolygonToRoi(const cv::Mat *image, const cv::Size &previewSize) const
  {
    float scaleX = static_cast<float>(image->cols) / static_cast<float>(previewSize.width);
    float scaleY = static_cast<float>(image->rows) / static_cast<float>(previewSize.height);

    QPolygonF poly = item->polygon();    // get polygon
    // Convert to cv::Point
    std::vector<cv::Point> contour;
    contour.reserve(static_cast<size_t>(poly.size()));
    for(int i = 0; i < poly.size(); ++i) {
      contour.emplace_back(static_cast<int>(static_cast<float>(poly[i].x()) * scaleX), static_cast<int>(static_cast<float>(poly[i].y()) * scaleY));
    }

    // Make contour
    auto boundingBox = cv::boundingRect(contour);
    cv::Mat mask     = cv::Mat::zeros(boundingBox.size(), CV_8UC1);

    // Bring the contours box in the area of the bounding box
    for(auto &point : contour) {
      point.x = point.x - boundingBox.x;
      point.y = point.y - boundingBox.y;
    }

    std::vector<std::vector<cv::Point>> contours = {contour};
    cv::drawContours(mask, contours, -1, cv::Scalar(255), cv::FILLED);

    //
    // Ready to classify -> First create a ROI object to get the measurements
    //
    joda::atom::ROI detectedRoi(
        atom::ROI::RoiObjectId{.classId = static_cast<enums::ClassId>(pixelClass), .imagePlane = {.tStack = 0, .zStack = 0, .cStack = 0}}, 1.0,
        boundingBox, mask, contour, image->size(), image->size(), {0, 0}, image->size());

    return detectedRoi;
  }

  static PaintedRoiProperties fromRoiToQPolygon(QPolygonF &polygon, const joda::atom::ROI &roi, const cv::Mat *image, const cv::Size &previewSize)
  {
    double scaleX = static_cast<double>(previewSize.width) / static_cast<double>(image->cols);
    double scaleY = static_cast<double>(previewSize.height) / static_cast<double>(image->rows);

    auto box  = roi.getBoundingBoxTile();
    auto mask = roi.getContour();

    for(const auto &point : mask) {
      auto xP = static_cast<double>(static_cast<double>(point.x + box.x) * scaleX);
      auto yP = static_cast<double>(static_cast<double>(point.y + box.y) * scaleY);
      polygon.push_back({xP, yP});
    }

    PaintedRoiProperties prop;
    prop.pixelClass = static_cast<int32_t>(roi.getClassId());

    return prop;
  }
};
}    // namespace joda::ui::gui
