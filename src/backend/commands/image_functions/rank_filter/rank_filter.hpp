///
/// \file      rank_filter.hpp
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

#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::cmd {

class RankFilter
{
public:
  /////////////////////////////////////////////////////
  static constexpr int MEAN = 0, MIN = 1, MAX = 2, VARIANCE = 3, MEDIAN = 4, OUTLIERS = 5, DESPECKLE = 6,
                       REMOVE_NAN = 7, OPEN = 8, CLOSE = 9,
                       TOP_HAT = 10;    // when adding a new filter, set HIGHEST_FILTER below.

  static constexpr int BRIGHT_OUTLIERS = 0, DARK_OUTLIERS = 1;
  static constexpr int HIGHEST_FILTER = TOP_HAT;

  RankFilter() = default;
  void rank(cv::Mat &ip, double radius, int filterType);
  void rank(cv::Mat &ip, double radius, int filterType, int whichOutliers, float threshold);
  void rank(cv::Mat &ip, double radius, int filterType, int whichOutliers, float threshold, bool lightBackground,
            bool dontSubtract);

private:
  /////////////////////////////////////////////////////
  void filterLine(float *values, int width, float *cache, std::shared_ptr<int> cachePoints, size_t cachePointersLength,
                  int kNPoints, int cacheLineP, cv::Mat &roi, int y, double *sums, float *medianBuf1, float *medianBuf2,
                  float minMaxOutliersSign, float maxValue, bool isFloat, int filterType, bool smallKernel,
                  bool sumFilter, bool minOrMax, bool minOrMaxOrOutliers, float threshold);

  void readLineToCacheOrPad(cv::Mat &pixels, int width, int height, int roiY, int xminInside, int widthInside,
                            float *cache, int cacheWidth, int cacheHeight, int padLeft, int padRight, int kHeight,
                            int y);

  void doFiltering(cv::Mat &ip, std::shared_ptr<int> lineRadii, size_t lineRadiiLength, int filterType,
                   float minMaxOutliersSign, float threshold, int &nextY);
  bool isMultiStepFilter(int filterType);
  std::shared_ptr<int> makeLineRadii(double radius, size_t &length);
  int kRadius(int *lineRadii, size_t length);
  void doFiltering(cv::Mat &ip, std::shared_ptr<int> lineRadii, size_t lineRadiiLength, float *cache,
                   size_t cacheLength, int cacheWidth, int cacheHeight, int filterType, float minMaxOutliersSign,
                   float threshold);

  int kHeight(int *lineRadii, size_t length);
  int kNPoints(int *lineRadii, size_t length);
  std::shared_ptr<int> makeCachePointers(int *lineRadii, size_t lineRadiiLength, int cacheWidth, size_t &length);
  static float getAreaMax(float *cache, int xCache0, std::shared_ptr<int> kernel, size_t kernelLength, int ignoreRight,
                          float max, float sign);
  static float getSideMax(float *cache, int xCache0, std::shared_ptr<int> kernel, size_t kernelLength, bool isRight,
                          float sign);
  static void getAreaSums(float *cache, int xCache0, std::shared_ptr<int> kernel, size_t kernelLength, double *sums);
  static void addSideSums(float *cache, int xCache0, std::shared_ptr<int> kernel, size_t kernelLength, double *sums);
  static float getMedian(float *cache, int xCache0, std::shared_ptr<int> kernel, size_t kernelLength, float *aboveBuf,
                         float *belowBuf, int kNPoints, float guess);
  static float getNaNAwareMedian(float *cache, int xCache0, std::shared_ptr<int> kernel, size_t kernelLength,
                                 float *aboveBuf, float *belowBuf, int kNPoints, float guess);
  static void writeLineToPixels(float *values, cv::Mat &pixels, int pixelP, int length);
  static void readLineToCache(cv::Mat &pixels, int pixelLineP, int xminInside, int widthInside, float *cache,
                              int cacheLineP, int padLeft, int padRight);
  static float findNthLowestNumber(float *buf, int bufLength, int n);

  /////////////////////////////////////////////////////
  int highestYinCache = INT32_MIN;
};

}    // namespace joda::cmd
