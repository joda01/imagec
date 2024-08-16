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
#include "backend/artifacts/roi/roi.hpp"
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
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override
  {
    auto parentPath = context.globalContext.resultsOutputFolder;
    auto fileName   = context.imageContext.imagePath.stem();

    std::filesystem::path saveName =
        parentPath /
        (fileName.string() + "__" + std::to_string(std::get<0>(context.pipelineContext.actImagePlane.tile)) + "x" +
         std::to_string(std::get<1>(context.pipelineContext.actImagePlane.tile)) + "__" +
         std::to_string((int32_t) context.pipelineContext.actImagePlane.getId().imagePlane.cStack) + "-" +
         std::to_string(context.pipelineContext.actImagePlane.getId().imagePlane.zStack) + "-" +
         std::to_string((int32_t) context.pipelineContext.actImagePlane.getId().imagePlane.tStack) +
         mSettings.namePrefix + ".png");

    // Convert to 8-bit grayscale
    cv::Mat img_8bit_color;

    // Paint on existing image
    if(mSettings.canvas == settings::ImageSaverSettings::Canvas::IMAGE_PLANE ||
       mSettings.canvas == settings::ImageSaverSettings::Canvas::IMAGE_$) {
      cv::Mat &imageBackground = image;
      if(mSettings.canvas == settings::ImageSaverSettings::Canvas::IMAGE_PLANE && mSettings.planesIn.has_value()) {
        imageBackground = context.loadImageFromCache(mSettings.planesIn.value())->image;
      }

      cv::Mat img_8bit_gray;
      imageBackground.convertTo(img_8bit_gray, CV_8U, 1.0 / 256);    // Scale down to 8-bit
      cvtColor(img_8bit_gray, img_8bit_color, cv::COLOR_GRAY2BGR);
    } else if(mSettings.canvas == settings::ImageSaverSettings::Canvas::BLACK) {
      img_8bit_color = cv::Mat::zeros(context.getImageSize(), CV_8UC3);
    } else if(mSettings.canvas == settings::ImageSaverSettings::Canvas::WHITE) {
      img_8bit_color = cv::Mat(context.getImageSize(), CV_8UC3, cv::Scalar(255, 255, 255));
    }

    //
    // Paint the objects
    //
    for(const auto &cluster : mSettings.clustersIn) {
      const auto &clusterObjects = result.at(context.getClusterId(cluster.clusterIn));

      for(const auto &roi : clusterObjects) {
        for(const auto &classIn : cluster.classesIn) {
          drawObject(classIn, roi, img_8bit_color);
        }
      }
    }

    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(mSettings.compression);
    cv::imwrite(saveName.string(), img_8bit_color, compression_params);
  }

private:
  /////////////////////////////////////////////////////
  static inline int THICKNESS     = 1;
  static inline cv::Scalar BLACK  = cv::Scalar(0, 0, 0);
  static inline cv::Scalar WHITE  = cv::Scalar(255, 255, 255);
  static inline cv::Scalar YELLOW = cv::Scalar(0, 255, 255);
  static inline cv::Scalar RED    = cv::Scalar(0, 0, 255);
  static inline cv::Scalar GREEN  = cv::Scalar(0, 255, 0);

  /////////////////////////////////////////////////////
  void drawObject(const settings::ImageSaverSettings::Cluster::Class &settings, const atom::ROI &roi, cv::Mat &imageOut)
  {
    if(settings.classIn == roi.getClassId()) {
      int left   = roi.getBoundingBox().x;
      int top    = roi.getBoundingBox().y;
      int width  = roi.getBoundingBox().width;
      int height = roi.getBoundingBox().height;

      if(!roi.getMask().empty() && !roi.getBoundingBox().empty()) {
        auto areaColor = hexToScalar(settings.color);

        // Boundding box
        if(settings.paintBoundingBox && !roi.getBoundingBox().empty()) {
          rectangle(imageOut, roi.getBoundingBox(), areaColor, 1 * THICKNESS, cv::LINE_4);
        }

        // Fill area
        if(settings.style == settings::ImageSaverSettings::Cluster::Class::Style::FILLED) {
          imageOut(roi.getBoundingBox()).setTo(areaColor, roi.getMask());
        }

        // Paint contour only for valid particles
        cv::Scalar contourColor = hexToScalar(settings.color);
        std::vector<std::vector<cv::Point>> contours;
        contours.push_back(roi.getContour());
        if(!contours.empty()) {
          drawContours(imageOut(roi.getBoundingBox()), contours, -1, contourColor, 1);
        }

        if(roi.hasSnapArea()) {
          std::vector<std::vector<cv::Point>> contours;
          contours.push_back(roi.getSnapAreaContour());
          if(!contours.empty())
            drawContours(imageOut(roi.getSnapAreaBoundingBox()), contours, -1, contourColor, 1);
        }
      }
    }
  }

  cv::Scalar hexToScalar(const std::string &hexColor)
  {
    int r, g, b;
    sscanf(hexColor.c_str(), "#%2x%2x%2x", &r, &g, &b);
    return cv::Scalar(b, g, r);    // OpenCV uses BGR order
  }

  /////////////////////////////////////////////////////
  const settings::ImageSaverSettings &mSettings;
};

}    // namespace joda::cmd
