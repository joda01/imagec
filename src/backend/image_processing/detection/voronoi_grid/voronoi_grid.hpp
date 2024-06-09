///
/// \file      margin_crop.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include "../detection.hpp"
#include "../detection_response.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace joda::image::detect {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Base class for an image processing function
///
class VoronoiGrid : public DetectionFunction
{
public:
  /////////////////////////////////////////////////////
  explicit VoronoiGrid(const std::unique_ptr<DetectionResults> &result, int maxRadius) :
      DetectionFunction({}), mMaxRadius(maxRadius)
  {
    // Extract points from the result bounding boxes
    for(const auto &res : *result) {
      if(res.isValid()) {
        int x = static_cast<int>(static_cast<float>(res.getBoundingBox().x) +
                                 static_cast<float>(res.getBoundingBox().width) / 2.0F);
        int y = static_cast<int>(static_cast<float>(res.getBoundingBox().y) +
                                 static_cast<float>(res.getBoundingBox().height) / 2.0F);
        mPoints.emplace_back(x, y);
      }
    }
  }

  auto forward(const cv::Mat &image, const cv::Mat &originalImage, joda::settings::ChannelIndex channelIndex)
      -> DetectionResponse override
  {
    // Rectangle to be used with Subdiv2D
    cv::Size size = image.size();
    cv::Rect rect(0, 0, size.width, size.height);

    // Create an instance of Subdiv2D
    cv::Subdiv2D subdiv(rect);

    // Insert points into subdiv
    for(auto mPoint : mPoints) {
      subdiv.insert(mPoint);
    }

    //
    // Draw delaunay triangles
    //
    /*
     cv::Scalar delaunay_color(255, 255, 255), points_color(0, 0, 255);
     drawDelaunay(image, subdiv, delaunay_color);
     cv::imwrite("test.jpg", image);
     */

    // Allocate space for Voronoi Diagram
    // Draw Voronoi diagram
    auto result = drawVoronoi(originalImage, subdiv, mMaxRadius);

    /*
    cv::Mat grayImageFloat;
    img_voronoi.convertTo(grayImageFloat, CV_32F, (float) UCHAR_MAX / (float) UCHAR_MAX);
    cv::Mat inputImage;
    cv::cvtColor(grayImageFloat, inputImage, cv::COLOR_GRAY2BGR);
    img_voronoi = inputImage * 0.5 + image;
    cv::imwrite("voronoi.png", inputImage);
    cv::imwrite("voronoi_combi.png", img_voronoi);
    */

    return result;
  }

  // Draw delaunay triangles
  static void drawDelaunay(const cv::Mat &img, cv::Subdiv2D &subdiv, cv::Scalar delaunay_color)
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

  ///
  /// \brief      Draw voronoi grid
  /// \author     Joachim Danmayr
  /// \ref        https://learnopencv.com/delaunay-triangulation-and-voronoi-diagram-using-opencv-c-python/
  /// \param[in]   subdiv   Sub division points
  ///
  static auto drawVoronoi(const cv::Mat &imgOriginal, cv::Subdiv2D &subdiv, int circleSize) -> DetectionResponse
  {
    DetectionResponse response;
    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    response.controlImage  = cv::Mat::zeros(imgOriginal.size(), CV_32FC3);
    response.originalImage = imgOriginal;

    for(size_t i = 0; i < facets.size(); i++) {
      std::vector<cv::Point> ifacet;
      std::vector<std::vector<cv::Point>> ifacets(1);

      cv::Mat points1_array(3, 1, CV_8U);
      ifacet.resize(facets[i].size());
      for(size_t j = 0; j < facets[i].size(); j++) {
        ifacet[j] = facets[i][j];
      }

      std::vector<cv::Point> circleMask;
      cv::ellipse2Poly(centers[i], cv::Size(circleSize, circleSize), 0, 0, 360, 1, circleMask);

      cv::Mat mask1 = cv::Mat::zeros(imgOriginal.size(), CV_8UC1);
      fillConvexPoly(mask1, ifacet, cv::Scalar(255), 8, 0);

      cv::Mat mask2 = cv::Mat::zeros(imgOriginal.size(), CV_8UC1);
      fillConvexPoly(mask2, circleMask, cv::Scalar(255), 8, 0);

      cv::Mat result = cv::Mat::zeros(imgOriginal.size(), CV_8UC1);
      if(circleSize >= 0) {
        cv::bitwise_and(mask1, mask2, result);
      } else {
        result = mask1;
      }

      ifacets[0] = ifacet;
      polylines(response.controlImage, ifacets, true, cv::Scalar(), 1, cv::LINE_AA, 0);
      circle(response.controlImage, centers[i], 3, cv::Scalar(), cv::FILLED, cv::LINE_AA, 0);
      // circle(img, centers[i], 80, cv::Scalar(0, 0, 0, 255), cv::FILLED, cv::LINE_AA, 0);

      auto box        = cv::boundingRect(result);
      cv::Mat boxMask = result(box) >= 0.2;

      std::vector<std::vector<cv::Point>> contours;
      cv::findContours(boxMask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
      if(contours.empty()) {
        contours.emplace_back();
      }
      // Look for the biggest contour area
      int idxMax = 0;
      for(int i = 1; i < contours.size(); i++) {
        if(contours[i - 1].size() < contours[i].size()) {
          idxMax = i;
        }
      }

      ROI roi(i, 1, 0, box, boxMask, contours[idxMax], {{joda::settings::ChannelIndex::NONE, &imgOriginal}});
      response.result->push_back(roi);
    }

    paintBoundingBox(response.controlImage, response.result, {}, "#FF0000", false, false);
    // cv::imwrite("voronoi_combi_ctrl.jpg", response.controlImage);

    return response;
  }

private:
  /////////////////////////////////////////////////////
  int mMaxRadius;
  std::vector<cv::Point2f> mPoints;
};

}    // namespace joda::image::detect
