///
/// \file      objects_list.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "objects_list.hpp"
#include <cstdint>
#include <vector>
#include "backend/global_enums.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

namespace joda::cmd {

void ObjectsListMap::createBinaryImage(cv::Mat &img, const std::set<joda::enums::ObjectClassId> &objectClasses) const
{
  for(const auto &roi : *this) {
    if(roi.isValid() && objectClasses.contains(roi.getClassId())) {
      int left   = roi.getBoundingBox().x;
      int top    = roi.getBoundingBox().y;
      int width  = roi.getBoundingBox().width;
      int height = roi.getBoundingBox().height;

      if(!roi.getMask().empty() && !roi.getBoundingBox().empty() && roi.getBoundingBox().x >= 0 &&
         roi.getBoundingBox().y >= 0 && roi.getBoundingBox().width >= 0 && roi.getBoundingBox().height >= 0 &&
         roi.getBoundingBox().x + roi.getBoundingBox().width <= img.cols &&
         roi.getBoundingBox().y + roi.getBoundingBox().height <= img.rows) {
        try {
          img(roi.getBoundingBox()).setTo(cv::Scalar(UINT16_MAX), roi.getMask());
        } catch(const std::exception &ex) {
          std::cout << "PA: " << ex.what() << std::endl;
        }
      }
    }
  }
}

}    // namespace joda::cmd
