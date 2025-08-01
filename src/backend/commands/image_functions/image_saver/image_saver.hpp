///
/// \file      image_saver.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <filesystem>
#include <regex>
#include <string>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/commands/command.hpp"
#include "backend/commands/image_functions/image_saver/image_saver_settings.hpp"
#include "backend/helper/base32.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
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
  std::string replaceImageName(const std::string &input, const std::string &replacement)
  {
    std::regex pattern("\\$\\{imageName\\}");
    return std::regex_replace(input, pattern, replacement);
  }

  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override
  {
    auto replacePlaceHolder = [this, &context](const std::string &inString) -> std::string {
      return replaceImageName(inString, context.getActImagePath().stem().string());
    };

    auto parentPath = context.getOutputFolder() / replacePlaceHolder(mSettings.subFolder);
    if(mSettings.outputSlot == settings::ImageSaverSettings::Output::FILE) {
      std::filesystem::create_directories(parentPath);
    }
    auto fileName = context.getActImagePath().stem();

    auto actTile = context.getActTile();

    std::filesystem::path saveName =
        parentPath / (fileName.string() + "__" + std::to_string(std::get<1>(actTile)) + "x" + std::to_string(std::get<0>(actTile)) + "__" +
                      std::to_string((int32_t) context.getActImagePlaneId().cStack) + "-" + std::to_string(context.getActImagePlaneId().zStack) +
                      "-" + std::to_string((int32_t) context.getActImagePlaneId().tStack) + mSettings.namePrefix + ".png");

    // Convert to 8-bit grayscale
    cv::Mat img_8bit_color;

    // Paint on existing image
    if(mSettings.canvas == settings::ImageSaverSettings::Canvas::IMAGE_PLANE || mSettings.canvas == settings::ImageSaverSettings::Canvas::IMAGE_$) {
      cv::Mat &imageBackground = image;
      bool isRgb               = context.isRgb();
      if(mSettings.canvas == settings::ImageSaverSettings::Canvas::IMAGE_PLANE && mSettings.planesIn.has_value()) {
        const auto *tmp = context.loadImageFromCache(enums::MemoryScope::ITERATION, mSettings.planesIn.value());
        imageBackground = tmp->image;
        isRgb           = tmp->isBinary();
      }

      if(!isRgb) {
        cv::Mat img_8bit_gray;
        imageBackground.convertTo(img_8bit_gray, CV_8U, 1.0 / 256);    // Scale down to 8-bit
        cvtColor(img_8bit_gray, img_8bit_color, cv::COLOR_GRAY2BGR);
      } else {
        img_8bit_color = imageBackground;
      }
    } else if(mSettings.canvas == settings::ImageSaverSettings::Canvas::BLACK) {
      img_8bit_color = cv::Mat::zeros(context.getImageSize(), CV_8UC3);
    } else if(mSettings.canvas == settings::ImageSaverSettings::Canvas::WHITE) {
      img_8bit_color = cv::Mat(context.getImageSize(), CV_8UC3, cv::Scalar(255, 255, 255));
    }

    //
    // Paint the objects
    //
    for(const auto &classs : mSettings.classesIn) {
      if(!result.contains(context.getClassId(classs.inputClass))) {
        continue;
      }
      const auto *classsObjects = result.at(context.getClassId(classs.inputClass)).get();
      for(const auto &roi : *classsObjects) {
        drawObject(context, classs, roi, img_8bit_color);
      }
    }

    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(mSettings.compression);

    if(settings::ImageSaverSettings::Output::FILE == mSettings.outputSlot) {
      cv::imwrite(saveName.string(), img_8bit_color, compression_params);
    } else {
      // Write image to output
      image = img_8bit_color;
    }
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
  void drawObject(processor::ProcessContext &context, const settings::ImageSaverSettings::SaveClasss &settings, const atom::ROI &roi,
                  cv::Mat &imageOut)
  {
    if(context.getClassId(settings.inputClass) == roi.getClassId()) {
      int left   = roi.getBoundingBoxTile().x;
      int top    = roi.getBoundingBoxTile().y;
      int width  = roi.getBoundingBoxTile().width;
      int height = roi.getBoundingBoxTile().height;

      auto centroid = roi.getCentroidTile();

      if(!roi.getMask().empty() && !roi.getBoundingBoxTile().empty()) {
        auto color     = context.getColorClass(settings.inputClass);
        auto areaColor = hexToScalar(color);

        // Centroid
        // cv::circle(imageOut, centroid, 3, RED, cv::FILLED);
        // Center
        // cv::circle(imageOut, {left + width / 2, top + height / 2}, 4, YELLOW, cv::FILLED);

        // Boundding box
        if(settings.paintBoundingBox && !roi.getBoundingBoxTile().empty()) {
          rectangle(imageOut, roi.getBoundingBoxTile(), areaColor, 1 * THICKNESS, cv::LINE_4);
        }

        // Paint contour only for valid particles
        cv::Scalar contourColor = hexToScalar(color);

        // Fill area
        if(settings.style == settings::ImageSaverSettings::Style::FILLED) {
          contourColor = hexToScalar("#000000");
          imageOut(roi.getBoundingBoxTile()).setTo(areaColor, roi.getMask());
        }

        std::vector<std::vector<cv::Point>> contours;
        contours.push_back(roi.getContour());
        if(!contours.empty()) {
          drawContours(imageOut(roi.getBoundingBoxTile()), contours, -1, contourColor, 1);
        }

        // Draw object ID
        if(settings.paintObjectId) {
          auto scale   = getFontScaleFromHeight(cv::FONT_HERSHEY_PLAIN, 12);
          int baseline = 0;
          auto tSize   = cv::getTextSize(helper::toBase32(roi.getObjectId()), cv::FONT_HERSHEY_PLAIN, 1, 0, &baseline);

          if(left + tSize.width > imageOut.cols) {
            left = left - tSize.width;
          }
          cv::putText(imageOut, helper::toBase32(roi.getObjectId()), cv::Point(left, top), cv::FONT_HERSHEY_PLAIN, 1, areaColor, 0);
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
