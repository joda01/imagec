///
/// \file      Intersection.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "fill_holes.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "fill_holes_settings.hpp"

namespace joda::cmd {

std::vector<int32_t> getUniqueLabels(const cv::Mat &labelMask);

FillHoles::FillHoles(const settings::FillHolesSettings &settings) : mSettings(settings)
{
}

void FillHoles::execute(processor::ProcessContext & /*context*/, cv::Mat &image, atom::ObjectList & /*resultIn*/)
{
  cv::Mat binaryImage(image.size(), CV_8UC1);
  image.convertTo(binaryImage, CV_8UC1);

  auto labels = getUniqueLabels(image);

  for(const auto label : labels) {
    cv::Mat binary = (binaryImage == label);

    // Find contours on the inverted image
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(binary, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    // Draw each contour filled with white (255) on 'des'
    for(size_t i = 0; i < contours.size(); i++) {
      // Do not paint a contour for elements inside an element.
      // In other words if there is a particle with a hole, ignore the hole.
      // See https://docs.opencv.org/4.x/d9/d8b/tutorial_py_contours_hierarchy.html
      auto parent     = hierarchy[i][3];
      auto firstChild = hierarchy[i][2];
      if(mSettings.hierarchyMode == settings::FillHolesSettings::HierarchyMode::OUTER && -1 != parent) {
        continue;
      }
      if(mSettings.hierarchyMode == settings::FillHolesSettings::HierarchyMode::INNER && firstChild > -1) {
        continue;
      }
      cv::drawContours(image, contours, static_cast<int>(i), cv::Scalar(label), cv::FILLED);
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
std::vector<int32_t> getUniqueLabels(const cv::Mat &labelMask)
{
  CV_Assert(labelMask.type() == CV_16UC1);

  // Flatten matrix to 1D
  cv::Mat flat = labelMask.reshape(1, static_cast<int>(labelMask.total()));

  // Convert to std::vector<int>
  std::vector<int32_t> vec;
  vec.assign(reinterpret_cast<const uint16_t *>(flat.datastart), reinterpret_cast<const uint16_t *>(flat.dataend));

  // Sort and remove duplicates
  std::sort(vec.begin(), vec.end());
  auto last = std::unique(vec.begin(), vec.end());
  vec.erase(last, vec.end());

  // Remove 0 (background) if needed
  vec.erase(std::remove(vec.begin(), vec.end(), 0), vec.end());

  return vec;
}

}    // namespace joda::cmd
