///
/// \file      image.hpp
/// \author    Joachim Danmayr
/// \date      2024-07-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qgraphicsitem.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <cstdint>
#include <mutex>
#include <vector>
#include "backend/image_meta/image_meta.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>

namespace joda::image {

///
/// \class      Image
/// \author     Joachim Danmayr
/// \brief      Class representing an image instance
///
class Image : public QGraphicsItem
{
public:
  /////////////////////////////////////////////////////
  Image();
  ~Image()
  {
    clear();
  }
  void setImage(const cv::Mat &imageToDisplay, const cv::Vec3f &pseudoColor, int32_t rescale = 2048);
  bool empty() const
  {
    return mImageOriginalScaled.empty();
  }
  [[nodiscard]] const cv::Mat *getImage() const
  {
    return &mImageOriginalScaled;
  }
  [[nodiscard]] const cv::Mat *getOriginalImage() const
  {
    return &mOriginalImage;
  }
  [[nodiscard]] uint16_t getHistogramDisplayAreaLower() const
  {
    return mDisplayAreaLower;
  }
  [[nodiscard]] uint16_t getHistogramDisplayAreaUpper() const
  {
    return mDisplayAreaUpper;
  }

  [[nodiscard]] uint16_t getUpperLevelContrast() const
  {
    return mUpperValue;
  }

  [[nodiscard]] uint16_t getLowerLevelContrast() const
  {
    return mLowerValue;
  }

  void clear()
  {
    std::lock_guard<std::mutex> lock(mLockMutex);
    mImageOriginalScaled = cv::Mat{};
    mOriginalImage       = cv::Mat{};
    refreshImageToPaint(mImageOriginalScaled);
  }

  auto getHistogram() const -> const std::vector<cv::Mat> &
  {
    return mHistograms;
  }

  auto getChannelColors() const -> const std::vector<cv::Vec3f> &
  {
    return mPseudoColor;
  }

  void setBrightnessRange(int32_t lowerValue, int32_t upperValue, int32_t displayAreaLower, int32_t displayAreaUpper);
  void setPseudoColorEnabled(bool);
  [[nodiscard]] bool getUsePseudoColors() const
  {
    return mPSeudoColorEnabled;
  }

  void autoAdjustBrightnessRange();
  auto getOriginalImageSize() const -> const QSize &
  {
    return mOriginalImageSize;
  }

  auto getPreviewImageSize() const -> QSize
  {
    return {mImageOriginalScaled.cols, mImageOriginalScaled.rows};
  }

  QRectF boundingRect() const override;

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

private:
  /////////////////////////////////////////////////////
  std::vector<cv::Mat> mHistograms;

  /////////////////////////////////////////////////////
  void refreshImageToPaint(cv::Mat &img);
  void setPseudoColor(const cv::Vec3f &color);

  //// BRIGHTNESS /////////////////////////////////////////////////
  uint16_t mLowerValue       = 0;
  uint16_t mUpperValue       = UINT16_MAX;
  uint16_t mDisplayAreaLower = 0;
  uint16_t mDisplayAreaUpper = 0;
  int32_t mHistogramMax      = UINT16_MAX;
  QSize mOriginalImageSize;

  //// PSEUDOCOLOR /////////////////////////////////////////////////
  bool mPSeudoColorEnabled = false;
  std::vector<cv::Vec3f> mPseudoColor{1.0, 1.0, 1.0};

  //// IMAGE /////////////////////////////////////////////////
  QImage mQImage;
  cv::Mat mImageOriginalScaled;
  cv::Mat mOriginalImage;
  mutable std::mutex mLockMutex;
  mutable std::mutex mPaintImage;
};

}    // namespace joda::image
