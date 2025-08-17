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

#include "backend/commands/command.hpp"
#include "backend/enums/enum_images.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "image_cache_settings.hpp"

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Gaussian Blur (2D convolution)
///
class ImageCache : public Command
{
public:
  /////////////////////////////////////////////////////
  explicit ImageCache(const settings::ImageCacheSettings &settings) : mSettings(settings)
  {
  }
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList & /*result*/) override
  {
    joda::enums::ImageId id{.zProjection = enums::ZProjection::NONE, .imagePlane = {}, .memoryId = mSettings.memoryId};

    if(mSettings.mode == settings::ImageCacheSettings::Mode::STORE) {
      context.storeImageToCache(mSettings.memoryScope, id, context.getActImage().clone(image));
    } else if(mSettings.mode == settings::ImageCacheSettings::Mode::LOAD) {
      context.setActImage(context.loadImageFromCache(mSettings.memoryScope, id));
    }
  }

private:
  /////////////////////////////////////////////////////
  const settings::ImageCacheSettings &mSettings;
};

}    // namespace joda::cmd
