///
/// \file      image_saver.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <bits/iterator_concepts.h>
#include <filesystem>
#include <string>
#include "backend/commands/command.hpp"
#include "backend/commands/image_functions/image_saver/image_saver_settings.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Gaussian Blur (2D convolution)
///
class ImageSaver : public Command
{
public:
  enum Type
  {
    CONVOLVE,
    BLUR_MORE,
    FIND_EDGES
  };

  /////////////////////////////////////////////////////
  explicit ImageSaver(const settings::ImageSaverSettings &settings) : mSettings(settings)
  {
  }
  void execute(processor::ProcessContext &context, processor::ProcessorMemory &memory, cv::Mat &image,
               ObjectsListMap &result) override
  {
    auto id = DurationCount::start("Save");

    auto parentPath = context.globalContext.resultsOutputFolder;
    auto fileName   = context.imageContext.imagePath.stem();

    std::filesystem::path saveName =
        parentPath / (fileName.string() + "__" + std::to_string(std::get<0>(context.imagePipelineContext.tile)) + "x" +
                      std::to_string(std::get<1>(context.imagePipelineContext.tile)) + "__" +
                      std::to_string((int32_t) context.imagePipelineContext.cStack) + "-" +
                      std::to_string(context.imagePipelineContext.zStack) + "-" +
                      std::to_string((int32_t) context.imagePipelineContext.tStack) + mSettings.namePrefix + ".png");

    // Convert to 8-bit grayscale
    cv::Mat img_8bit_gray;
    image.convertTo(img_8bit_gray, CV_8U, 1.0 / 256);    // Scale down to 8-bit
    cv::Mat img_8bit_color;
    cvtColor(img_8bit_gray, img_8bit_color, cv::COLOR_GRAY2BGR);
    cv::imwrite(saveName.string(), img_8bit_color);

    DurationCount::stop(id);
  }

private:
  const settings::ImageSaverSettings &mSettings;
};

}    // namespace joda::cmd
