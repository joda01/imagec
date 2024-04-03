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
#include <vector>
#include "../../functions/function.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::func::img {

class RankFilter
{
public:
  /////////////////////////////////////////////////////
  RankFilter() = default;
  void rank(cv::Mat &ip, double radius, int filterType);
  void rank(cv::Mat &ip, double radius, int filterType, int whichOutliers, float threshold);
  void rank(cv::Mat &ip, double radius, int filterType, int whichOutliers, float threshold, bool lightBackground,
            bool dontSubtract);

private:
  /////////////////////////////////////////////////////
  void filterLine(float *values, int width, float *cache, int *cachePointers, int kNPoints, int cacheLineP,
                  cv::Rect roi, int y, double *sums, float *medianBuf1, float *medianBuf2, float minMaxOutliersSign,
                  float maxValue, bool isFloat, int filterType, bool smallKernel, bool sumFilter, bool minOrMax,
                  bool minOrMaxOrOutliers, float threshold);

  void readLineToCacheOrPad(cv::Mat &pixels, int width, int height, int roiY, int xminInside, int widthInside,
                            float *cache, int cacheWidth, int cacheHeight, int padLeft, int padRight, int colorChannel,
                            int kHeight, int y);

  void doFiltering(cv::Mat &ip, std::shared_ptr<int> lineRadii, int filterType, float minMaxOutliersSign,
                   float threshold, int colorChannel, int &nextY);
  bool isMultiStepFilter(int filterType);
  std::shared_ptr<int> makeLineRadii(double radius);
  int kRadius(int *lineRadii);
  void doFiltering(cv::Mat &ip, std::shared_ptr<int> lineRadii, float *cache, size_t cacheLength, int cacheWidth,
                   int cacheHeight, int filterType, float minMaxOutliersSign, float threshold, int colorChannel);

  int kHeight(int *lineRadii);
  int kNPoints(int *lineRadii);
  std::shared_ptr<int> makeCachePointers(int *lineRadii, int cacheWidth, size_t &length);
  /////////////////////////////////////////////////////
  static int MEAN = 0, MIN = 1, MAX = 2, VARIANCE = 3, MEDIAN = 4, OUTLIERS = 5, DESPECKLE = 6, REMOVE_NAN = 7,
             OPEN = 8, CLOSE = 9, TOP_HAT = 10;    // when adding a new filter, set HIGHEST_FILTER below.

  static int BRIGHT_OUTLIERS = 0, DARK_OUTLIERS = 1;

  static String[] outlierStrings = {"Bright", "Dark"};

  static int HIGHEST_FILTER = TOP_HAT;
  // Filter parameters

  int filterType;

  double radius;

  double threshold;    // this and the next for 'remove outliers' only

  int whichOutliers;

  bool lightBackground = Prefs.get("bs.background", true);    // this and the next for top hat only

  bool dontSubtract;
  // Remember filter parameters for the next time

  static double[] lastRadius = new double[HIGHEST_FILTER + 1];    // separate for each filter type

  static double lastThreshold = 50.;

  static int lastWhichOutliers = BRIGHT_OUTLIERS;

  static bool lastLightBackground = false;

  static bool lastDontSubtract = false;
  //
  // F u r t h e r   c l a s s   v a r i a b l e s
  int flags = DOES_ALL | SUPPORTS_MASKING | KEEP_PREVIEW;

  ImagePlus imp;

  int nPasses = 1;    // The number of passes (color channels * stack slices)

  PlugInFilterRunner pfr;

  int pass;

  bool previewing = false;
  // M u l t i t h r e a d i n g - r e l a t e d

  int numThreads = Prefs.getThreads();
  // The current state of multithreaded processing is in class variables.
  // Thus, stack parallelization must be done ONLY with one thread for the image
  // (not using these class variables).
  // Atomic objects are used to avoid caching (i.e., to ensure that always the current state of the variable is read).

  AtomicInteger highestYinCache =
      new AtomicInteger(Integer.MIN_VALUE);    // the highest line read into the cache so far

  AtomicInteger nThreadsWaiting = new AtomicInteger(0);    // number of threads waiting until they may read data

  Atomicbool copyingToCache = new Atomicbool(false);    // whether a thread is currently copying data to the cache
};

}    // namespace joda::func::img
