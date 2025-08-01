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
  auto min2    = static_cast<float>(min);
  auto max2    = static_cast<float>(max);
  int maxValue = 65535;
  double scale = 65535.0 / (max2 - min2 + 1);
  int32_t value;

  for(int y = 0; y < image.rows; ++y) {
    for(int x = 0; x < image.cols; ++x) {
      value = (image.at<uint16_t>(y, x) & 0xffff) - min2;
      if(value < 0) {
        value = 0;
      }
      value = static_cast<int32_t>(static_cast<float>(value) * scale + 0.5);
      if(value > maxValue) {
        value = maxValue;
      }
      image.at<uint16_t>(y, x) = static_cast<uint16_t>(value);
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
  for(int y = 0; y < image.rows; y++) {
    lineStart = y * image.cols;
    lineEnd   = lineStart + image.rows;
    for(int i = lineEnd; --i >= lineStart;) {
      auto v                = lut[image.at<uint16_t>(i) & 0xffff];
      image.at<uint16_t>(i) = static_cast<uint16_t>(v);
    }
  }
  findMinAndMax(image);
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
double getWeightedValue(cv::Mat &histogram, int i)
{
  bool classicEqualization = false;
  float h                  = histogram.at<float>(i);
  if(h < 2 || classicEqualization) {
    return static_cast<double>(h);
  }
  return std::sqrt(static_cast<double>(h));
}

///
/// \brief         Changes the tone curves of images.
///                It should bring up the detail in the flat regions of your image.
///                Histogram Equalization can enhance meaningless detail and hide
///                important but small high-contrast features. This method uses a
///                similar algorithm, but uses the square root of the histogram
///                values, so its effects are less extreme. Hold the alt key down
///                to use the standard histogram equalization algorithm.
///                This code was contributed by Richard Kirk (rak@cre.canon.co.uk).
/// \author        Richard Kirk
/// \author        Ported to C++ by Joachim Danmayr
///
auto EnhanceContrast::equalize(cv::Mat &histogram) -> std::array<int32_t, UINT16_MAX + 1>
{
  static constexpr uint16_t max   = UINT16_MAX;
  static constexpr uint16_t range = UINT16_MAX;

  double scale = 0;
  {
    double sum = getWeightedValue(histogram, 0);
    for(int i = 1; i < max; i++) {
      sum += 2 * getWeightedValue(histogram, i);
    }
    sum += getWeightedValue(histogram, max);
    scale = range / sum;
  }
  {
    double sum = getWeightedValue(histogram, 0);
    std::array<int32_t, range + 1> lut;
    lut[0] = 0;
    for(int i = 1; i < max; i++) {
      double delta = getWeightedValue(histogram, i);
      sum += delta;
      lut[i] = static_cast<int>(std::round(sum * scale));
      sum += delta;
    }
    lut[max] = max;
    return lut;
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
std::pair<int, int> EnhanceContrast::findContrastStretchBounds(const cv::Mat &hist, double percentage)
{
  CV_Assert(hist.type() == CV_32F || hist.type() == CV_64F);
  int histSize = hist.rows * hist.cols;    // Usually 65536 for 16-bit

  // Normalize histogram to sum to 1
  cv::Mat histNorm;
  hist.convertTo(histNorm, CV_64F);
  double total = cv::sum(histNorm)[0];
  histNorm /= total;

  // Compute cumulative distribution function (CDF)
  std::vector<double> cdf(histSize, 0.0);
  cdf[0] = histNorm.at<double>(0);
  for(int i = 1; i < histSize; i++) {
    cdf[i] = cdf[i - 1] + histNorm.at<double>(i);
  }

  double lower_thresh = percentage;
  double upper_thresh = 1.0 - percentage;

  int low  = 0;
  int high = histSize - 1;

  // Find lower bound
  for(int i = 0; i < histSize; ++i) {
    if(cdf[i] >= lower_thresh) {
      low = i;
      break;
    }
  }

  // Find upper bound
  for(int i = histSize - 1; i >= 0; --i) {
    if(cdf[i] <= upper_thresh) {
      high = i;
      break;
    }
  }

  return {low, high};
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

  //
  // Execute
  //
  if(mSettings.equalizeHistogram) {
    applyTable(image, equalize(hist));
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
