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

protected:
  /////////////////////////////////////////////////////
  auto getFilterSettings() const -> const joda::settings::json::ChannelFiltering *
  {
    return mFilterSettings;
  }

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

      if(paintRectangel) {
        rectangle(img, result[i].getBoundingBox(), RED, 1 * THICKNESS);
      }

      if(!result[i].getMask().empty()) {
        mask(result[i].getBoundingBox()).setTo(RED, result[i].getMask());
        std::vector<std::vector<cv::Point>> contours;
        findContours(result[i].getMask(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        drawContours(img(result[i].getBoundingBox()), contours, -1, cv::Scalar(0, 255, 0), 1);
      }
      std::string label = std::to_string(result[i].getIndex());
      // drawLabel(img, label, left, top);
    }
    addWeighted(mask, 0.5, img, 1, 0, img);
  }

  /////////////////////////////////////////////////////
  static inline cv::Scalar BLACK  = cv::Scalar(0, 0, 0);
  static inline cv::Scalar WHITE  = cv::Scalar(255, 255, 255);
  static inline cv::Scalar YELLOW = cv::Scalar(0, 255, 255);
  static inline cv::Scalar RED    = cv::Scalar(0, 0, 255);
  static inline cv::Scalar GREEN  = cv::Scalar(0, 255, 0);
  static inline int THICKNESS     = 1;

  // Text parameters.
  static inline float FONT_SCALE = 0.7;
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
