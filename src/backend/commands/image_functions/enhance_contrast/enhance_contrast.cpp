///
/// \file      Intersection.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \brief     Ported from ImageJ
///
/// \ref       https://imagej.net/ij/developer/source/ij/plugin/ContrastEnhancer.java.html
///

#include "enhance_contrast.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "enhance_contrast_settings.hpp"

namespace joda::cmd {

EnhanceContrast::EnhanceContrast(const settings::EnhanceContrastSettings &settings) : mSettings(settings)
{
}

void setMinMax(cv::Mat &image, uint16_t min, uint16_t max)
{
  // Create a lookup table for mapping pixel values
  cv::Mat lookupTable(1, 65535, CV_16U);

  for(int i = 0; i < 65535; ++i) {
    if(i < min) {
      lookupTable.at<uint16_t>(i) = 0;
    } else if(i > max) {
      lookupTable.at<uint16_t>(i) = 65535;
    } else {
      lookupTable.at<uint16_t>(i) =
          static_cast<uint16_t>((i - static_cast<float>(min)) * 65535.0 / (static_cast<float>(max) - static_cast<float>(min)));
    }
  }

  // Apply the lookup table to the source image to get the destination image
  for(int y = 0; y < image.rows; ++y) {
    for(int x = 0; x < image.cols; ++x) {
      uint16_t pixelValue      = image.at<uint16_t>(y, x);
      image.at<uint16_t>(y, x) = lookupTable.at<uint16_t>(pixelValue);
    }
  }
}

void findMinAndMax(cv::Mat &image)
{
  double max = 0;
  double min = 0;
  cv::minMaxLoc(image, &min, &max);
  setMinMax(image, static_cast<uint16_t>(min), static_cast<uint16_t>(max));
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void applyTable(cv::Mat &image, const std::array<int32_t, UINT16_MAX + 1> &lut)
{
  int lineStart = 0;
  int lineEnd   = 0;
  int v         = 0;
  for(int y = 0; y < image.rows; y++) {
    lineStart = y * image.cols;
    lineEnd   = lineStart + image.rows;
    for(int i = lineEnd; --i >= lineStart;) {
      v                     = lut[image.at<uint16_t>(i) & 0xffff];
      image.at<uint16_t>(i) = static_cast<uint16_t>(v);
    }
  }
  findMinAndMax(image);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
double getWeightedValue(cv::Mat &histogram, int i)
{
  bool classicEqualization = false;
  int h                    = histogram.at<float>(i);
  if(h < 2 || classicEqualization) {
    return static_cast<double>(h);
  }
  return std::sqrt(static_cast<double>(h));
}

std::tuple<int32_t, int32_t> getMinAndMax(cv::Mat &ip, double saturated, cv::Mat &histogram)
{
  int hmin = 0;
  int hmax = 0;
  int64_t threshold;
  int hsize = UINT16_MAX + 1;
  if(saturated > 0.0) {
    threshold = static_cast<int64_t>((ip.rows * ip.cols) * saturated / 200.0);
  } else {
    threshold = 0;
  }
  int i        = -1;
  bool found   = false;
  int count    = 0;
  int maxindex = hsize - 1;
  do {
    i++;
    count += static_cast<int32_t>(histogram.at<float>(i));
    found = count > threshold;
  } while(!found && i < maxindex);
  hmin = i;

  i     = hsize;
  count = 0;
  do {
    i--;
    count += static_cast<int32_t>(histogram.at<float>(i));
    found = count > threshold;
  } while(!found && i > 0);
  hmax = i;
  return {hmin, hmax};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void normalize(cv::Mat &ip, double min, double max)
{
  int min2                       = 0;
  static constexpr uint16_t max2 = UINT16_MAX;
  static constexpr size_t range  = UINT16_MAX + 1;

  std::array<int32_t, range> lut;
  for(int i = 0; i < range; i++) {
    if(i <= min) {
      lut[i] = 0;
    } else if(i >= max) {
      lut[i] = max2;
    } else {
      lut[i] = (int) (((double) (i - min) / (max - min)) * max2);
    }
  }
  applyTable(ip, lut);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void equalize(cv::Mat &ip, cv::Mat &histogram)
{
  static constexpr uint16_t max   = UINT16_MAX;
  static constexpr uint16_t range = UINT16_MAX;

  double sum;
  sum = getWeightedValue(histogram, 0);
  for(int i = 1; i < max; i++) {
    sum += 2 * getWeightedValue(histogram, i);
  }
  sum += getWeightedValue(histogram, max);
  double scale = range / sum;
  std::array<int32_t, range + 1> lut;
  lut[0] = 0;
  sum    = getWeightedValue(histogram, 0);
  for(int i = 1; i < max; i++) {
    double delta = getWeightedValue(histogram, i);
    sum += delta;
    lut[i] = static_cast<int>(std::round(sum * scale));
    sum += delta;
  }
  lut[max] = max;
  applyTable(ip, lut);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void stretchHistogram(cv::Mat &ip, double saturated, cv::Mat &histogram, bool doNormalize)
{
  auto [hmin, hmax] = getMinAndMax(ip, saturated, histogram);
  if(hmax > hmin) {
    if(doNormalize) {
      normalize(ip, hmin, hmax);
    } else {
      setMinMax(ip, hmin, hmax);
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void EnhanceContrast::execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList & /*resultIn*/)
{
  //
  // Compute the histogram
  //
  int histSize           = UINT16_MAX + 1;
  float range[]          = {0, UINT16_MAX + 1};
  const float *histRange = {range};
  bool uniform           = true;
  bool accumulate        = false;
  cv::Mat hist;
  cv::calcHist(&image, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);    //, uniform, accumulate);
  //// Normalize the histogram to [0, histImage.height()]
  // hist.at<float>(0)    = 0;    // We don't want to display black
  // double globalMaximum = 0;
  // double globalMinimum = 0;
  // cv::minMaxLoc(hist, &globalMinimum, &globalMaximum);

  //
  // Execute
  //
  if(mSettings.equalizeHistogram) {
    equalize(image, hist);
  } else {
    stretchHistogram(image, mSettings.saturatedPixels, hist, mSettings.normalize);
  }
  if(mSettings.normalize) {
    double max = 0;
    double min = 0;
    cv::minMaxLoc(image, &min, &max);
    setMinMax(image, 0, static_cast<uint16_t>(max));
  }
}

}    // namespace joda::cmd
