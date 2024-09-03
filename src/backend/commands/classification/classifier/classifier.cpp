///
/// \file      classifier.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "classifier.hpp"
#include <climits>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include "backend/global_enums.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::cmd {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Classifier::Classifier(const settings::ClassifierSettings &settings) : mSettings(settings)
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Classifier::execute(processor::ProcessContext &context, cv::Mat &imageIn, atom::ObjectList &result)
{
  const cv::Mat &image = imageIn;

  //
  // Iterate over each defined grayscale value
  //
  for(const auto &objectClass : mSettings.classifiers) {
    // Create a mask where pixels with value 1 are set to 255
    cv::Mat binaryImage(image.size(), CV_8UC1);
    binaryImage = image == objectClass.modelClassId;

    // std::unique_ptr<image::detect::DetectionResults> response = std::make_unique<image::detect::DetectionResults>();

    // Find contours in the binary image
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(binaryImage, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

    if(contours.size() > 50000) {
      WARN("Too much particles found >" + std::to_string(contours.size()) + "<, seems to be noise.");
    } else {
      // Create a mask for each contour and draw bounding boxes
      size_t i = 0;
      for(auto &contour : contours) {
        // Do not paint a contour for elements inside an element.
        // In other words if there is a particle with a hole, ignore the hole.
        // See https://docs.opencv.org/4.x/d9/d8b/tutorial_py_contours_hierarchy.html
        if(hierarchy[i][3] == -1) {
          auto boundingBox  = cv::boundingRect(contour);
          cv::Mat mask      = cv::Mat::zeros(boundingBox.size(), CV_8UC1);
          cv::Mat imagePart = binaryImage(boundingBox).clone();

          // Bring the contours box in the area of the bounding box
          for(auto &point : contour) {
            point.x = point.x - boundingBox.x;
            point.y = point.y - boundingBox.y;
          }
          cv::drawContours(mask, contours, i, cv::Scalar(255), cv::FILLED);
          // Remove inner holes from the mask
          cv::bitwise_and(mask, imagePart, mask);

          //
          // Ready to classify -> First create a ROI object to get the measurements
          //
          joda::atom::ROI detectedRoi(
              atom::ROI::RoiObjectId{.objectId   = context.acquireNextObjectId(),
                                     .clusterId  = context.getClusterId(objectClass.outputClusterNoMatch.clusterId),
                                     .classId    = context.getClassId(objectClass.outputClusterNoMatch.classId),
                                     .imagePlane = context.getActIterator()},
              context.getAppliedMinThreshold(), 0, boundingBox, mask, contour, context.getImageSize(),
              context.getActTile(), context.getTileSize());

          for(const auto &filter : objectClass.filters) {
            // If filter matches assign the new cluster and class to the ROI
            if(filter.doesFilterMatch(context, detectedRoi, filter.intensity)) {
              detectedRoi.setClusterAndClass(context.getClusterId(filter.outputCluster.clusterId),
                                             joda::processor::ProcessContext::getClassId(filter.outputCluster.classId));
            }
          }
          result.push_back(detectedRoi);
        }
        i++;
      }
    }
  }
}

}    // namespace joda::cmd
