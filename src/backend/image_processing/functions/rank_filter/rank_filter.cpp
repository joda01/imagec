///
/// \file      rank_filter.cpp
/// \author    Joachim Danmayr
/// \date      2023-02-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     C++ implementation of rank filter based on
///            ImageJ rank filter suggestion by M. Schmid
///            https://github.com/imagej/ImageJ/blob/master/ij/plugin/filter/RankFilters.java
///

#include "rank_filter.hpp"
#include <opencv2/core/hal/interface.h>
#include <cstddef>
#include <opencv2/core/types.hpp>

namespace joda::func::img {

/** OPEN, CLOSE, TOPHAT need more than one run of the underlying filter */

bool RankFilter::isMultiStepFilter(int filterType)
{
  return filterType >= OPEN && filterType <= TOP_HAT;
}

/** Filters an image by any method except 'despecle', 'remove outliers', or top-hat
 *	@param ip	   The ImageProcessor that should be filtered (all 4 types supported)
 *	@param radius  Determines the kernel size, see Process>Filters>Show Circular Masks.
 *				   Must not be negative. No checking is done for large values that would
 *				   lead to excessive computing times.
 *	@param filterType May be MEAN, MIN, MAX, VARIANCE, or MEDIAN.
 */

void RankFilter::rank(cv::Mat &ip, double radius, int filterType)
{
  rank(ip, radius, filterType, 0, 50.0F);
}

/** Filters an image by any method except 'despecle' and top-hat (for 'despeckle', use 'median' and radius=1)
 * @param ip The image subject to filtering
 * @param radius The kernel radius
 * @param filterType as defined above; DESPECKLE is not a valid type here; use median and
 *		  a radius of 1.0 instead
 * @param whichOutliers BRIGHT_OUTLIERS or DARK_OUTLIERS for 'outliers' filter
 * @param threshold Threshold for 'outliers' filter
 */

void RankFilter::rank(cv::Mat &ip, double radius, int filterType, int whichOutliers, float threshold)
{
  rank(ip, radius, filterType, whichOutliers, threshold, false, false);
}

/** Filters an image by any method except 'despecle' (for 'despeckle', use 'median' and radius=1)
 * @param ip The image subject to filtering
 * @param radius The kernel radius
 * @param filterType as defined above; DESPECKLE is not a valid type here; use median and
 *		  a radius of 1.0 instead
 * @param whichOutliers BRIGHT_OUTLIERS or DARK_OUTLIERS for 'outliers' filter
 * @param threshold Threshold for 'outliers' filter
 * @param lightBackground for top-hat background subtraction, background is light, not dark
 * @param dontSubtract fpr top-hat filter, performs a grayscale open or close instead of top-hat,
 *        where the result of grayscale open/close is subtracted from the original.
 */

void RankFilter::rank(cv::Mat &ip, double radius, int filterType, int whichOutliers, float threshold,
                      bool lightBackground, bool dontSubtract)
{
  auto lineRadii = makeLineRadii(radius);

  bool snapshotRequired = (filterType == TOP_HAT && !dontSubtract);
  cv::Mat snapshot      = ip.clone();
  // if(snapshotRequired && ip.getSnapshotPixels() == null) {
  //   ip.snapshot();
  // }
  bool isInvertedLut       = false;
  float minMaxOutliersSign = filterType == MIN || filterType == OPEN ? -1.0f : 1.0f;    // open is minimum first
  if(filterType == OUTLIERS) {    // sign is -1 for high outliers: compare number with minimum
    minMaxOutliersSign = (isInvertedLut == (whichOutliers == DARK_OUTLIERS)) ? -1.0f : 1.0f;
  }

  if(filterType == TOP_HAT) {
    bool invertedLut   = isInvertedLut;
    bool invert        = (invertedLut && !lightBackground) || (!invertedLut && lightBackground);
    minMaxOutliersSign = invert ? 1.0f : -1.0f;
  }

  cv::Mat snapIp;

  bool isImagePart = false;
  int nextY        = 0;    // becomes negative when interrupted during preview or ESC pressed
  if(filterType == TOP_HAT && !dontSubtract) {
    snapIp = snapshot.clone();
  }
  int filterType1 = filterType;
  if(isMultiStepFilter(filterType)) {    // open, close, top-hat
    filterType1 = (minMaxOutliersSign == -1.0f) ? MIN : MAX;
  }
  doFiltering(ip, lineRadii, filterType1, minMaxOutliersSign, threshold, ch, nextY);
  if(isMultiStepFilter(filterType)) {    // open, close, top-hat
    if(nextY < 0) {
      return;
    }
    int filterType2 = (minMaxOutliersSign == -1.0f) ? MAX : MIN;
    doFiltering(ip, lineRadii, filterType2, -minMaxOutliersSign, threshold, ch, nextY);
  }
  if(nextY < 0) {
    return;
  }

  if(filterType == TOP_HAT && !dontSubtract) {    // top-hat filter: Subtract opened from input
    float offset = 0;
    if(minMaxOutliersSign == 1.0f) {
      double minVal, maxVal;
      cv::Point minLoc, maxLoc;
      cv::minMaxLoc(ip, &minVal, &maxVal, &minLoc, &maxLoc);
      offset = maxVal;    // background is maximum of possible range (255 or 65535)
    }
    cv::Mat fp;        // float processor
    cv::Mat snapFp;    // float processor

    ip.convertTo(fp, CV_32FC1);
    snapIp.convertTo(snapFp, CV_32FC1);
    // float[] pixels     = (float[]) fp.getPixels();
    // float[] snapPixels = (float[]) snapFp.getPixels();
    for(int y = 0; y < ip.rows; y++) {
      for(int ix = 0, p = y * ip.cols; ix < ip.cols; ix++, p++) {
        fp.at<float>(p) = snapFp.at<float>(p) - fp.at<float>(p) + offset;
      }
    }
    ip = fp;
  }
}

// Filter a grayscale image or one channel of an RGB image with several threads
// Implementation: each thread uses the same input buffer (cache), always works on the next unfiltered line
// Usually, one thread reads reads several lines into the cache, while the others are processing the data.
// 'nextY.get()' is set to a large negative number if the main thread has been interrupted (during preview) or ESC
// pressed. 'nextY' must not be a class variable because it is also used with one thread (with stack parallelization)

void RankFilter::doFiltering(cv::Mat &ip, std::shared_ptr<int> lineRadii, int filterType, float minMaxOutliersSign,
                             float threshold, int colorChannel, int &nextY)
{
  int width = ip.cols;
  // Object pixels  = ip.getPixels();

  int kHeight     = this->kHeight(lineRadii.get());
  int kRadius     = this->kRadius(lineRadii.get());
  int cacheWidth  = width + 2 * kRadius;
  int cacheHeight = kHeight;
  // 'cache' is the input buffer. Each line y in the image is mapped onto cache line y%cacheHeight
  float *cache = new float[cacheWidth * cacheHeight];
  highestYinCache.set(std::max(0 - kHeight / 2, 0) - 1);    // this line+1 will be read into the cache first

  nextY = 0;    // first thread started should begin at roi.y
  doFiltering(ip, lineRadii, cache, cacheWidth * cacheHeight, cacheWidth, cacheHeight, filterType, minMaxOutliersSign,
              threshold, colorChannel);
}

// Filter a grayscale image or one channel of an RGB image using one thread
//
// Synchronization: unless a thread is waiting, we avoid the overhead of 'synchronized'
// statements. That's because a thread waiting for another one should be rare.
//
// Data handling: The area needed for processing a line is written into the array 'cache'.
// This is a stripe of sufficient width for all threads to have each thread processing one
// line, and some extra space if one thread is finished to start the next line.
// This array is padded at the edges of the image so that a surrounding with radius kRadius
// for each pixel processed is within 'cache'. Out-of-image
// pixels are set to the value of the nearest edge pixel. When adding a new line, the lines in
// 'cache' are not shifted but rather the smaller array with the start and end pointers of the
// kernel area is modified to point at the addresses for the next line.
//
// Algorithm: For mean and variance, except for very small radius, usually do not calculate the
// sum over all pixels. This sum is calculated for the first pixel of every line only. For the
// following pixels, add the new values and subtract those that are not in the sum any more.
// For min/max, also first look at the new values, use their maximum if larger than the old
// one. The look at the values not in the area any more; if it does not contain the old
// maximum, leave the maximum unchanged. Otherwise, determine the maximum inside the area.
// For outliers, calculate the median only if the pixel deviates by more than the threshold
// from any pixel in the area. Therfore min or max is calculated; this is a much faster
// operation than the median.

void RankFilter::doFiltering(cv::Mat &ip, std::shared_ptr<int> lineRadii, float *cache, size_t cacheLength,
                             int cacheWidth, int cacheHeight, int filterType, float minMaxOutliersSign, float threshold,
                             int colorChannel)
{
  int width  = ip.cols;
  int height = ip.rows;

  int kHeight  = this->kHeight(lineRadii.get());
  int kRadius  = this->kRadius(lineRadii.get());
  int kNPoints = this->kNPoints(lineRadii.get());

  int xmin                   = 0 - kRadius;
  int xmax                   = 0 + width + kRadius;
  size_t cachePointersLength = 0;
  auto cachePointers         = makeCachePointers(lineRadii.get(), cacheWidth, cachePointersLength);

  int padLeft     = xmin < 0 ? -xmin : 0;
  int padRight    = xmax > width ? xmax - width : 0;
  int xminInside  = xmin > 0 ? xmin : 0;
  int xmaxInside  = xmax < width ? xmax : width;
  int widthInside = xmaxInside - xminInside;

  bool minOrMax           = filterType == MIN || filterType == MAX;
  bool minOrMaxOrOutliers = minOrMax || filterType == OUTLIERS;
  bool sumFilter          = filterType == MEAN || filterType == VARIANCE;
  bool medianFilter       = filterType == MEDIAN || filterType == OUTLIERS;
  double *sums            = sumFilter ? new double[2] : nullptr;
  float *medianBuf1       = (medianFilter || filterType == REMOVE_NAN) ? new float[kNPoints] : nullptr;
  float *medianBuf2       = (medianFilter || filterType == REMOVE_NAN) ? new float[kNPoints] : nullptr;

  bool smallKernel = kRadius < 2;

  double minVal, maxVal;
  cv::Point minLoc, maxLoc;
  cv::minMaxLoc(ip, &minVal, &maxVal, &minLoc, &maxLoc);

  // Object pixels  = ip.getPixels();
  bool isFloat   = false;
  float maxValue = (float) maxVal;
  float *values  = new float[width];

  int previousY = kHeight / 2 - cacheHeight;
  bool rgb      = false;

  while(true) {
    int y = y + 1;    // y of the next line that needs processing

    bool threadFinished = y >= 0 + height || y < 0;    // y<0 if aborted

    if(threadFinished) {
      return;    // all done, break the loop
    }

    for(int i = 0; i < cachePointersLength; i++) {    // shift kernel pointers to new line
      cachePointers.get()[i] = (cachePointers.get()[i] + cacheWidth * (y - previousY)) % cacheLength;
    }
    previousY = y;

    if(numThreads == 1) {    // R E A D
      int yStartReading = y == 0 ? std::max(0 - kHeight / 2, 0) : y + kHeight / 2;
      for(int yNew = yStartReading; yNew <= y + kHeight / 2; yNew++) {    // only 1 line except at start
        readLineToCacheOrPad(ip, width, height, 0, xminInside, widthInside, cache, cacheWidth, cacheHeight, padLeft,
                             padRight, colorChannel, kHeight, yNew);
      }
    } else {    // if no other thread is copying or if the own thread needs the data
      if(!copyingToCache.get() || highestYinCache.get() < y + kHeight / 2)

        copyingToCache.set(true);    // copy as many new line(s) as possible into the cache
      while(highestYinCache.get() < 0 - kHeight / 2 + cacheHeight - 1) {
        int yNew = highestYinCache.get() + 1;
        readLineToCacheOrPad(ip, width, height, 0, xminInside, widthInside, cache, cacheWidth, cacheHeight, padLeft,
                             padRight, colorChannel, kHeight, yNew);
        highestYinCache.set(yNew);
      }
      copyingToCache.set(false);
    }

    int cacheLineP = cacheWidth * (y % cacheHeight) + kRadius;                       // points to pixel (roi.x, y)
    filterLine(values, width, cache, cachePointers, kNPoints, cacheLineP, roi, y,    // F I L T E R
               sums, medianBuf1, medianBuf2, minMaxOutliersSign, maxValue, isFloat, filterType, smallKernel, sumFilter,
               minOrMax, minOrMaxOrOutliers, threshold);
    if(!isFloat) {    // Float images: data are written already during 'filterLine'
      writeLineToPixels(values, pixels, y * width, width, colorChannel);    // W R I T E
    }
    // IJ.log("thread "+threadNumber+" @y="+y+" line done");
  }    // while (true); loop over y (lines)
}

// returns the minimum of the array, which may be modified concurrently, but not less than 0

void RankFilter::filterLine(float *values, int width, float *cache, int *cachePointers, int kNPoints, int cacheLineP,
                            cv::Rect roi, int y, double *sums, float *medianBuf1, float *medianBuf2,
                            float minMaxOutliersSign, float maxValue, bool isFloat, int filterType, bool smallKernel,
                            bool sumFilter, bool minOrMax, bool minOrMaxOrOutliers, float threshold)
{
  int valuesP          = isFloat ? roi.x + y * width : 0;
  float max            = 0.0f;
  float median         = std::isnan(cache[cacheLineP]) ? 0 : cache[cacheLineP];    // a first guess
  bool fullCalculation = true;
  for(int x = 0; x < roi.width; x++, valuesP++) {    // x is with respect to roi.x
    if(fullCalculation) {
      fullCalculation = smallKernel;    // for small kernel, always use the full area, not incremental algorithm
      if(minOrMaxOrOutliers)
        max = getAreaMax(cache, x, cachePointers, 0, -std::numeric_limits<float>::max(), minMaxOutliersSign);
      if(minOrMax) {
        values[valuesP] = max * minMaxOutliersSign;
        continue;
      } else if(sumFilter)
        getAreaSums(cache, x, cachePointers, sums);
    } else {
      if(minOrMaxOrOutliers) {
        float newPointsMax = getSideMax(cache, x, cachePointers, true, minMaxOutliersSign);
        if(newPointsMax >= max) {    // compare with previous maximum 'max'
          max = newPointsMax;
        } else {
          float removedPointsMax = getSideMax(cache, x, cachePointers, false, minMaxOutliersSign);
          if(removedPointsMax >= max)
            max = getAreaMax(cache, x, cachePointers, 1, newPointsMax, minMaxOutliersSign);
        }
        if(minOrMax) {
          values[valuesP] = max * minMaxOutliersSign;
          continue;
        }
      } else if(sumFilter) {
        addSideSums(cache, x, cachePointers, sums);
        if(std::isnan(sums[0]))    // avoid perpetuating NaNs into remaining line
          fullCalculation = true;
      }
    }
    if(sumFilter) {
      if(filterType == MEAN)
        values[valuesP] = (float) (sums[0] / kNPoints);
      else {    // Variance: sum of squares - square of sums
        float value = (float) ((sums[1] - sums[0] * sums[0] / kNPoints) / kNPoints);
        if(value > maxValue)
          value = maxValue;
        if(value < 0)
          value = 0;    // numeric noise can cause values < 0
        values[valuesP] = value;
      }
    } else if(filterType == MEDIAN) {
      if(isFloat) {
        median =
            std::isnan(values[valuesP]) ? std::numeric_limits<float>::quiet_NaN() : values[valuesP];    // a first guess
        median = getNaNAwareMedian(cache, x, cachePointers, medianBuf1, medianBuf2, kNPoints, median);
      } else
        median = getMedian(cache, x, cachePointers, medianBuf1, medianBuf2, kNPoints, median);
      values[valuesP] = median;
    } else if(filterType == OUTLIERS) {
      float v = cache[cacheLineP + x];
      if(v * minMaxOutliersSign + threshold <
         max) {    // for low outliers: median can't be higher than max (minMaxOutliersSign is +1)
        median = getMedian(cache, x, cachePointers, medianBuf1, medianBuf2, kNPoints, median);
        if(v * minMaxOutliersSign + threshold < median * minMaxOutliersSign)
          v = median;    // beyond threshold (below if minMaxOutliersSign=+1), replace outlier by median
      }
      values[valuesP] = v;
    } else if(filterType == REMOVE_NAN) {    // float only; then 'values' is pixels array
      if(std::isnan(values[valuesP]))
        values[valuesP] = getNaNAwareMedian(cache, x, cachePointers, medianBuf1, medianBuf2, kNPoints, median);
      else
        median = values[valuesP];    // initial guess for the next point
    }
  }    // for x
}

/** Read a line into the cache (including padding in x).
 *	If y>=height, instead of reading new data, it duplicates the line y=height-1.
 *	If y==0, it also creates the data for y<0, as far as necessary, thus filling the cache with
 *	more than one line (padding by duplicating the y=0 row).
 */

void RankFilter::readLineToCacheOrPad(cv::Mat &pixels, int width, int height, int roiY, int xminInside, int widthInside,
                                      float *cache, int cacheWidth, int cacheHeight, int padLeft, int padRight,
                                      int colorChannel, int kHeight, int y)
{
  int lineInCache = y % cacheHeight;
  if(y < height) {
    readLineToCache(pixels, y * width, xminInside, widthInside, cache, lineInCache * cacheWidth, padLeft, padRight,
                    colorChannel);
    if(y == 0)
      for(int prevY = roiY - kHeight / 2; prevY < 0; prevY++) {    // for y<0, pad with y=0 border pixels
        int prevLineInCache = cacheHeight + prevY;
        System.arraycopy(cache, 0, cache, prevLineInCache * cacheWidth, cacheWidth);
      }
  } else
    System.arraycopy(cache, cacheWidth * ((height - 1) % cacheHeight), cache, lineInCache * cacheWidth, cacheWidth);
}

/** Read a line into the cache (includes conversion to flaot). Pad with edge pixels in x if necessary */

static void readLineToCache(Object pixels, int pixelLineP, int xminInside, int widthInside, float[] cache,
                            int cacheLineP, int padLeft, int padRight, int colorChannel)
{
  if(pixels instanceof byte[]) {
    byte[] bPixels = (byte[]) pixels;
    for(int pp = pixelLineP + xminInside, cp = cacheLineP + padLeft; pp < pixelLineP + xminInside + widthInside;
        pp++, cp++)
      cache[cp] = bPixels[pp] & 0xff;
  } else if(pixels instanceof short[]) {
    short[] sPixels = (short[]) pixels;
    for(int pp = pixelLineP + xminInside, cp = cacheLineP + padLeft; pp < pixelLineP + xminInside + widthInside;
        pp++, cp++)
      cache[cp] = sPixels[pp] & 0xffff;
  } else if(pixels instanceof float[]) {
    System.arraycopy(pixels, pixelLineP + xminInside, cache, cacheLineP + padLeft, widthInside);
  } else {    // RGB
    int[] cPixels = (int[]) pixels;
    int shift     = 16 - 8 * colorChannel;
    int byteMask  = 255 << shift;
    for(int pp = pixelLineP + xminInside, cp = cacheLineP + padLeft; pp < pixelLineP + xminInside + widthInside;
        pp++, cp++)
      cache[cp] = (cPixels[pp] & byteMask) >> shift;
  }
  for(int cp = cacheLineP; cp < cacheLineP + padLeft; cp++)
    cache[cp] = cache[cacheLineP + padLeft];
  for(int cp = cacheLineP + padLeft + widthInside; cp < cacheLineP + padLeft + widthInside + padRight; cp++)
    cache[cp] = cache[cacheLineP + padLeft + widthInside - 1];
}

/** Write a line to pixels arrax, converting from float (not for float data!)
 *	No checking for overflow/underflow
 */

static void writeLineToPixels(float[] values, Object pixels, int pixelP, int length, int colorChannel)
{
  if(pixels instanceof byte[]) {
    byte[] bPixels = (byte[]) pixels;
    for(int i = 0, p = pixelP; i < length; i++, p++)
      bPixels[p] = (byte) (((int) (values[i] + 0.5f)) & 0xff);
  } else if(pixels instanceof short[]) {
    short[] sPixels = (short[]) pixels;
    for(int i = 0, p = pixelP; i < length; i++, p++)
      sPixels[p] = (short) (((int) (values[i] + 0.5f)) & 0xffff);
  } else {    // RGB
    int[] cPixels = (int[]) pixels;
    int shift     = 16 - 8 * colorChannel;
    int resetMask = 0xffffffff ^ (0xff << shift);
    for(int i = 0, p = pixelP; i < length; i++, p++)
      cPixels[p] = (cPixels[p] & resetMask) | (((int) (values[i] + 0.5f)) << shift);
  }
}

/** Get max (or -min if sign=-1) within the kernel area.
 *	@param x between 0 and cacheWidth-1
 *	@param ignoreRight should be 0 for analyzing all data or 1 for leaving out the row at the right
 *	@param max should be -Float.MAX_VALUE or the smallest value the maximum can be */

static float getAreaMax(float[] cache, int xCache0, int[] kernel, int ignoreRight, float max, float sign)
{
  for(int kk = 0; kk < kernel.length; kk++) {    // y within the cache stripe (we have 2 kernel pointers per cache line)
    for(int p = kernel[kk++] + xCache0; p <= kernel[kk] + xCache0 - ignoreRight; p++) {
      float v = cache[p] * sign;
      if(max < v)
        max = v;
    }
  }
  return max;
}

/** Get max (or -min if sign=-1) at the right border inside or left border outside the kernel area.
 *	x between 0 and cacheWidth-1 */

static float getSideMax(float[] cache, int xCache0, int[] kernel, bool isRight, float sign)
{
  float max = -Float.MAX_VALUE;
  if(!isRight)
    xCache0--;
  for(int kk = isRight ? 1 : 0; kk < kernel.length;
      kk += 2) {    // y within the cache stripe (we have 2 kernel pointers per cache line)
    float v = cache[xCache0 + kernel[kk]] * sign;
    if(max < v)
      max = v;
  }
  return max;
}

/** Get sum of values and values squared within the kernel area.
 *	x between 0 and cacheWidth-1
 *	Output is written to array sums[0] = sum; sums[1] = sum of squares */

static void getAreaSums(float[] cache, int xCache0, int[] kernel, double[] sums)
{
  double sum = 0, sum2 = 0;
  for(int kk = 0; kk < kernel.length; kk++) {    // y within the cache stripe (we have 2 kernel pointers per cache line)
    for(int p = kernel[kk++] + xCache0; p <= kernel[kk] + xCache0; p++) {
      double v = cache[p];
      sum += v;
      sum2 += v * v;
    }
  }
  sums[0] = sum;
  sums[1] = sum2;
  return;
}

/** Add all values and values squared at the right border inside minus at the left border outside the kernal area.
 *	Output is added or subtracted to/from array sums[0] += sum; sums[1] += sum of squares  when at
 *	the right border, minus when at the left border */

static void addSideSums(float[] cache, int xCache0, int[] kernel, double[] sums)
{
  double sum = 0, sum2 = 0;
  for(int kk = 0; kk < kernel.length; /*k++;k++ below*/) {
    double v = cache[kernel[kk++] + (xCache0 - 1)];    // this value is not in the kernel area any more
    sum -= v;
    sum2 -= v * v;
    v = cache[kernel[kk++] + xCache0];    // this value comes into the kernel area
    sum += v;
    sum2 += v * v;
  }
  sums[0] += sum;
  sums[1] += sum2;
  return;
}

/** Get median of values within kernel-sized neighborhood. Kernel size kNPoints should be odd.
 */

static float getMedian(float[] cache, int xCache0, int[] kernel, float[] aboveBuf, float[] belowBuf, int kNPoints,
                       float guess)
{
  int nAbove = 0, nBelow = 0;
  for(int kk = 0; kk < kernel.length; kk++) {
    for(int p = kernel[kk++] + xCache0; p <= kernel[kk] + xCache0; p++) {
      float v = cache[p];
      if(v > guess) {
        aboveBuf[nAbove] = v;
        nAbove++;
      } else if(v < guess) {
        belowBuf[nBelow] = v;
        nBelow++;
      }
    }
  }
  int half = kNPoints / 2;
  if(nAbove > half)
    return findNthLowestNumber(aboveBuf, nAbove, nAbove - half - 1);
  else if(nBelow > half)
    return findNthLowestNumber(belowBuf, nBelow, half);
  else
    return guess;
}

/** Get median of values within kernel-sized neighborhood.
 *	NaN data values are ignored; the output is NaN only if there are only NaN values in the
 *	kernel-sized neighborhood */

static float getNaNAwareMedian(float[] cache, int xCache0, int[] kernel, float[] aboveBuf, float[] belowBuf,
                               int kNPoints, float guess)
{
  int nAbove = 0, nBelow = 0;
  for(int kk = 0; kk < kernel.length; kk++) {
    for(int p = kernel[kk++] + xCache0; p <= kernel[kk] + xCache0; p++) {
      float v = cache[p];
      if(Float.isNaN(v)) {
        kNPoints--;
      } else if(v > guess) {
        aboveBuf[nAbove] = v;
        nAbove++;
      } else if(v < guess) {
        belowBuf[nBelow] = v;
        nBelow++;
      }
    }
  }
  if(kNPoints == 0)
    return Float.NaN;    // only NaN data in the neighborhood?
  int half = kNPoints / 2;
  if(nAbove > half)
    return findNthLowestNumber(aboveBuf, nAbove, nAbove - half - 1);
  else if(nBelow > half)
    return findNthLowestNumber(belowBuf, nBelow, half);
  else
    return guess;
}

/** Find the n-th lowest number in part of an array
 *	@param buf The input array. Only values 0 ... bufLength are read. <code>buf</code> will be modified.
 *	@param bufLength Number of values in <code>buf</code> that should be read
 *	@param n which value should be found; n=0 for the lowest, n=bufLength-1 for the highest
 *	@return the value */

static float findNthLowestNumber(float[] buf, int bufLength, int n)
{
  // Hoare's find, algorithm, based on http://www.geocities.com/zabrodskyvlada/3alg.html
  // Contributed by Heinz Klar
  int i, j;
  int l     = 0;
  int m     = bufLength - 1;
  float med = buf[n];
  float dum;

  while(l < m) {
    i = l;
    j = m;
    do {
      while(buf[i] < med)
        i++;
      while(med < buf[j])
        j--;
      dum    = buf[j];
      buf[j] = buf[i];
      buf[i] = dum;
      i++;
      j--;
    } while((j >= n) && (i <= n));
    if(j < n)
      l = i;
    if(n < i)
      m = j;
    med = buf[n];
  }
  return med;
}

/** Reset region between inner rectangle 'roi' and outer rectangle 'roi1' to the snapshot */

void resetRoiBoundary(ImageProcessor ip, Rectangle roi, Rectangle roi1)
{
  int width       = ip.getWidth();
  Object pixels   = ip.getPixels();
  Object snapshot = ip.getSnapshotPixels();
  for(int y = roi1.y, p = roi1.x + y * width; y < roi.y; y++, p += width)
    System.arraycopy(snapshot, p, pixels, p, roi1.width);
  int leftWidth  = roi.x - roi1.x;
  int rightWidth = roi1.x + roi1.width - (roi.x + roi.width);
  for(int y = roi.y, pL = roi1.x + y * width, pR = roi.x + roi.width + y * width; y < roi.y + roi.height;
      y++, pL += width, pR += width) {
    if(leftWidth > 0)
      System.arraycopy(snapshot, pL, pixels, pL, leftWidth);
    if(rightWidth > 0)
      System.arraycopy(snapshot, pR, pixels, pR, rightWidth);
  }
  for(int y = roi.y + roi.height, p = roi1.x + y * width; y < roi1.y + roi1.height; y++, p += width)
    System.arraycopy(snapshot, p, pixels, p, roi1.width);
}

/** @deprecated
 * Not needed any more, use the rank(ip, ...) method, which creates the kernel */

void makeKernel(double radius)
{
  this.radius = radius;
}

/** Sets the display range after variance and top-hat (16-bit & floating-point images only) */

void setDisplayRange(ImageProcessor ip)
{
  if((ip instanceof ByteProcessor) || (ip instanceof ColorProcessor))
    return;
  new ContrastEnhancer().stretchHistogram(ip, 0.5);
}

/** Create a circular kernel (structuring element) of a given radius.
 *	@param radius
 *	Radius = 0.5 includes the 4 neighbors of the pixel in the center,
 *	radius = 1 corresponds to a 3x3 kernel size.
 *	@return the circular kernel
 *	The output is an array that gives the length of each line of the structuring element
 *	(kernel) to the left (negative) and to the right (positive):
 *	[0] left in line 0, [1] right in line 0,
 *	[2] left in line 2, ...
 *	The maximum (absolute) value should be kernelRadius.
 *	Array elements at the end:
 *	length-2: nPoints, number of pixels in the kernel area
 *	length-1: kernelRadius in x direction (kernel width is 2*kernelRadius+1)
 *	Kernel height can be calculated as (array length - 1)/2 (odd number);
 *	Kernel radius in y direction is kernel height/2 (truncating integer division).
 *	Note that kernel width and height are the same for the circular kernels used here,
 *	but treated separately for the case of future extensions with non-circular kernels.
 */

std::shared_ptr<int> RankFilter::makeLineRadii(double radius)
{
  if(radius >= 1.5 && radius < 1.75) {    // this code creates the same sizes as the previous RankFilters
    radius = 1.75;
  } else if(radius >= 2.5 && radius < 2.85) {
    radius = 2.85;
  }
  int r2      = (int) (radius * radius) + 1;
  int kRadius = (int) (Math.sqrt(r2 + 1e-10));
  int kHeight = 2 * kRadius + 1;
  std::shared_ptr<int> kernel(new int[2 * kHeight + 2]{0}, [](int *p) { delete[] p; });
  kernel[2 * kRadius]     = -kRadius;
  kernel[2 * kRadius + 1] = kRadius;
  int nPoints             = 2 * kRadius + 1;
  for(int y = 1; y <= kRadius; y++) {    // lines above and below center together
    int dx                        = (int) (Math.sqrt(r2 - y * y + 1e-10));
    kernel[2 * (kRadius - y)]     = -dx;
    kernel[2 * (kRadius - y) + 1] = dx;
    kernel[2 * (kRadius + y)]     = -dx;
    kernel[2 * (kRadius + y) + 1] = dx;
    nPoints += 4 * dx + 2;    // 2*dx+1 for each line, above&below
  }
  kernel[kernel.length - 2] = nPoints;
  kernel[kernel.length - 1] = kRadius;
  // for (int i=0; i<kHeight;i++)IJ.log(i+": "+kernel[2*i]+"-"+kernel[2*i+1]);
  return kernel;
}

// kernel height

int RankFilter::kHeight(int *lineRadii)
{
  return (lineRadii.length - 2) / 2;
}

// kernel radius in x direction. width is 2+kRadius+1

int RankFilter::kRadius(int *lineRadii)
{
  return lineRadii[lineRadii.length - 1];
}

// number of points in kernal area

int RankFilter::kNPoints(int *lineRadii)
{
  return lineRadii[lineRadii.length - 2];
}

// cache pointers for a given kernel

std::shared_ptr<int> RankFilter::makeCachePointers(int *lineRadii, int cacheWidth, size_t &length)
{
  int kRadius = kRadius(lineRadii);
  int kHeight = kHeight(lineRadii);
  std::shared_ptr<int> cachePointers(new int[2 * kHeight]{0}, [](int *p) { delete[] p; });
  length = 2 * kHeight;

  for(int i = 0; i < kHeight; i++) {
    cachePointers[2 * i]     = i * cacheWidth + kRadius + lineRadii[2 * i];
    cachePointers[2 * i + 1] = i * cacheWidth + kRadius + lineRadii[2 * i + 1];
  }
  return cachePointers;
}

void showMasks()
{
  int w = 150, h = 150;
  ImageStack stack = new ImageStack(w, h);
  // for (double r=0.1; r<3; r+=0.01) {
  for(double r = 0.5; r < 50; r += 0.5) {
    ImageProcessor ip = new FloatProcessor(w, h, new int[w * h]);
    float[] pixels    = (float[]) ip.getPixels();
    int[] lineRadii   = makeLineRadii(r);
    int kHeight       = kHeight(lineRadii);
    int kRadius       = kRadius(lineRadii);
    int y0            = h / 2 - kHeight / 2;
    for(int i = 0, y = y0; i < kHeight; i++, y++)
      for(int x = w / 2 + lineRadii[2 * i], p = x + y * w; x <= w / 2 + lineRadii[2 * i + 1]; x++, p++)
        pixels[p] = 1f;
    stack.addSlice("radius=" + r + ", size=" + (2 * kRadius + 1), ip);
  }
  new ImagePlus("Masks", stack).show();
}

/** This method is called by ImageJ to set the number of calls to run(ip)
 *	corresponding to 100% of the progress bar.
 *  Setting nPasses=0 suppresses the progress bar */

void setNPasses(int nPasses)
{
  this.nPasses = nPasses;
  pass         = 0;
}

void showProgress(double percent, bool rgb)
{
  if(nPasses == 0)
    return;
  int nPasses2 = rgb ? nPasses * 3 : nPasses;
  percent      = (double) pass / nPasses2 + percent / nPasses2;
  IJ.showProgress(percent);
}
}    // namespace joda::func::img
