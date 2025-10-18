///
/// \file      margin_crop.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/artifacts/roi/roi.hpp"
#include "backend/commands/command.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "voronoi_grid_settings.hpp"

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Base class for an image processing function
///
class VoronoiGrid : public cmd::Command
{
public:
  /////////////////////////////////////////////////////
  explicit VoronoiGrid(const settings::VoronoiGridSettings &settings) : mSettings(settings)
  {
    // Extract points from the result bounding boxes
  }

  void execute(processor::ProcessContext &context, cv::Mat & /*image*/, atom::ObjectList &objects) override
  {
    //
    // Prepare points
    //
    cv::Size size = context.getImageSize();
    cv::Rect rect(0, 0, size.width, size.height);
    cv::Subdiv2D subdiv(rect);
    atom::SpheralIndexStandAlone voronoiPoints;

    for(const auto &inputPoints : mSettings.inputClassesPoints) {
      const auto *voronoiPointsTmp = context.loadObjectsFromCache()->at(context.getClassId(inputPoints)).get();

      // Create an instance of Subdiv2D

      for(const auto &res : *voronoiPointsTmp) {
        if(context.getClassId(inputPoints) == res.getClassId()) {
          voronoiPoints.emplace(res);
          const auto boundingBox = res.getBoundingBoxTile(context.getTileInfo());
          int x                  = static_cast<int>(static_cast<float>(boundingBox.x) + static_cast<float>(boundingBox.width) / 2.0F);
          int y                  = static_cast<int>(static_cast<float>(boundingBox.y) + static_cast<float>(boundingBox.height) / 2.0F);
          subdiv.insert(cv::Point2f(static_cast<float>(x), static_cast<float>(y)));
        }
      }
    }

    //
    // Draw delaunay triangles
    //
    /*
     cv::Scalar delaunay_color(255, 255, 255), points_color(0, 0, 255);
     drawDelaunay(image, subdiv, delaunay_color);
     cv::imwrite("test.jpg", image);
     */

    atom::SpheralIndexStandAlone voronoiGrid;
    drawVoronoi(context, size, subdiv, mSettings.maxRadius, voronoiGrid);
    applyFilter(context, voronoiGrid, voronoiPoints, objects);
  }

  ///
  /// \brief      Draw voronoi grid
  /// \author     Joachim Danmayr
  /// \ref        https://learnopencv.com/delaunay-triangulation-and-voronoi-diagram-using-opencv-c-python/
  /// \param[in]   subdiv   Sub division points
  ///
  void drawVoronoi(processor::ProcessContext &context, const cv::Size &imgSize, cv::Subdiv2D &subdiv, float circleSize,
                   atom::SpheralIndexStandAlone &result) const
  {
    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);

    for(size_t i = 0; i < facets.size(); i++) {
      std::vector<cv::Point> ifacet;
      // std::vector<std::vector<cv::Point>> ifacets(1);

      cv::Mat points1_array(3, 1, CV_8U);
      ifacet.resize(facets[i].size());
      for(size_t j = 0; j < facets[i].size(); j++) {
        ifacet[j] = facets[i][j];
      }

      cv::Mat tmpImage(imgSize, CV_8UC1);

      auto [pixelWidth, pixelHeight, _] = context.getPhysicalPixelSIzeOfImage().getPixelSize(context.getPipelineRealValuesUnit());

      double radiusPx = static_cast<double>(circleSize) / pixelWidth;     // radius in x direction (pixels)
      double radiusPy = static_cast<double>(circleSize) / pixelHeight;    // radius in y direction (pixels)
      std::vector<cv::Point> circleMask;
      cv::ellipse2Poly(centers[i], cv::Size(static_cast<int32_t>(radiusPx), static_cast<int32_t>(radiusPy)), 0, 0, 360, 1, circleMask);

      cv::Mat mask1 = cv::Mat::zeros(tmpImage.size(), CV_8UC1);
      fillConvexPoly(mask1, ifacet, cv::Scalar(255), 8, 0);

      cv::Mat mask2 = cv::Mat::zeros(tmpImage.size(), CV_8UC1);
      fillConvexPoly(mask2, circleMask, cv::Scalar(255), 8, 0);

      cv::Mat resultImage = cv::Mat::zeros(tmpImage.size(), CV_8UC1);
      if(circleSize >= 0) {
        cv::bitwise_and(mask1, mask2, resultImage);
      } else {
        resultImage = mask1;
      }

      auto box        = cv::boundingRect(resultImage);
      cv::Mat boxMask = resultImage(box) >= 0.2;

      std::vector<std::vector<cv::Point>> contours;
      cv::findContours(boxMask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
      if(contours.empty()) {
        contours.emplace_back();
      }
      // Look for the biggest contour area
      size_t idxMax = 0;
      for(size_t ii = 1; ii < contours.size(); ii++) {
        if(contours[ii - 1].size() < contours[ii].size()) {
          idxMax = ii;
        }
      }

      atom::ROI roi(atom::ROI::RoiObjectId{.classId = context.getClassId(mSettings.outputClassVoronoi), .imagePlane = context.getActIterator()}, 1,
                    box, boxMask, contours[idxMax], context.getTileInfo());
      if(roi.getClassId() != enums::ClassId::NONE) {
        result.push_back(roi);
      }
    }
  }

  void applyFilter(processor::ProcessContext &context, const atom::SpheralIndex &voronoiGrid, const atom::SpheralIndex &voronoiPoints,
                   atom::ObjectList &objects);

  static bool doesAreaContainsPoint(const atom::ROI &voronoiArea, const atom::SpheralIndex &voronoiPoints,
                                    const std::set<enums::ClassId> &pointsClassIn)
  {
    for(const auto &point : voronoiPoints) {
      if(pointsClassIn.contains(point.getClassId())) {
        if(voronoiArea.isIntersecting(point, 0.1F)) {
          return true;
        }
      }
    }
    return false;
  }

  /*
    // Draw delaunay triangles
    static void drawDelaunay(const cv::Mat &img, cv::Subdiv2D &subdiv, cv::Scalar delaunay_color,
                             atom::ObjectList &result)
    {
      std::vector<cv::Vec6f> triangleList;
      subdiv.getTriangleList(triangleList);
      std::vector<cv::Point> pt(3);
      cv::Size size = img.size();
      cv::Rect rect(0, 0, size.width, size.height);

      for(size_t i = 0; i < triangleList.size(); i++) {
        cv::Vec6f t = triangleList[i];
        pt[0]       = cv::Point(cvRound(t[0]), cvRound(t[1]));
        pt[1]       = cv::Point(cvRound(t[2]), cvRound(t[3]));
        pt[2]       = cv::Point(cvRound(t[4]), cvRound(t[5]));

        // Draw rectangles completely inside the image.
        if(rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2])) {
          line(img, pt[0], pt[1], delaunay_color, 1, cv::LINE_AA, 0);
          line(img, pt[1], pt[2], delaunay_color, 1, cv::LINE_AA, 0);
          line(img, pt[2], pt[0], delaunay_color, 1, cv::LINE_AA, 0);
        }
      }
    }
  */

private:
  /////////////////////////////////////////////////////
  const settings::VoronoiGridSettings &mSettings;
};

}    // namespace joda::cmd
