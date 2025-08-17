///
/// \file      color_filter.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-01
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include <string>
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
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList & /*result*/) override
  {
    execute(image);
    context.setToGrayscaleImage();
  }

  void execute(cv::Mat &image)
  {
    for(const auto &filter : mSetting.filter) {
      cv::Mat hsvImageTmp;
      cvtColor(image, hsvImageTmp, cv::COLOR_BGR2HSV_FULL);

      int32_t hueMin = mSetting.filter.begin()->colorRangeFrom.hue;
      int32_t hueMax = mSetting.filter.begin()->colorRangeTo.hue;
      int32_t satMax = std::max(mSetting.filter.begin()->colorRangeFrom.sat, mSetting.filter.begin()->colorRangeTo.sat);
      int32_t satMin = std::min(mSetting.filter.begin()->colorRangeFrom.sat, mSetting.filter.begin()->colorRangeTo.sat);
      int32_t valMax = std::max(mSetting.filter.begin()->colorRangeFrom.val, mSetting.filter.begin()->colorRangeTo.val);
      int32_t valMin = std::min(mSetting.filter.begin()->colorRangeFrom.val, mSetting.filter.begin()->colorRangeTo.val);

      // Create a mask that isolates the target color
      cv::Mat combinedMask;
      if(hueMin > hueMax) {
        cv::Scalar lowerBound1((hueMin * 255) / 360, satMin, valMin);    // Hue, Satturation, Value
        cv::Scalar upperBound1((360 * 255) / 360, satMax, valMax);       // Hue, Satturation, Value

        cv::inRange(hsvImageTmp, lowerBound1, upperBound1, combinedMask);
        cv::Scalar lowerBound2((0 * 255) / 360, satMin, valMin);         // Hue, Satturation, Value
        cv::Scalar upperBound2((hueMax * 255) / 360, satMax, valMax);    // Hue, Satturation, Value

        cv::Mat mask2;
        cv::inRange(hsvImageTmp, lowerBound2, upperBound2, mask2);

        // Combine the masks
        cv::bitwise_or(combinedMask, mask2, combinedMask);

      } else {
        cv::Scalar lowerBound1((hueMin * 255) / 360, satMin, valMin);    // Hue, Satturation, Value
        cv::Scalar upperBound1((hueMax * 255) / 360, satMax, valMax);    // Hue, Satturation, Value
        cv::inRange(hsvImageTmp, lowerBound1, upperBound1, combinedMask);
      }

      // Convert the original image to grayscale
      hsvImageTmp = cv::Mat::zeros(image.size(), CV_16UC1);
      if(mSetting.grayScaleConvertMode == settings::ColorFilterSettings::GrayscaleMode::HUMAN) {
        for(int i = 0; i < image.rows; ++i) {
          for(int j = 0; j < image.cols; ++j) {
            if(combinedMask.at<uint8_t>(i, j) == 0) {
            } else {
              cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);
              auto grayValue  = static_cast<uint16_t>(
                  ((0.299F * static_cast<float>(pixel[2]) + 0.587F * static_cast<float>(pixel[1]) + 0.114F * static_cast<float>(pixel[0])) *
                   65535.0F) /
                  (3.0F * 255.0F));    // BGR format
              hsvImageTmp.at<uint16_t>(i, j) = grayValue;
            }
          }
        }
      } else {
        for(int i = 0; i < image.rows; ++i) {
          for(int j = 0; j < image.cols; ++j) {
            if(combinedMask.at<uint8_t>(i, j) == 0) {
            } else {
              cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);
              auto grayValue =
                  static_cast<uint16_t>((((static_cast<float>(pixel[2]) + static_cast<float>(pixel[1]) + static_cast<float>(pixel[0])) * 65535.0F) /
                                         (3.0F * 255.0F)));    // BGR format
              hsvImageTmp.at<uint16_t>(i, j) = grayValue;
            }
          }
        }
      }

      // Copy grayscale values only where the mask is non-zero (color matches)
      image = cv::Mat::zeros(image.size(), CV_16UC1);
      hsvImageTmp.copyTo(image, combinedMask);
    }
  }

private:
  /////////////////////////////////////////////////////
  static cv::Scalar hexToRGB(const std::string &hexColor)
  {
    // Remove the '#' if it's there
    std::string hex = hexColor[0] == '#' ? hexColor.substr(1) : hexColor;

    // Convert hex string to integer values for R, G, B
    uint8_t r;
    uint8_t g;
    uint8_t b;
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
    return cv::Vec3b(b, g, r);    // OpenCV uses BGR order
  }

  /////////////////////////////////////////////////////
  const joda::settings::ColorFilterSettings &mSetting;
};
}    // namespace joda::cmd
