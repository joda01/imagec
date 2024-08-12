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
#include "backend/helper/roi/roi.hpp"
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
void Classifier::execute(processor::ProcessContext &context, processor::ProcessorMemory &memory, cv::Mat &imageIn,
                         ObjectsListMap &result)
{
  auto id = DurationCount::start("Classifier");

  const cv::Mat &image = imageIn;
  for(const auto &objectClass : mSettings.objectClasses) {
    // Create a mask where pixels with value 1 are set to 255
    cv::Mat binaryImage(image.size(), CV_8UC1);
    binaryImage = image == objectClass.grayscaleValue;

    // std::unique_ptr<image::detect::DetectionResults> response = std::make_unique<image::detect::DetectionResults>();

    // Find contours in the binary image
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(binaryImage, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

    if(contours.size() > 50000) {
      joda::log::logWarning("Too much particles found >" + std::to_string(contours.size()) + "<, seems to be noise.");
    } else {
      // Create a mask for each contour and draw bounding boxes
      size_t idx = 0;
      size_t i   = 0;
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
          joda::roi::ROI detect(idx, context.imageProcessingContext.appliedMinThreshold, objectClass.classId,
                                boundingBox, mask, contour, context.imagePipelineContext.originalImage,
                                objectClass.channelId,
                                joda::roi::ChannelSettingsFilter{.maxParticleSize = objectClass.filter.maxParticleSize,
                                                                 .minParticleSize = objectClass.filter.minParticleSize,
                                                                 .minCircularity  = objectClass.filter.minCircularity,
                                                                 .snapAreaSize    = objectClass.filter.snapAreaSize});
          idx++;
          result.push_back(detect);
        }
        i++;
      }
    }
  }

  DurationCount::stop(id);
}

}    // namespace joda::cmd
