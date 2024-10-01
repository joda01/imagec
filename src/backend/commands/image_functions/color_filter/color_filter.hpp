///
/// \file      color_filter.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-01
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "backend/commands/command.hpp"
#include "backend/commands/image_functions/color_filter/color_filter_settings.hpp"

namespace joda::cmd {

///
/// \class      Function
/// \author     Joachim Danmayr
/// \brief      Base class for an image processing function
///
class ColorFilter : public Command
{
public:
  /////////////////////////////////////////////////////
  explicit ColorFilter(const joda::settings::ColorFilterSettings &settings) : mSetting(settings)
  {
  }
  void execute(processor::ProcessContext & /*context*/, cv::Mat &image, atom::ObjectList & /*result*/) override
  {
    execute(image);
  }

  void execute(cv::Mat &image)
  {
    // Define the color you want to keep (in RGB format)
    cv::Vec3b targetColor = hexToRGB(mSetting.colorToKeep);
    int32_t tolerance     = mSetting.tolerance;

    // Define the tolerance range for each channel (R, G, B)
    cv::Scalar lowerBound(targetColor[2] - tolerance, targetColor[1] - tolerance, targetColor[0] - tolerance);    // BGR
    cv::Scalar upperBound(targetColor[2] + tolerance, targetColor[1] + tolerance, targetColor[0] + tolerance);    // BGR

    // Create a mask that isolates the target color
    cv::Mat mask;
    cv::inRange(image, lowerBound, upperBound, mask);

    // Create a result image initialized to black
    cv::Mat result = cv::Mat::zeros(image.size(), CV_8UC1);

    // Convert the original image to grayscale
    cv::Mat grayscale = cv::Mat::zeros(image.size(), CV_8UC1);
    if(mSetting.useHumanPerceptionGrayscaleConversion) {
      cv::cvtColor(image, grayscale, cv::COLOR_BGR2GRAY);
    } else {
      for(int i = 0; i < image.rows; ++i) {
        for(int j = 0; j < image.cols; ++j) {
          cv::Vec3b pixel           = image.at<cv::Vec3b>(i, j);
          uchar grayValue           = (pixel[2] + pixel[1] + pixel[0]) / 3;    // BGR format
          grayscale.at<uchar>(i, j) = grayValue;
        }
      }
    }

    // Copy grayscale values only where the mask is non-zero (color matches)
    grayscale.copyTo(result, mask);

    // Convert the grayscale image to 16-bit format
    cv::Mat grayscale16Bit;
    result.convertTo(grayscale16Bit, CV_16U, 65535.0 / 255.0);    // Scale to 16-bit

    image = grayscale16Bit.clone();
  }

private:
  /////////////////////////////////////////////////////
  static cv::Vec3b hexToRGB(const std::string &hexColor)
  {
    // Remove the '#' if it's there
    std::string hex = hexColor[0] == '#' ? hexColor.substr(1) : hexColor;

    // Convert hex string to integer values for R, G, B
    int r, g, b;
    std::stringstream ss;

    // Convert red component
    ss << std::hex << hex.substr(0, 2);
    ss >> r;
    ss.clear();

    // Convert green component
    ss << std::hex << hex.substr(2, 2);
    ss >> g;
    ss.clear();

    // Convert blue component
    ss << std::hex << hex.substr(4, 2);
    ss >> b;

    // Return the RGB color as a Vec3b (used by OpenCV)
    return cv::Vec3b(r, g, b);    // OpenCV uses BGR order
  }

  /////////////////////////////////////////////////////
  const joda::settings::ColorFilterSettings &mSetting;
};
}    // namespace joda::cmd
