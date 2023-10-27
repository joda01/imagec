///
/// \file      detection.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-23
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <exception>
#include <string>
#include "../../functions/func_types.hpp"
#include <opencv2/imgproc.hpp>

namespace joda::func {

///
/// \class      Detection
/// \author     Joachim Danmayr
/// \brief
///
class DetectionFunction
{
public:
  DetectionFunction(const joda::settings::json::ChannelFiltering *filt) : mFilterSettings(filt)
  {
  }
  /////////////////////////////////////////////////////
  virtual auto forward(const cv::Mat &srcImg, const cv::Mat &originalImage) -> DetectionResponse = 0;

  ///
  /// \brief      Paints the masks and bounding boxes around the found elements
  /// \author     Joachim Danmayr
  /// \param[in]  img    Image where the mask should be painted on
  /// \param[in]  result Prediction result of the forward
  ///
  static void paintBoundingBox(cv::Mat &img, const DetectionResults &result, bool paintRectangel = true)
  {
    cv::Mat mask = img.clone();

    for(int i = 0; i < result.size(); i++) {
      int left      = result[i].getBoundingBox().x;
      int top       = result[i].getBoundingBox().y;
      int width     = result[i].getBoundingBox().width;
      int height    = result[i].getBoundingBox().height;
      int color_num = i;

      if(paintRectangel && !result[i].getBoundingBox().empty()) {
        // rectangle(img, result[i].getBoundingBox(), RED, 1 * THICKNESS, cv::LINE_4);
      }
      if(!result[i].getMask().empty() && !result[i].getBoundingBox().empty()) {
        try {
          mask(result[i].getBoundingBox()).setTo(RED, result[i].getMask());
          std::vector<std::vector<cv::Point>> contours;
          findContours(result[i].getMask(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
          drawContours(img(result[i].getBoundingBox()), contours, -1, cv::Scalar(0, 255, 0), 1);
        } catch(const std::exception &ex) {
          std::cout << "P" << ex.what() << std::endl;
        }
      }
      std::string label = std::to_string(result[i].getIndex());
      // drawLabel(img, label, left, top);
    }
    addWeighted(mask, 0.5, img, 1, 0, img);
  }

  struct OverlaySettings
  {
    const DetectionResults &result;
    cv::Scalar backgroundColor;
    cv::Scalar borderColor;
    bool paintRectangel;
    float opaque;
  };

  static void paintOverlay(cv::Mat &img, const std::vector<OverlaySettings> &overlays)
  {
    for(const auto &ov : overlays) {
      cv::Mat mask = img.clone();

      for(int i = 0; i < ov.result.size(); i++) {
        int left      = ov.result[i].getBoundingBox().x;
        int top       = ov.result[i].getBoundingBox().y;
        int width     = ov.result[i].getBoundingBox().width;
        int height    = ov.result[i].getBoundingBox().height;
        int color_num = i;

        if(ov.paintRectangel && !ov.result[i].getBoundingBox().empty()) {
          rectangle(mask, ov.result[i].getBoundingBox(), RED, 1 * THICKNESS, cv::LINE_4);
        }
        if(!ov.result[i].getMask().empty() && !ov.result[i].getBoundingBox().empty()) {
          try {
            mask(ov.result[i].getBoundingBox()).setTo(ov.backgroundColor, ov.result[i].getMask());
            std::vector<std::vector<cv::Point>> contours;
            findContours(ov.result[i].getMask(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
            drawContours(mask(ov.result[i].getBoundingBox()), contours, -1, ov.borderColor, 1);
          } catch(const std::exception &ex) {
            std::cout << "P" << ex.what() << std::endl;
          }
        }
        std::string label = std::to_string(ov.result[i].getIndex());
        // drawLabel(img, label, left, top);
      }
      try {
        addWeighted(mask, ov.opaque, img, 1, 0, img);
      } catch(const std::exception &ex) {
        std::cout << "P" << ex.what() << std::endl;
      }
    }
  }

protected:
  /////////////////////////////////////////////////////
  auto getFilterSettings() const -> const joda::settings::json::ChannelFiltering *
  {
    return mFilterSettings;
  }

  /////////////////////////////////////////////////////
  static inline cv::Scalar BLACK  = cv::Scalar(0, 0, 0);
  static inline cv::Scalar WHITE  = cv::Scalar(255, 255, 255);
  static inline cv::Scalar YELLOW = cv::Scalar(0, 255, 255);
  static inline cv::Scalar RED    = cv::Scalar(0, 0, 255);
  static inline cv::Scalar GREEN  = cv::Scalar(0, 255, 0);
  static inline int THICKNESS     = 1;

  // Text parameters.
  static inline float FONT_SCALE = 0.3;
  static inline int FONT_FACE    = cv::FONT_HERSHEY_SIMPLEX;

private:
  const joda::settings::json::ChannelFiltering *mFilterSettings = nullptr;

  ///
  /// \brief      Draw labels
  /// \author     Joachim Danmayr
  /// \param[in,out]  inputImage input image where bounding boxes should be painted on
  /// \param[in]      label Text to print in the image
  /// \param[in]      left position to print
  /// \param[in]      top position to print
  ///
  static void drawLabel(cv::Mat &inputImage, const std::string &label, int left, int top)
  {
    // Display the label at the top of the bounding box.
    int baseLine;
    cv::Size label_size = cv::getTextSize(label, FONT_FACE, FONT_SCALE, THICKNESS, &baseLine);
    top                 = cv::max(top, label_size.height);
    // Top left corner.
    cv::Point tlc = cv::Point(left, top);
    // Bottom right corner.
    cv::Point brc = cv::Point(left + label_size.width, top + label_size.height + baseLine);
    // Draw white rectangle.
    rectangle(inputImage, tlc, brc, BLACK, cv::FILLED);
    // Put the label on the black rectangle.
    putText(inputImage, label, cv::Point(left, top + label_size.height), FONT_FACE, FONT_SCALE, WHITE, THICKNESS);
  }
};
}    // namespace joda::func
