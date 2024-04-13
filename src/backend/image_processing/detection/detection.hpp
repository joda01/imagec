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
#include "backend/helper/onnx_parser/onnx_parser.hpp"
#include "backend/settings/channel/channel_settings_filter.hpp"
#include <opencv2/imgproc.hpp>
#include "detection_response.hpp"

namespace joda::func {

///
/// \class      Detection
/// \author     Joachim Danmayr
/// \brief
///
class DetectionFunction
{
public:
  /////////////////////////////////////////////////////
  struct OverlaySettings
  {
    const DetectionResults *result;
    cv::Scalar backgroundColor;
    cv::Scalar borderColor;
    bool paintRectangel;
    float opaque;
  };

  /////////////////////////////////////////////////////
  DetectionFunction(const joda::settings::ChannelSettingsFilter &filt) : mFilterSettings(filt)
  {
  }

  /////////////////////////////////////////////////////
  virtual auto forward(const cv::Mat &srcImg, const cv::Mat &originalImage, joda::settings::ChannelIndex channelIndex)
      -> DetectionResponse = 0;
  static void paintBoundingBox(cv::Mat &img, const DetectionResults &result,
                               const joda::onnx::OnnxParser::Data &modelInfo, const std::string &fillColor,
                               bool paintRectangel, bool paintLabels);
  static void paintOverlay(cv::Mat &img, const std::vector<OverlaySettings> &overlays);
  static cv::Scalar hexToScalar(const std::string &hexColor);

protected:
  /////////////////////////////////////////////////////
  auto getFilterSettings() const -> const joda::settings::ChannelSettingsFilter &
  {
    return mFilterSettings;
  }

  /////////////////////////////////////////////////////
  static inline cv::Scalar BLACK   = cv::Scalar(0, 0, 0);
  static inline cv::Scalar WHITE   = cv::Scalar(255, 255, 255);
  static inline cv::Scalar YELLOW  = cv::Scalar(0, 255, 255);
  static inline cv::Scalar RED     = cv::Scalar(0, 0, 255);
  static inline cv::Scalar GREEN   = cv::Scalar(0, 255, 0);
  static inline int THICKNESS      = 2;
  static inline int FONT_THICKNESS = 2;

  // Text parameters.
  static inline float FONT_SCALE = 0.8;
  static inline int FONT_FACE    = cv::FONT_HERSHEY_SIMPLEX;

private:
  const joda::settings::ChannelSettingsFilter &mFilterSettings;

  ///
  /// \brief      Draw labels
  /// \author     Joachim Danmayr
  /// \param[in,out]  inputImage input image where bounding boxes should be painted on
  /// \param[in]      label Text to print in the image
  /// \param[in]      left position to print
  /// \param[in]      top position to print
  ///
  static void drawLabel(cv::Mat &inputImage, const cv::Scalar &foregroundColor, const std::string &label, int left,
                        int top)
  {
    // Display the label at the top of the bounding box.
    int baseLine;
    cv::Size label_size = cv::getTextSize(label, FONT_FACE, FONT_SCALE, FONT_THICKNESS, &baseLine);
    top                 = cv::max(top, label_size.height);
    // Top left corner.
    cv::Point tlc = cv::Point(left, top);
    // Bottom right corner.
    cv::Point brc = cv::Point(left + label_size.width, top + label_size.height + baseLine);
    // Draw white rectangle.
    // rectangle(inputImage, tlc, brc, BLACK, cv::FILLED);
    // Put the label on the black rectangle.
    putText(inputImage, label, cv::Point(left, top + label_size.height), FONT_FACE, FONT_SCALE, foregroundColor,
            FONT_THICKNESS);
  }
};
}    // namespace joda::func
