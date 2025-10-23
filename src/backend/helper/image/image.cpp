///
/// \file      image.cpp
/// \author    Joachim Danmayr
/// \date      2024-07-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "image.hpp"
#include <qnamespace.h>
#include <QPainter>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <string>
#include "backend/commands/image_functions/enhance_contrast/enhance_contrast.hpp"
#include "backend/commands/image_functions/resize/resize.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::image {

///
/// \brief      Constructor
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Image::Image()
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Image::setImage(const cv::Mat &imageToDisplay, const cv::Vec3f &pseudoColor, int32_t rescale)
{
  prepareGeometryChange();
  setPseudoColor(pseudoColor);

  mOriginalImageSize = {imageToDisplay.cols, imageToDisplay.rows};
  if(rescale > 0) {
    std::lock_guard<std::mutex> lock(mLockMutex);
    mOriginalImage       = imageToDisplay.clone();
    mImageOriginalScaled = joda::image::func::Resizer::resizeWithAspectRatio(imageToDisplay, std::min(imageToDisplay.cols, rescale),
                                                                             std::min(imageToDisplay.rows, rescale));
  } else {
    mImageOriginalScaled = imageToDisplay;
    mOriginalImage       = imageToDisplay;
  }
  mHistograms.clear();

  int type  = mImageOriginalScaled.type();
  int depth = type & CV_MAT_DEPTH_MASK;
  if(depth == CV_16U && 1 == mImageOriginalScaled.channels()) {
    mHistograms                   = {{}};
    static const auto histSize    = UINT16_MAX + 1;
    static const float range[2]   = {0, static_cast<float>(histSize)};
    static const float *histRange = {range};
    cv::calcHist(&mImageOriginalScaled, 1, nullptr, cv::Mat(), mHistograms.at(0), 1, &histSize, &histRange);
    cv::normalize(mHistograms.at(0), mHistograms.at(0), 0, 1, cv::NORM_MINMAX);
  } else if(depth == CV_8U && 3 == mImageOriginalScaled.channels()) {
    mHistograms                   = {{}, {}, {}};
    static const auto histSize    = UINT8_MAX + 1;
    static const float range[2]   = {0, static_cast<float>(histSize)};
    static const float *histRange = {range};
    mHistogramMax                 = histSize;

    int channels[] = {0};
    cv::calcHist(&mImageOriginalScaled, 1, channels, cv::Mat(), mHistograms.at(0), 1, &histSize, &histRange);
    cv::normalize(mHistograms.at(0), mHistograms.at(0), 0, 1, cv::NORM_MINMAX);

    channels[0] = 1;
    cv::calcHist(&mImageOriginalScaled, 1, channels, cv::Mat(), mHistograms.at(1), 1, &histSize, &histRange);
    cv::normalize(mHistograms.at(1), mHistograms.at(1), 0, 1, cv::NORM_MINMAX);

    channels[0] = 2;
    cv::calcHist(&mImageOriginalScaled, 1, channels, cv::Mat(), mHistograms.at(2), 1, &histSize, &histRange);
    cv::normalize(mHistograms.at(2), mHistograms.at(2), 0, 1, cv::NORM_MINMAX);
  }

  refreshImageToPaint(mImageOriginalScaled);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Image::setBrightnessRange(int32_t lowerValue, int32_t upperValue, int32_t displayAreaLower, int32_t displayAreaUpper)
{
  if(lowerValue > upperValue) {
    lowerValue = upperValue;
  }
  if(displayAreaLower < 0) {
    displayAreaLower = 0;
  }
  if(displayAreaUpper > UINT16_MAX) {
    displayAreaUpper = UINT16_MAX;
  }
  if(displayAreaLower > displayAreaUpper) {
    displayAreaLower = displayAreaUpper;
  }

  mLowerValue       = static_cast<uint16_t>(lowerValue);
  mUpperValue       = static_cast<uint16_t>(upperValue);
  mDisplayAreaLower = static_cast<uint16_t>(displayAreaLower);
  mDisplayAreaUpper = static_cast<uint16_t>(displayAreaUpper);

  refreshImageToPaint(mImageOriginalScaled);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Image::autoAdjustBrightnessRange()
{
  std::lock_guard<std::mutex> lock(mLockMutex);
  if(mImageOriginalScaled.empty()) {
    return;
  }
  if(3 == mImageOriginalScaled.channels()) {
    setBrightnessRange(0, 255, 0, 255);
  }
  // int histSize           = UINT16_MAX + 1;
  // float range[]          = {0, UINT16_MAX + 1};
  // const float *histRange = {range};
  // cv::Mat hist;
  // cv::calcHist(&mImageOriginalScaled, 1, nullptr, cv::Mat(), hist, 1, &histSize, &histRange);
  if(mHistograms.size() == 1) {
    auto [lowerIdx, upperIdx] = joda::cmd::EnhanceContrast::findContrastStretchBounds(mHistograms.at(0), 0.005);
    setBrightnessRange(lowerIdx, upperIdx, lowerIdx - 256, upperIdx + 256);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Image::setPseudoColor(const cv::Vec3f &color)
{
  mPseudoColor.clear();
  if(1 == mImageOriginalScaled.channels()) {
    mPseudoColor.emplace_back(color);
  } else {
    mPseudoColor.emplace_back(cv::Vec3f{1.0, 0.0, 0.0});
    mPseudoColor.emplace_back(cv::Vec3f{0.0, 1.0, 0.0});
    mPseudoColor.emplace_back(cv::Vec3f{0.0, 0.0, 1.0});
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Image::setPseudoColorEnabled(bool enabled)
{
  mPSeudoColorEnabled = enabled;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Image::refreshImageToPaint(cv::Mat &img16)
{
  if(mLowerValue > mUpperValue) {
    std::cerr << "Minimum value must be less than maximum value." << std::endl;
    return;
  }

  if(1 == mImageOriginalScaled.channels()) {
    //
    // Generate LUT
    //
    std::vector<uint16_t> lut16(65536);
    double lower = static_cast<double>(mLowerValue);
    double upper = static_cast<double>(mUpperValue);
    for(size_t i = 0; i < 65536; ++i) {
      if(i < static_cast<size_t>(lower)) {
        lut16[i] = 0;
      } else if(i > static_cast<size_t>(mUpperValue)) {
        lut16[i] = 65535;
      } else {
        lut16[i] = static_cast<uint16_t>((static_cast<double>(i) - lower) * 65535.0 / (upper - lower));
      }
    }
    //
    // Prepate pseude colors
    //
    auto color = cv::Vec3f{1.0, 1.0, 1.0};
    if(mPSeudoColorEnabled && !mPseudoColor.empty()) {
      color = mPseudoColor.at(0);
    }

    int rows = img16.rows;
    int cols = img16.cols;

    //
    // Apply LUT and pseudo color
    //
    cv::Mat color8U(rows, cols, CV_8UC3);
    for(int y = 0; y < rows; ++y) {
      const uint16_t *srcRow = img16.ptr<uint16_t>(y);
      cv::Vec3b *dstRow      = color8U.ptr<cv::Vec3b>(y);

      for(int x = 0; x < cols; ++x) {
        uint16_t val = lut16[srcRow[x]];    // apply LUT
        for(int c = 0; c < 3; ++c) {
          float scaled = static_cast<float>(val) * color[c];             // scale for channel
          dstRow[x][c] = cv::saturate_cast<uint8_t>(scaled / 256.0F);    // 16->8 bit
        }
      }
    }

    mQImage = QImage(color8U.data, color8U.cols, color8U.rows, static_cast<int>(color8U.step), QImage::Format_BGR888).copy();
  } else if(3 == mImageOriginalScaled.channels()) {
    mQImage = QImage(mImageOriginalScaled.data, mImageOriginalScaled.cols, mImageOriginalScaled.rows, static_cast<int>(mImageOriginalScaled.step),
                     QImage::Format_BGR888)
                  .copy();
  }
  update();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
QRectF Image::boundingRect() const
{
  if(mQImage.isNull() || mQImage.width() == 0) {
    return QRectF(0, 0, 0, 0);
  }
  return QRectF(QPointF(0, 0), mQImage.size());
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Image::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  painter->drawImage(0, 0, mQImage);
}

}    // namespace joda::image
