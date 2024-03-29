

/** This ImageJ plug-in filter finds the maxima (or minima) of an image.
 * It can create a mask where the local maxima of the current image are
 * marked (255; unmarked pixels 0).
 * The plug-in can also create watershed-segmented particles: Assume a
 * landscape of inverted heights, i.e., maxima of the image are now water sinks.
 * For each point in the image, the sink that the water goes to determines which
 * particle it belongs to.
 * When finding maxima (not minima), pixels with a level below the lower threshold
 * can be left unprocessed.
 *
 * Except for segmentation, this plugin works with area ROIs, including non-rectangular ROIs,
 * which define the area where maxima are reported.
 * Since this plug-in creates a separate output image it processes only single images or slices, no stacks.
 *
 * Notes:
 * - When using one instance of MaximumFinder for more than one image in parallel threads,
 *   all must images have the same width and height.
 *
 * version 09-Nov-2006 Michael Schmid
 * version 21-Nov-2006 Wayne Rasband. Adds "Display Point Selection" option and "Count" output type.
 * version 28-May-2007 Michael Schmid. Preview added, bugfix: minima of calibrated images, uses Arrays.sort
 * version 07-Aug-2007 Fixed a bug that could delete particles when doing watershed segmentation of an EDM.
 * version 21-Apr-2007 Adapted for float instead of 16-bit EDM; correct progress bar on multiple calls
 * version 05-May-2009 Works for images>32768 pixels in width or height
 * version 01-Nov-2009 Bugfix: extra lines in segmented output eliminated; watershed is also faster now
 *                     Maximum points encoded in long array for sorting instead of separete objects that need gc
 *                     New output type 'List'
 * version 22-May-2011 Bugfix: Maximum search in EDM and float images with large dynamic range could omit maxima
 * version 13-Sep-2013 added the findMaxima() and findMinima() functions for arrays (Norbert Vischer)
 * version 20-Mar-2014 Watershed segmentation of EDM with tolerance>=1.0 does not kill fine particles
 * version 11-Mar-2019 adds "strict" option, "noise tolerance" renamed to "prominence"
 */

#include "maximum_finder.hpp"
#include <opencv2/core/hal/interface.h>
#include <cstddef>
#include <iostream>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "tools.hpp"

/** Read the parameters (during preview or after showing the dialog) */

//  bool DialogItemChanged

/** Finds the image maxima and returns them as a Polygon, where
 * poly.npoints is the number of maxima. There is an example at<br>
 * http://imagej.nih.gov/ij/macros/js/FindMaxima.js.
 * @param ip             The input image
 * @param tolerance      Height tolerance: maxima are accepted only if protruding more than this value
 *                       from the ridge to a higher maximum
 * @param excludeOnEdges Whether to exclude edge maxima. Also determines whether strict mode is on, i.e.,
 *                       whether the global maximum is accepted even if all other pixel are less than 'tolerance'
 *                       below this level (In 1.52m and before, 'strict' and 'excludeOnEdges' were the same).
 * @return         A Polygon containing the coordinates of the maxima, where poly.npoints
 *                       is the number of maxima. Note that poly.xpoints.length may be greater
 *                       than the number of maxima.
 */

Polygon MaximumFinder::getMaxima(cv::Mat &ip, double tolerance, bool excludeOnEdges)
{
  return getMaxima(ip, tolerance, excludeOnEdges, excludeOnEdges);
}

/** Finds the image maxima and returns them as a Polygon, where poly.npoints is
 * the number of maxima.
 * @param ip             The input image
 * @param tolerance      Height tolerance: maxima are accepted only if protruding more than this value
 *                       from the ridge to a higher maximum
 * @param strict         When off, the global maximum is accepted even if all other pixel are less than
 *                       'tolerance' below this level. With <code>excludeOnEdges=true</code>, 'strict' also
 *                       means that the surounding of a maximum within 'tolerance' must not include an edge pixel
 *                       (otherwise, it is enough that there is no edge pixel with the maximum value).
 * @param excludeOnEdges Whether to exclude edge maxima. Also determines whether strict mode is on, i.e.,
 *                       whether the global maximum is accepted even if all other pixel are less than 'tolerance'
 *                       below this level (In 1.52m and before, 'strict' and 'excludeOnEdges' were the same).
 * @return         A Polygon containing the coordinates of the maxima, where poly.npoints
 *                       is the number of maxima. Note that poly.xpoints.length may be greater
 *                       than the number of maxima.
 */

Polygon MaximumFinder::getMaxima(cv::Mat &ip, double tolerance, bool strict, bool excludeOnEdges)
{
  findMaxima(ip, tolerance, strict, MaximumFinder::NO_THRESHOLD, MaximumFinder::POINT_SELECTION, excludeOnEdges, false);
  return xyCoordinates;
}

/**
 * Calculates peak positions of 1D array N.Vischer, 06-mar-2017
 *
 * @param xx Array containing peaks.
 * @param tolerance Depth of a qualified valley must exceed tolerance.
 * Tolerance must be >= 0. Flat tops are marked at their centers.
 * @param  edgeMode 0=include, 1=exclude, 2=circular
 * edgeMode = 0 (include edges) peak may be separated by one qualified valley and by a border.
 * edgeMode = 1 (exclude edges) peak must be separated by two qualified valleys
 * edgeMode = 2 (circular) array is regarded to be circular
 * @return Positions of peaks, sorted with decreasing amplitude
 */

int *MaximumFinder::findMaxima(double *xx, size_t xxSize, double tolerance, int edgeMode)
{
  int INCLUDE_EDGE = 0;
  int CIRCULAR     = 2;
  int len          = xxSize;
  int origLen      = len;
  if(len < 2)
    return new int[0];
  if(tolerance < 0)
    tolerance = 0;
  if(edgeMode == CIRCULAR) {
    double *cascade3 = new double[len * 3]{0};
    for(int jj = 0; jj < len; jj++) {
      cascade3[jj]           = xx[jj];
      cascade3[jj + len]     = xx[jj];
      cascade3[jj + 2 * len] = xx[jj];
    }
    len *= 3;
    xx = cascade3;
  }
  int *maxPositions = new int[len]{0};
  int jStart        = 0;
  double min = std::numeric_limits<double>::quiet_NaN(), max = std::numeric_limits<double>::quiet_NaN();
  do {    // find first non-NaN value
    max = xx[jStart];
    min = xx[jStart];
    jStart++;
    if(jStart >= xxSize)
      return new int[0];    // only NaNs
  } while(std::isnan(min));
  int maxPos           = jStart - 1;
  int lastMaxPos       = -1;
  bool leftValleyFound = (edgeMode == INCLUDE_EDGE);
  int maxCount         = 0;
  for(int jj = jStart; jj < len; jj++) {
    double val = xx[jj];
    if(val > min + tolerance)
      leftValleyFound = true;
    if(val > max && leftValleyFound) {
      max    = val;
      maxPos = jj;
    }
    if(leftValleyFound)
      lastMaxPos = maxPos;
    if(val < max - tolerance && leftValleyFound) {
      maxPositions[maxCount] = maxPos;
      maxCount++;
      leftValleyFound = false;
      min             = val;
      max             = val;
    }
    if(val < min) {
      min = val;
      if(!leftValleyFound)
        max = val;
    }
  }
  if(edgeMode == INCLUDE_EDGE) {
    if(maxCount > 0 && maxPositions[maxCount - 1] != lastMaxPos)
      maxPositions[maxCount++] = lastMaxPos;
    if(maxCount == 0 && max - min >= tolerance)
      maxPositions[maxCount++] = lastMaxPos;
  }
  int *cropped = new int[maxCount];
  // System.arraycopy(maxPositions, 0, cropped, 0, maxCount);
  std::copy(maxPositions, maxPositions + maxCount, cropped);

  maxPositions      = cropped;
  double *maxValues = new double[maxCount]{0};
  for(int jj = 0; jj < maxCount; jj++) {
    int pos       = maxPositions[jj];
    double midPos = pos;
    while(pos < len - 1 && xx[pos] == xx[pos + 1]) {
      midPos += 0.5;
      pos++;
    }
    maxPositions[jj] = (int) midPos;
    maxValues[jj]    = xx[maxPositions[jj]];
  }
  int *rankPositions = rank(maxValues, maxCount);
  int *returnArr     = new int[maxCount]{0};
  for(int jj = 0; jj < maxCount; jj++) {
    int pos                      = maxPositions[rankPositions[jj]];
    returnArr[maxCount - jj - 1] = pos;    // use descending order
  }
  if(edgeMode == CIRCULAR) {
    int count = 0;
    for(int jj = 0; jj < maxCount; jj++) {
      int pos = returnArr[jj] - origLen;
      if(pos >= 0 && pos < origLen)    // pick maxima from cascade center part
        returnArr[count++] = pos;
    }
    int *returrn2Arr = new int[count]{0};
    // System.arraycopy(returnArr, 0, returrn2Arr, 0, count);
    std::copy(returnArr, returnArr + count, returrn2Arr);

    returnArr = returrn2Arr;
  }
  return returnArr;
}

int *MaximumFinder::findMaxima(double *xx, size_t xxSize, double tolerance, bool excludeOnEdges)
{
  int edgeBehavior = (excludeOnEdges) ? 1 : 0;
  return findMaxima(xx, xxSize, tolerance, edgeBehavior);
}

/**
 * Returns minimum positions of array xx, sorted with decreasing strength
 */

int *MaximumFinder::findMinima(double *xx, size_t xxSize, double tolerance, bool excludeEdges)
{
  int edgeMode = (excludeEdges) ? 1 : 0;
  return findMinima(xx, xxSize, tolerance, edgeMode);
}

int *MaximumFinder::findMinima(double *xx, size_t xxSize, double tolerance, int edgeMode)
{
  int len        = xxSize;
  double *negArr = new double[len]{0};
  for(int jj = 0; jj < len; jj++)
    negArr[jj] = -xx[jj];
  int *minPositions = findMaxima(negArr, len, tolerance, edgeMode);
  return minPositions;
}

/** Find the maxima of an image.
 * @param ip             The input image
 * @param tolerance      Height tolerance: maxima are accepted only if protruding more than this value
 *                       from the ridge to a higher maximum
 * @param outputType     What to mark in output image: SINGLE_POINTS, IN_TOLERANCE or SEGMENTED.
 *                       No output image is created for output types POINT_SELECTION, LIST and COUNT.
 * @param excludeOnEdges Whether to exclude edge maxima. Also determines whether strict mode is on, i.e.,
 *                       whether the global maximum is accepted even if all other pixel are less than 'tolerance'
 *                       below this level (In 1.52m and before, 'strict' and 'excludeOnEdges' were the same).
 * @return               A new byteProcessor with a normal (uninverted) LUT where the marked points
 *                       are set to 255 (Background 0). Pixels outside of the roi of the input ip are not set.
 *                       Returns null if outputType does not require an output or if cancelled by escape
 */

cv::Mat MaximumFinder::findMaxima(cv::Mat &ip, double tolerance, int outputType, bool excludeOnEdges)
{
  return findMaxima(ip, tolerance, MaximumFinder::NO_THRESHOLD, outputType, excludeOnEdges, false);
}

/** Finds the maxima of an image (does not find minima).
 *
 * LIMITATIONS:          With outputType=SEGMENTED (watershed segmentation), some segmentation lines
 *                       may be improperly placed if local maxima are suppressed by the tolerance.
 *
 * @param ip             The input image
 * @param tolerance      Height tolerance: maxima are accepted only if protruding more than this value
 *                       from the ridge to a higher maximum
 * @param threshold      minimum height of a maximum (uncalibrated); for no minimum height set it to
 *                       MaximumFinder::NO_THRESHOLD
 * @param outputType     What to mark in output image: SINGLE_POINTS, IN_TOLERANCE or SEGMENTED.
 *                       No output image is created for output types POINT_SELECTION, LIST and COUNT.
 * @param excludeOnEdges Whether to exclude edge maxima. Also determines whether strict mode is on, i.e.,
 *                       whether the global maximum is accepted even if all other pixel are less than 'tolerance'
 *                       below this level (In 1.52m and before, 'strict' and 'excludeOnEdges' were the same).
 * @param isEDM          Whether the image is a float Euclidian Distance Map.
 * @return               A new byteProcessor with a normal (uninverted) LUT where the marked points
 *                       are set to 255 (Background 0). Pixels outside of the roi of the input ip are not set.
 *                       Returns null if outputType does not require an output or if cancelled by escape
 */

cv::Mat MaximumFinder::findMaxima(cv::Mat &ip, double tolerance, double threshold, int outputType, bool excludeOnEdges,
                                  bool isEDM)
{
  return findMaxima(ip, tolerance, excludeOnEdges, threshold, outputType, excludeOnEdges, isEDM);
}

/** Here the processing is done: Find the maxima of an image (does not find minima).
 *
 * LIMITATIONS:          With outputType=SEGMENTED (watershed segmentation), some segmentation lines
 *                       may be improperly placed if local maxima are suppressed by the tolerance.
 *
 * @param ip             The input image
 * @param tolerance      Height tolerance: maxima are accepted only if protruding more than this value
 *                       from the ridge to a higher maximum
 * @param strict         When off, the global maximum is accepted even if all other pixel are less than
 *                       'tolerance' below this level. With <code>excludeOnEdges=true</code>, 'strict' also
 *                       means that the surounding of a maximum within 'tolerance' must not include an edge pixel
 *                       (otherwise, it is enough that there is no edge pixel with the maximum value).
 * @param threshold      Minimum height of a maximum (uncalibrated); for no minimum height set it to
 *                       MaximumFinder::NO_THRESHOLD
 * @param outputType     What to mark in output image: SINGLE_POINTS, IN_TOLERANCE or SEGMENTED.
 *                       No output image is created for output types POINT_SELECTION, LIST and COUNT.
 * @param excludeOnEdges Whether to exclude edge maxima
 * @param isEDM          Whether the image is a float Euclidian Distance Map.
 * @return               A new byteProcessor with a normal (uninverted) LUT where the marked points
 *                       are set to 255 (Background 0). Pixels outside of the roi of the input ip are not set.
 *                       Returns null if outputType does not require an output or if cancelled by escape
 */

cv::Mat MaximumFinder::findMaxima(cv::Mat &ip, double tolerance, bool strict, double threshold, int outputType,
                                  bool excludeOnEdges, bool isEDM)
{
  if(dirOffset == nullptr) {
    makeDirectionOffsets(ip);
  }
  char *mask = nullptr;    // ip.getMaskArray();
  // if(threshold != MaximumFinder::NO_THRESHOLD && ip.getCalibrationTable() != nullptr && threshold > 0 &&
  //    threshold < ip.getCalibrationTable().length) {
  //   threshold = ip.getCalibrationTable()[(int) threshold];    // convert threshold to calibrated
  // }
  cv::Mat typeP   = cv::Mat::zeros(height, width, CV_8UC1);    // will be a notepad for pixel types
  float globalMin = std::numeric_limits<float>::max();
  float globalMax = -std::numeric_limits<float>::max();
  for(int y = 0; y < ip.rows; y++) {      // find local minimum/maximum now
    for(int x = 0; x < ip.cols; x++) {    // ImageStatistics won't work if we have no ImagePlus
      float v = ip.at<float>(y, x);
      if(globalMin > v) {
        globalMin = v;
      }
      if(globalMax < v) {
        globalMax = v;
      }
    }
  }
  bool maximumPossible = globalMax > globalMin;
  if(strict && globalMax - globalMin <= tolerance) {
    maximumPossible = false;
  }

  if(threshold != MaximumFinder::NO_THRESHOLD) {
    threshold -= (globalMax - globalMin) * 1e-6;    // avoid MaximumFinder::rounding errors
  }
  // for segmentation, exclusion of edge maxima cannot be done now but has to be done after segmentation:
  bool excludeEdgesNow = excludeOnEdges && outputType != SEGMENTED;
  size_t maxPointsSize = 0;
  long *maxPoints      = maximumPossible ? getSortedMaxPoints(ip, typeP, excludeEdgesNow, isEDM, globalMin, globalMax,
                                                              threshold, maxPointsSize)
                                         : new long[0];

  float maxSortingError = 0;
  if(ip.type() == CV_32FC1) {    // sorted sequence may be inaccurate by this value
    std::cout << "Fliut" << std::endl;
    maxSortingError = 1.1f * (isEDM ? SQRT2 / 2.0f : (globalMax - globalMin) / 2e9f);
  }
  analyzeAndMarkMaxima(ip, typeP, maxPoints, maxPointsSize, excludeEdgesNow, isEDM, globalMin, tolerance, strict,
                       outputType, maxSortingError);
  // new ImagePlus("Pixel types",typeP.duplicate()).show();

  cv::imwrite("tmp.jpg", typeP * 50);

  if(outputType == POINT_SELECTION || outputType == LIST || outputType == COUNT)
    return {};

  cv::Mat outIp;
  char *pixels;
  if(outputType == SEGMENTED) {
    // Segmentation required, convert to 8bit (also for 8-bit images, since the calibration
    // may have a negative slope). outIp has background 0, maximum areas 255
    cv::imwrite("outIp0ip.jpg", ip * 20);

    outIp = make8bit(ip, typeP, isEDM, globalMin, globalMax, threshold);
    cv::imwrite("outIp0.jpg", outIp * 20);

    // if (IJ.debugMode) new ImagePlus("pixel types precleanup", typeP.duplicate()).show();
    cleanupMaxima(outIp, typeP, maxPoints,
                  maxPointsSize);    // eliminate all the small maxima (i.e. those outside MAX_AREA)
                                     // if (IJ.debugMode) new ImagePlus("pixel types postcleanup", typeP).show();
                                     // if (IJ.debugMode) new ImagePlus("pre-watershed", outIp.duplicate()).show();

    cv::imwrite("outIp.jpg", outIp * 50);

    if(!watershedSegment(outIp)) {    // do watershed segmentation
      return {};                      // if user-cancelled, return
    }
    cv::imwrite("outIp_wa.jpg", outIp * 50);

    if(!isEDM)
      cleanupExtraLines(outIp);     // eliminate lines due to local minima (none in EDM)
    watershedPostProcess(outIp);    // levels to binary image

    cv::imwrite("outIp_wa2.jpg", outIp * 50);

    if(excludeOnEdges)
      deleteEdgeParticles(outIp, typeP);
  } else {    // outputType other than SEGMENTED
    for(int i = 0; i < width * height; i++)
      typeP.at<uint8_t>(i) = (char) (((typeP.at<uint8_t>(i) & outputTypeMasks[outputType]) != 0) ? 255 : 0);
    outIp = typeP;
  }
  for(int y = 0, i = 0; y < outIp.rows; y++) {    // delete everything outside roi
    for(int x = 0; x < outIp.cols; x++, i++) {
      if(x < 0 || x >= 0 + ip.cols || y < 0 || y >= 0 + ip.rows) {
        outIp.at<uint8_t>(i) = (uint8_t) 0;
      } else if(mask != nullptr && (mask[x - 0 + ip.cols * (y - 0)] == 0)) {
        outIp.at<uint8_t>(i) = (uint8_t) 0;
      }
    }
  }

  return outIp;
}    //  ByteProcessor findMaxima

/** Find all local maxima (irrespective whether they finally qualify as maxima or not)
 * @param ip    The image to be analyzed
 * @param typeP A byte image, same size as ip, where the maximum points are marked as MAXIMUM
 *              (do not use it as output: for rois, the points are shifted w.r.t. the input image)
 * @param excludeEdgesNow Whether to exclude edge pixels
 * @param isEDM     Whether ip is a float Euclidian distance map
 * @param globalMin The minimum value of the image or roi
 * @param threshold The threshold (calibrated) below which no pixels are processed. Ignored if
 * MaximumFinder::NO_THRESHOLD
 * @return          Maxima sorted by value. In each array element (long, i.e., 64-bit integer), the value
 *                  is encoded in the upper 32 bits and the pixel offset in the lower 32 bit
 * Note: Do not use the positions of the points marked as MAXIMUM in typeP, they are invalid for images with a roi.
 */
long *MaximumFinder::getSortedMaxPoints(cv::Mat &ip, cv::Mat &typeP, bool excludeEdgesNow, bool isEDM, float globalMin,
                                        float globalMax, double threshold, size_t &maxPointSize)
{
  // byte[] types        = (byte[]) typeP.getPixels();
  int nMax            = 0;    // counts local maxima
  bool checkThreshold = threshold != MaximumFinder::NO_THRESHOLD;
  // long t0 = System.currentTimeMillis();
  for(int y = 0; y < ip.rows; y++) {    // find local maxima now
    for(int x = 0, i = x + y * width; x < ip.cols;
        x++, i++) {    // for better performance with rois, restrict search to roi
      float v     = ip.at<float>(y, x);
      float vTrue = isEDM ? trueEdmHeight(x, y, ip) : v;    // for EDMs, use interpolated ridge height
      if(v == globalMin)
        continue;
      if(excludeEdgesNow && (x == 0 || x == width - 1 || y == 0 || y == height - 1))
        continue;
      if(checkThreshold && v < threshold)
        continue;
      bool isMax = true;
      /* check wheter we have a local maximum.
       Note: For an EDM, we need all maxima: those of the EDM-corrected values
       (needed by findMaxima) and those of the raw values (needed by cleanupMaxima) */
      bool isInner =
          (y != 0 && y != height - 1) && (x != 0 && x != width - 1);    // not necessary, but faster than isWithin
      for(int d = 0; d < 8; d++) {                                      // compare with the 8 neighbor pixels
        if(isInner || isWithin(x, y, d)) {
          float vNeighbor     = ip.at<float>(y + DIR_Y_OFFSET[d], x + DIR_X_OFFSET[d]);
          float vNeighborTrue = isEDM ? trueEdmHeight(x + DIR_X_OFFSET[d], y + DIR_Y_OFFSET[d], ip) : vNeighbor;
          if(vNeighbor > v && vNeighborTrue > vTrue) {
            isMax = false;
            break;
          }
        }
      }
      if(isMax) {
        typeP.at<uint8_t>(i) = MAXIMUM;
        nMax++;
      }
    }    // for x
  }      // for y

  // long t1 = System.currentTimeMillis();IJ.log("markMax:"+(t1-t0));

  float vFactor   = (float) (2e9 / (globalMax - globalMin));    // for converting float values into a 32-bit int
  long *maxPoints = new long[nMax]{0};    // value (int) is in the upper 32 bit, pixel offset in the lower
  int iMax        = 0;
  for(int y = 0; y < ip.rows; y++)    // enter all maxima into an array
    for(int x = 0, p = x + y * width; x < ip.cols; x++, p++)
      if(typeP.at<uint8_t>(p) == MAXIMUM) {
        float fValue      = isEDM ? trueEdmHeight(x, y, ip) : ip.at<float>(y, x);
        int iValue        = (int) ((fValue - globalMin) * vFactor);    // 32-bit int, linear function of float value
        maxPoints[iMax++] = (long) iValue << 32 | p;
      }
  // long t2 = System.currentTimeMillis();IJ.log("makeArray:"+(t2-t1));
  // Arrays.sort(maxPoints);    // sort the maxima by value
  std::sort(maxPoints, maxPoints + nMax);

  // long t3 = System.currentTimeMillis();IJ.log("sort:"+(t3-t2));
  maxPointSize = nMax;
  return maxPoints;
}    // getSortedMaxPoints

/** Check all maxima in list maxPoints, mark type of the points in typeP
 * @param ip             the image to be analyzed
 * @param typeP          8-bit image, here the point types are marked by type: MAX_POINT, etc.
 * @param maxPoints      input: a list of all local maxima, sorted by height. Lower 32 bits are pixel offset
 * @param excludeEdgesNow whether to avoid MaximumFinder::edge maxima
 * @param isEDM          whether ip is a (float) Euclidian distance map
 * @param globalMin      minimum pixel value in ip
 * @param tolerance      minimum pixel value difference for two separate maxima
 * @param maxSortingError sorting may be inaccurate, sequence may be reversed for maxima having values
 *                       not deviating from each other by more than this (this could be a result of
 *                       precision loss when sorting ints instead of floats, or because sorting does not
 *                       take the height correction in 'trueEdmHeight' into account
 * @param outputType
 */
void MaximumFinder::analyzeAndMarkMaxima(const cv::Mat &ip, cv::Mat &typeP, long *maxPoints, size_t maxPointsSize,
                                         bool excludeEdgesNow, bool isEDM, float globalMin, double tolerance,
                                         bool strict, int outputType, float maxSortingError)
{
  // byte[] types        = (byte[]) typeP.getPixels();
  int nMax            = maxPointsSize;
  int *pList          = new int[width * height]{0};    // here we enter points starting from a maximum
  xyCoordinates       = {};
  bool displayOrCount = outputType == POINT_SELECTION || outputType == LIST || outputType == COUNT;
  if(displayOrCount) {
    xyCoordinates = {};
  }

  for(int iMax = nMax - 1; iMax >= 0; iMax--) {    // process all maxima now, starting from the highest

    int offset0 = (int) maxPoints[iMax];    // type cast gets 32 lower bits, where pixel index is encoded
    // int offset0 = maxPoints[iMax].offset;
    if((typeP.at<uint8_t>(offset0) & PROCESSED) != 0)    // this maximum has been reached from another one, skip it
      continue;
    // we create a list of connected points and start the list at the current maximum
    int x0   = offset0 % width;
    int y0   = offset0 / width;
    float v0 = isEDM ? trueEdmHeight(x0, y0, ip) : ip.at<float>(y0, x0);
    bool sortingError;
    do {    // repeat if we have encountered a sortingError
      pList[0] = offset0;
      typeP.at<uint8_t>(offset0) |= (EQUAL | LISTED);    // mark first point as equal height (to itself) and listed
      int listLen        = 1;                            // number of elements in the list
      int listI          = 0;                            // index of current element in the list
      bool isEdgeMaximum = (x0 == 0 || x0 == width - 1 || y0 == 0 || y0 == height - 1);
      sortingError       = false;    // if sorting was inaccurate: a higher maximum was not handled so far
      bool maxPossible   = true;     // it may be a true maximum
      double xEqual      = x0;       // for creating a single point: determine average over the
      double yEqual      = y0;       //  coordinates of contiguous equal-height points
      int nEqual         = 1;        // counts xEqual/yEqual points that we use for averaging
      do {                           // while neigbor list is not fully processed (to listLen)
        int offset = pList[listI];
        int x      = offset % width;
        int y      = offset / width;
        bool isInner =
            (y != 0 && y != height - 1) && (x != 0 && x != width - 1);    // not necessary, but faster than isWithin
        for(int d = 0; d < 8; d++) {    // analyze all neighbors (in 8 directions) at the same level
          int offset2 = offset + dirOffset[d];
          if((isInner || isWithin(x, y, d)) && (typeP.at<uint8_t>(offset2) & LISTED) == 0) {
            if(isEDM && ip.at<float>(offset2) <= 0)
              continue;    // ignore the background (non-particles)
            if((typeP.at<uint8_t>(offset2) & PROCESSED) != 0) {
              maxPossible = false;    // we have reached a point processed previously, thus it is no maximum now
              break;
            }
            int x2   = x + DIR_X_OFFSET[d];
            int y2   = y + DIR_Y_OFFSET[d];
            float v2 = isEDM ? trueEdmHeight(x2, y2, ip) : ip.at<float>(y2, x2);
            if(v2 > v0 + maxSortingError) {
              maxPossible = false;    // we have reached a higher point, thus it is no maximum
              break;
            } else if(v2 >= v0 - (float) tolerance) {
              if(v2 > v0) {    // maybe this point should have been treated earlier
                sortingError = true;
                offset0      = offset2;
                v0           = v2;
                x0           = x2;
                y0           = y2;
              }
              pList[listLen] = offset2;
              listLen++;    // we have found a new point within the tolerance
              typeP.at<uint8_t>(offset2) |= LISTED;
              if((x2 == 0 || x2 == width - 1 || y2 == 0 || y2 == height - 1) && (strict || v2 >= v0)) {
                isEdgeMaximum = true;
                if(excludeEdgesNow) {
                  maxPossible = false;
                  break;    // we have an edge maximum
                }
              }
              if(v2 == v0) {    // prepare finding center of equal points (in case single point needed)
                typeP.at<uint8_t>(offset2) |= EQUAL;
                xEqual += x2;
                yEqual += y2;
                nEqual++;
              }
            }
          }    // if isWithin & not LISTED
        }      // for directions d
        listI++;
      } while(listI < listLen);

      if(sortingError) {    // if x0,y0 was not the true maximum but we have reached a higher one
        for(listI = 0; listI < listLen; listI++)
          typeP.at<uint8_t>(pList[listI]) = 0;    // reset all points encountered, then retry
      } else {
        int resetMask = ~(maxPossible ? LISTED : (LISTED | EQUAL));
        xEqual /= nEqual;
        yEqual /= nEqual;
        double minDist2 = 1e20;
        int nearestI    = 0;
        for(listI = 0; listI < listLen; listI++) {
          int offset = pList[listI];
          int x      = offset % width;
          int y      = offset / width;
          typeP.at<uint8_t>(offset) &= resetMask;    // reset attributes no longer needed
          typeP.at<uint8_t>(offset) |= PROCESSED;    // mark as processed
          if(maxPossible) {
            typeP.at<uint8_t>(offset) |= MAX_AREA;
            if((typeP.at<uint8_t>(offset) & EQUAL) != 0) {
              double dist2 = (xEqual - x) * (double) (xEqual - x) + (yEqual - y) * (double) (yEqual - y);
              if(dist2 < minDist2) {
                minDist2 = dist2;    // this could be the best "single maximum" point
                nearestI = listI;
              }
            }
          }
        }    // for listI
        if(maxPossible) {
          int offset = pList[nearestI];
          typeP.at<uint8_t>(offset) |= MAX_POINT;
          if(displayOrCount && !(this->excludeOnEdges && isEdgeMaximum)) {
            int x = offset % width;
            int y = offset / width;
            xyCoordinates.push_back(cv::Point(y, x));
          }
        }
      }                       // if !sortingError
    } while(sortingError);    // redo if we have encountered a higher maximum: handle it now.
  }                           // for all maxima iMax
  if(nMax == 0) {             // no initial maxima at all? then consider all as 'within tolerance'
    // Arrays.fill(types, (byte) (PROCESSED | MAX_AREA));
    typeP.setTo((PROCESSED | MAX_AREA));
  }

}    // void MaximumFinder::analyzeAndMarkMaxima

/** Create an 8-bit image by scaling the pixel values of ip to 1-254 (<lower threshold 0) and mark maximum areas as
 * 255. For use as input for watershed segmentation
 * @param ip         The original image that should be segmented
 * @param typeP      Pixel types in ip
 * @param isEDM      Whether ip is an Euclidian distance map
 * @param globalMin  The minimum pixel value of ip
 * @param globalMax  The maximum pixel value of ip
 * @param threshold  Pixels of ip below this value (calibrated) are considered background. Ignored if
 * MaximumFinder::NO_THRESHOLD
 * @return           The 8-bit output image.
 */
cv::Mat MaximumFinder::make8bit(cv::Mat &ip, cv::Mat &typeP, bool isEDM, float globalMin, float globalMax,
                                double threshold)
{
  double minValue = 0;
  if(isEDM) {
    std::cout << "Is EDM " << std::endl;
    threshold = 0.5;
    minValue  = 1.;
  } else {
    minValue = (threshold == MaximumFinder::NO_THRESHOLD) ? globalMin : threshold;
  }
  double offset =
      minValue - (globalMax - minValue) *
                     (1.0F / 253.0F / 2.0F - (double) 1e-6);    // everything above minValue should become >(byte)0
  double factor = 253.0F / (globalMax - minValue);

  if(isEDM && factor > 1) {
    factor = 1;    // with EDM, no better resolution
  }
  cv::Mat outIp = cv::Mat::zeros(this->height, this->width, CV_8UC1);
  // convert possibly calibrated image to byte without damaging threshold (setMinAndMax would kill threshold)

  std::cout << "gm" << std::to_string(factor) << " | " << std::to_string(offset) << " | " << std::to_string(globalMin)
            << " | " << std::to_string(globalMax) << std::endl;

  long v = 0;
  for(int y = 0, i = 0; y < height; y++) {
    for(int x = 0; x < width; x++, i++) {
      float rawValue = ip.at<float>(y, x);
      if(threshold != MaximumFinder::NO_THRESHOLD && rawValue < threshold)
        outIp.at<uint8_t>(i) = (uint8_t) 0;
      else if((typeP.at<uint8_t>(i) & MAX_AREA) != 0)
        outIp.at<uint8_t>(i) = (uint8_t) 255;    // prepare watershed by setting "true" maxima+surroundings to 255
      else {
        v = 1 + std::round((rawValue - offset) * factor);
        if(v < 1)
          outIp.at<uint8_t>(i) = (uint8_t) 1;
        else if(v <= 254)
          outIp.at<uint8_t>(i) = (uint8_t) (v & 255);
        else
          outIp.at<uint8_t>(i) = (uint8_t) 254;
      }
    }
  }
  return outIp;
}    // byteProcessor make8bit

/** Get estimated "true" height of a maximum or saddle point of a Euclidian Distance Map.
 * This is needed since the point sampled is not necessarily at the highest position.
 * For simplicity, we don't care about the Sqrt(5) distance here although this would be more accurate
 * @param x     x-position of the point
 * @param y     y-position of the point
 * @param ip    the EDM (FloatProcessor)
 * @return      estimated height
 */
float MaximumFinder::trueEdmHeight(int x, int y, const cv::Mat &ip)
{
  int xmax   = width - 1;
  int ymax   = ip.rows - 1;
  int offset = x + y * width;
  float v    = ip.at<float>(offset);
  if(x == 0 || y == 0 || x == xmax || y == ymax || v == 0) {
    return v;    // don't recalculate for edge pixels or background
  } else {
    float trueH     = v + 0.5f * SQRT2;    // true height can never by higher than this
    bool ridgeOrMax = false;
    for(int d = 0; d < 4; d++) {    // for all directions halfway around:
      int d2   = (d + 4) % 8;       // get the opposite direction and neighbors
      float v1 = ip.at<float>(offset + dirOffset[d]);
      float v2 = ip.at<float>(offset + dirOffset[d2]);
      float h;
      if(v >= v1 && v >= v2) {
        ridgeOrMax = true;
        h          = (v1 + v2) / 2;
      } else {
        h = std::min(v1, v2);
      }
      h += (d % 2 == 0) ? 1 : SQRT2;    // in diagonal directions, distance is sqrt2
      if(trueH > h) {
        trueH = h;
      }
    }
    if(!ridgeOrMax) {
      trueH = v;
    }
    return trueH;
  }
}

/** eliminate unmarked maxima for use by watershed. Starting from each previous maximum,
 * explore the surrounding down to successively lower levels until a marked maximum is
 * touched (or the plateau of a previously eliminated maximum leads to a marked maximum).
 * Then set all the points above this value to this value
 * @param outIp     the image containing the pixel values
 * @param typeP     the types of the pixels are marked here
 * @param maxPoints array containing the coordinates of all maxima that might be relevant
 */
void MaximumFinder::cleanupMaxima(cv::Mat &outIp, cv::Mat &typeP, long *maxPoints, size_t maxPointSize)
{
  // byte[] pixels = (byte[]) outIp.getPixels();
  // byte[] types  = (byte[]) typeP.getPixels();
  int nMax   = maxPointSize;
  int *pList = new int[this->width * this->height]{0};
  for(int iMax = nMax - 1; iMax >= 0; iMax--) {
    int offset0 = (int) maxPoints[iMax];    // type cast gets lower 32 bits where pixel offset is encoded
    if((typeP.at<uint8_t>(offset0) & (MAX_AREA | ELIMINATED)) != 0)
      continue;
    int level   = outIp.at<uint8_t>(offset0) & 255;
    int loLevel = level + 1;
    pList[0]    = offset0;                   // we start the list at the current maximum
    typeP.at<uint8_t>(offset0) |= LISTED;    // mark first point as listed
    int listLen      = 1;                    // number of elements in the list
    int lastLen      = 1;
    int listI        = 0;    // index of current element in the list
    bool saddleFound = false;
    while(!saddleFound && loLevel > 0) {
      loLevel--;
      lastLen = listLen;    // remember end of list for previous level
      listI   = 0;          // in each level, start analyzing the neighbors of all pixels
      do {                  // for all pixels listed so far
        int offset = pList[listI];
        int x      = offset % width;
        int y      = offset / width;
        bool isInner =
            (y != 0 && y != height - 1) && (x != 0 && x != width - 1);    // not necessary, but faster than isWithin
        for(int d = 0; d < 8; d++) {    // analyze all neighbors (in 8 directions) at the same level
          int offset2 = offset + dirOffset[d];
          if((isInner || isWithin(x, y, d)) && (typeP.at<uint8_t>(offset2) & LISTED) == 0) {
            if((typeP.at<uint8_t>(offset2) & MAX_AREA) != 0 ||
               (((typeP.at<uint8_t>(offset2) & ELIMINATED) != 0) && (outIp.at<uint8_t>(offset2) & 255) >= loLevel)) {
              saddleFound = true;    // we have reached a point touching a "true" maximum...
              break;                 //...or a level not lower, but touching a "true" maximum
            } else if((outIp.at<uint8_t>(offset2) & 255) >= loLevel && (typeP.at<uint8_t>(offset2) & ELIMINATED) == 0) {
              pList[listLen] = offset2;
              // xList[listLen] = x+DIR_X_OFFSET[d];
              // yList[listLen] = x+DIR_Y_OFFSET[d];
              listLen++;    // we have found a new point to be processed
              typeP.at<uint8_t>(offset2) |= LISTED;
            }
          }    // if isWithin & not LISTED
        }      // for directions d
        if(saddleFound) {
          break;    // no reason to search any further
        }
        listI++;
      } while(listI < listLen);
    }                                             // while !levelFound && loLevel>=0
    for(listI = 0; listI < listLen; listI++) {    // reset attribute since we may come to this place again
      typeP.at<uint8_t>(pList[listI]) &= ~LISTED;
    }
    for(listI = 0; listI < lastLen; listI++) {    // for all points higher than the level of the saddle point
      int offset                = pList[listI];
      outIp.at<uint8_t>(offset) = (uint8_t) loLevel;    // set pixel value to the level of the saddle point
      typeP.at<uint8_t>(offset) |= ELIMINATED;    // mark as processed: there can't be a local maximum in this area
    }
  }    // for all maxima iMax
}    // void MaximumFinder::cleanupMaxima

/** Delete extra structures form watershed of non-EDM images, e.g., foreground patches,
 *  single dots and lines ending somewhere within a segmented particle
 *  Needed for post-processing watershed-segmented images that can have local minima
 *  @param ip 8-bit image with background = 0, lines between 1 and 254 and segmented particles = 255
 */
void MaximumFinder::cleanupExtraLines(cv::Mat &ip)
{
  for(int y = 0, i = 0; y < ip.rows; y++) {
    for(int x = 0; x < ip.cols; x++, i++) {
      int v = ip.at<uint8_t>(i);
      if(v != (uint8_t) 255 && v != 0) {
        int nRadii = this->nRadii(ip.data, x, y);    // number of lines radiating
        if(nRadii == 0)                              // single point or foreground patch?
          ip.at<uint8_t>(i) = (uint8_t) 255;
        else if(nRadii == 1)
          removeLineFrom(ip.data, x, y);
      }    // if v<255 && v>0
    }      // for x
  }        // for y
}    // void MaximumFinder::cleanupExtraLines

/** delete a line starting at x, y up to the next (4-connected) vertex */
void MaximumFinder::removeLineFrom(uint8_t *pixels, int x, int y)
{
  // IJ.log("del line from "+x+","+y);
  pixels[x + width * y] = (uint8_t) 255;    // delete the first point
  bool continues;
  do {
    continues = false;
    bool isInner =
        (y != 0 && y != height - 1) && (x != 0 && x != width - 1);    // not necessary, but faster than isWithin
    for(int d = 0; d < 8; d += 2) {                                   // analyze 4-connected neighbors
      if(isInner || isWithin(x, y, d)) {
        int v = pixels[x + width * y + dirOffset[d]];
        if(v != (uint8_t) 255 && v != 0) {
          int nRadii = this->nRadii(pixels, x + DIR_X_OFFSET[d], y + DIR_Y_OFFSET[d]);
          if(nRadii <= 1) {    // found a point or line end
            x += DIR_X_OFFSET[d];
            y += DIR_Y_OFFSET[d];
            pixels[x + width * y] = (uint8_t) 255;    // delete the point
            continues             = nRadii == 1;      // continue along that line
            break;
          }
        }
      }
    }    // for directions d
  } while(continues);
  // IJ.log("deleted to "+x+","+y);
}    // void MaximumFinder::removeLineFrom

/** Analyze the neighbors of a pixel (y, x) in a byte image; pixels <255 ("non-white") are
 * considered foreground. Edge pixels are considered foreground.
 * @param   ip
 * @param   x coordinate of the point
 * @param   y coordinate of the point
 * @return  Number of 4-connected lines emanating from this point. Zero if the point is
 *          embedded in either foreground or background
 */
int MaximumFinder::nRadii(uint8_t *pixels, int x, int y)
{
  int offset           = x + y * width;
  int countTransitions = 0;
  bool prevPixelSet    = true;
  bool firstPixelSet   = true;    // initialize to make the compiler happy
  bool isInner =
      (y != 0 && y != height - 1) && (x != 0 && x != width - 1);    // not necessary, but faster than isWithin
  for(int d = 0; d < 8; d++) {    // walk around the point and note every no-line->line transition
    bool pixelSet = prevPixelSet;
    if(isInner || isWithin(x, y, d)) {
      bool isSet = (pixels[offset + dirOffset[d]] != (uint8_t) 255);
      if((d & 1) == 0)
        pixelSet = isSet;    // non-diagonal directions: always regarded
      else if(!isSet)        // diagonal directions may separate two lines,
        pixelSet = false;    //    but are insufficient for a 4-connected line
    } else {
      pixelSet = true;
    }
    if(pixelSet && !prevPixelSet)
      countTransitions++;
    prevPixelSet = pixelSet;
    if(d == 0)
      firstPixelSet = pixelSet;
  }
  if(firstPixelSet && !prevPixelSet)
    countTransitions++;
  return countTransitions;
}    // int nRadii

/** after watershed, set all pixels in the background and segmentation lines to 0
 */

void MaximumFinder::watershedPostProcess(cv::Mat &ip)
{
  int size = ip.cols * ip.rows;
  for(int i = 0; i < size; i++) {
    if((ip.at<uint8_t>(i) & 255) < 255) {
      ip.at<uint8_t>(i) = (uint8_t) 0;
    } else {
      std::cout << "NA" << std::endl;
    }
  }
  // new ImagePlus("after postprocess",ip.duplicate()).show();
}

/** delete particles corresponding to edge maxima
 * @param typeP Here the pixel types of the original image are noted,
 * pixels with bit MAX_AREA at the edge are considered indicators of an edge maximum.
 * @param ip the image resulting from watershed segmentaiton
 * (foreground pixels, i.e. particles, are 255, background 0)
 */
void MaximumFinder::deleteEdgeParticles(cv::Mat &ip, cv::Mat &typeP)
{
  // byte[] pixels = (byte[]) ip.getPixels();
  // byte[] types  = (byte[]) typeP.getPixels();
  width      = ip.cols;
  height     = ip.rows;
  Wand *wand = new Wand(ip);
  for(int x = 0; x < width; x++) {
    int y = 0;
    if((typeP.at<uint8_t>(x + y * width) & MAX_AREA) != 0 && ip.at<uint8_t>(x + y * width) != 0)
      deleteParticle(x, y, ip, *wand);
    y = height - 1;
    if((typeP.at<uint8_t>(x + y * width) & MAX_AREA) != 0 && ip.at<uint8_t>(x + y * width) != 0)
      deleteParticle(x, y, ip, *wand);
  }
  for(int y = 1; y < height - 1; y++) {
    int x = 0;
    if((typeP.at<uint8_t>(x + y * width) & MAX_AREA) != 0 && ip.at<uint8_t>(x + y * width) != 0)
      deleteParticle(x, y, ip, *wand);
    x = width - 1;
    if((typeP.at<uint8_t>(x + y * width) & MAX_AREA) != 0 && ip.at<uint8_t>(x + y * width) != 0)
      deleteParticle(x, y, ip, *wand);
  }
}    // void MaximumFinder::deleteEdgeParticles

/** delete a particle (set from value 255 to current fill value).
 * Position x,y must be within the particle
 */
void MaximumFinder::deleteParticle(int x, int y, cv::Mat &ip, Wand &wand)
{
  /*
  wand.autoOutline(x, y, 255, 255);
  if(wand.npoints == 0) {
    return;
  }
  Roi roi = new PolygonRoi(wand.xpoints, wand.ypoints, wand.npoints, Roi.TRACED_ROI);
  ip.snapshot();    // prepare for reset outside of mask
  ip.setRoi(roi);
  ip.fill();
  ip.reset(ip.getMask());
  */
}

/** Do watershed segmentation on a byte image, with the start points (maxima)
 * set to 255 and the background set to 0. The image should not have any local maxima
 * other than the marked ones. Local minima will lead to artifacts that can be removed
 * later. On output, all particles will be set to 255, segmentation lines remain at their
 * old value.
 * @param ip  The byteProcessor containing the image, with size given by the class variables width and height
 * @return    false if canceled by the user (note: can be cancelled only if called by "run" with a known ImagePlus)
 */

bool MaximumFinder::watershedSegment(cv::Mat &ip)
{
  // Create an array with the coordinates of all points between value 1 and 254
  // This method, suggested by Stein Roervik (stein_at_kjemi-dot-unit-dot-no),
  // greatly speeds up the watershed segmentation routine.
  // Calculate the histogram of the image
  int histSize           = UINT8_MAX + 1;    // Number of bins
  float range[]          = {0, 256};         // Pixel value range
  const float *histRange = {range};
  cv::Mat histogram;
  cv::calcHist(&ip, 1, 0, cv::Mat(), histogram, 1, &histSize, &histRange);

  std::cout << "Hist " << std::to_string(histogram.at<float>(0)) << " " << std::to_string(histogram.at<float>(255))
            << std::endl;
  //         int arraySize = width*height - histogram[0] -histogram[255];

  ///////////////////////////
  int arraySize    = width * height - histogram.at<float>(0) - histogram.at<float>(255);
  int *coordinates = coordinates = new int[arraySize]{0};    // from pixel coordinates, low bits x, high bits y

  int highestValue = 0;
  int maxBinSize   = 0;
  int offset       = 0;
  int *levelStart  = new int[256]{0};
  for(int v = 1; v < 255; v++) {
    levelStart[v] = offset;
    offset += histogram.at<float>(v);
    if(histogram.at<float>(v) > 0) {
      highestValue = v;
    }
    if(histogram.at<float>(v) > maxBinSize) {
      maxBinSize = histogram.at<float>(v);
    }
  }
  int *levelOffset = new int[highestValue + 1]{0};
  for(int y = 0, i = 0; y < height; y++) {
    for(int x = 0; x < width; x++, i++) {
      int v = ip.at<uint8_t>(i) & 255;
      if(v > 0 && v < 255) {
        offset              = levelStart[v] + levelOffset[v];
        coordinates[offset] = x | y << intEncodeShift;
        levelOffset[v]++;
      }
    }    // for x
  }      // for y
  // Create an array of the points (pixel offsets) that we set to 255 in one pass.
  // If we remember this list we need not create a snapshot of the ImageProcessor.
  int *setPointList = new int[std::min(maxBinSize, (width * height + 2) / 3)]{0};
  // now do the segmentation, starting at the highest level and working down.
  // At each level, dilate the particle (set pixels to 255), constrained to pixels
  // whose values are at that level and also constrained (by the fateTable)
  // to prevent features from merging.
  int *table              = makeFateTable();
  int directionSequence[] = {7, 3, 1, 5, 0, 4, 2, 6};    // diagonal directions first
  for(int level = highestValue; level >= 1; level--) {
    int remaining = histogram.at<float>(level);    // number of points in the level that have not been processed
    int idle      = 0;
    while(remaining > 0 && idle < 8) {
      int sumN   = 0;
      int dIndex = 0;
      do {    // expand each level in 8 directions -> manipukates image
        int n = processLevel(directionSequence[dIndex % 8], ip, table, levelStart[level], remaining, coordinates,
                             setPointList);
        // IJ.log("level="+level+" direction="+directionSequence[dIndex%8]+" remain="+remaining+"-"+n);
        remaining -= n;    // number of points processed
        sumN += n;
        if(n > 0) {
          idle = 0;    // nothing processed in this direction?
        }
        dIndex++;
      } while(remaining > 0 && idle++ < 8);
    }
    if(remaining > 0 && level > 1) {    // any pixels that we have not reached?
      int nextLevel = level;            // find the next level to process
      do {
        nextLevel--;
      } while(nextLevel > 1 && (int) histogram.at<float>(nextLevel) == 0);
      // in principle we should add all unprocessed pixels of this level to the
      // tasklist of the next level. This would make it very slow for some images,
      // however. Thus we only add the pixels if they are at the border (of the
      // image or a thresholded area) and correct unprocessed pixels at the very
      // end by CleanupExtraLines
      if(nextLevel > 0) {
        int newNextLevelEnd = levelStart[nextLevel] + histogram.at<float>(nextLevel);
        for(int i = 0, p = levelStart[level]; i < remaining; i++, p++) {
          int xy      = coordinates[p];
          int x       = xy & intEncodeXMask;
          int y       = (xy & intEncodeYMask) >> intEncodeShift;
          int pOffset = x + y * width;
          if((ip.at<uint8_t>(pOffset) & 255) == 255) {
            // IJ.log("ERROR");
          }
          bool addToNext = false;
          if(x == 0 || y == 0 || x == width - 1 || y == height - 1) {
            addToNext = true;    // image border
          } else {
            for(int d = 0; d < 8; d++)
              if(isWithin(x, y, d) && ip.at<uint8_t>(pOffset + dirOffset[d]) == 0) {
                addToNext = true;    // border of area below threshold
                break;
              }
          }
          if(addToNext) {
            coordinates[newNextLevelEnd++] = xy;
          }
        }
        // IJ.log("level="+level+": add "+(newNextLevelEnd-levelStart[nextLevel+1])+" points to "+nextLevel);
        // tasklist for the next level to process becomes longer by this:
        histogram.at<float>(nextLevel) = newNextLevelEnd - levelStart[nextLevel];
      }
    }
  }

  return true;
}    // bool watershedSegment

/** dilate the UEP on one level by one pixel in the direction specified by step, i.e., set pixels to 255
 * @param pass gives direction of dilation, see makeFateTable
 * @param ip the EDM with the segmeted blobs successively getting set to 255
 * @param table             The fateTable
 * @param levelStart        offsets of the level in pixelPointers[]
 * @param levelNPoints      number of points in the current level
 * @param pixelPointers[]   list of pixel coordinates (x+y*width) sorted by level (in sequence of y, x within each
 * level)
 * @param xCoordinates      list of x Coorinates for the current level only (no offset levelStart)
 * @return                  number of pixels that have been changed
 */

int MaximumFinder::processLevel(int pass, cv::Mat &ip, int *fateTable, int levelStart, int levelNPoints,
                                int *coordinates, int *setPointList)
{
  int xmax = width - 1;
  int ymax = height - 1;
  // byte[] pixels2 = (byte[])ip2.getPixels();
  int nChanged   = 0;
  int nUnchanged = 0;
  for(int i = 0, p = levelStart; i < levelNPoints; i++, p++) {
    int xy     = coordinates[p];
    int x      = xy & intEncodeXMask;
    int y      = (xy & intEncodeYMask) >> intEncodeShift;
    int offset = x + y * width;
    int index  = 0;    // neighborhood pixel ocupation: index in fateTable
    if(y > 0 && (ip.at<uint8_t>(offset - width) & 255) == 255) {
      index ^= 1;
    }
    if(x < xmax && y > 0 && (ip.at<uint8_t>(offset - width + 1) & 255) == 255) {
      index ^= 2;
    }
    if(x < xmax && (ip.at<uint8_t>(offset + 1) & 255) == 255) {
      index ^= 4;
    }
    if(x < xmax && y < ymax && (ip.at<uint8_t>(offset + width + 1) & 255) == 255) {
      index ^= 8;
    }
    if(y < ymax && (ip.at<uint8_t>(offset + width) & 255) == 255) {
      index ^= 16;
    }
    if(x > 0 && y < ymax && (ip.at<uint8_t>(offset + width - 1) & 255) == 255) {
      index ^= 32;
    }
    if(x > 0 && (ip.at<uint8_t>(offset - 1) & 255) == 255) {
      index ^= 64;
    }
    if(x > 0 && y > 0 && (ip.at<uint8_t>(offset - width - 1) & 255) == 255) {
      index ^= 128;
    }
    int mask = 1 << pass;

    if((fateTable[index] & mask) == mask) {
      setPointList[nChanged++] = offset;    // remember to set pixel to 255
    } else {
      coordinates[levelStart + (nUnchanged++)] = xy;    // keep this pixel for future passes
    }

  }    // for pixel i
       // IJ.log("pass="+pass+", changed="+nChanged+" unchanged="+nUnchanged);

  for(int i = 0; i < nChanged; i++) {
    ip.at<uint8_t>(setPointList[i]) = (uint8_t) 255;
  }
  return nChanged;
}    // processLevel

/** Creates the lookup table used by the watershed function for dilating the particles.
 * The algorithm allows dilation in both straight and diagonal directions.
 * There is an entry in the table for each possible 3x3 neighborhood:
 *          x-1          x          x+1
 *  y-1    128            1          2
 *  y       64     pxl_unset_yet     4
 *  y+1     32           16          8
 * (to find throws entry, sum up the numbers of the neighboring pixels set; e.g.
 * entry 6=2+4 if only the pixels (x,y-1) and (x+1, y-1) are set.
 * A pixel is added on the 1st pass if bit 0 (2^0 = 1) is set,
 * on the 2nd pass if bit 1 (2^1 = 2) is set, etc.
 * pass gives the direction of rotation, with 0 = to top left (x--,y--), 1 to top,
 * and clockwise up to 7 = to the left (x--).
 * E.g. 4 = add on 3rd pass, 3 = add on either 1st or 2nd pass.
 */

int *MaximumFinder::makeFateTable()
{
  int *table  = new int[256]{0};
  bool *isSet = new bool[8]{0};
  for(int item = 0; item < 256; item++) {    // dissect into pixels
    for(int i = 0, mask = 1; i < 8; i++) {
      isSet[i] = (item & mask) == mask;
      mask *= 2;
    }
    for(int i = 0, mask = 1; i < 8;
        i++) {    // we dilate in the direction opposite to the direction of the existing neighbors
      if(isSet[(i + 4) % 8])
        table[item] |= mask;
      mask *= 2;
    }
    for(int i = 0; i < 8;
        i +=
        2) {    // if side pixels are set, for counting transitions it is as good as if the adjacent edges were also set
      if(isSet[i]) {
        isSet[(i + 1) % 8] = true;
        isSet[(i + 7) % 8] = true;
      }
    }
    int transitions = 0;
    for(int i = 0, mask = 1; i < 8; i++) {
      if(isSet[i] != isSet[(i + 1) % 8])
        transitions++;
    }
    if(transitions >= 4) {    // if neighbors contain more than one region, dilation ito this pixel is forbidden
      table[item] = 0;
    } else {
    }
  }
  return table;
}    // int[] makeFateTable

/** create an array of offsets within a pixel array for directions in clockwise order:
 * 0=(x,y-1), 1=(x+1,y-1), ... 7=(x-1,y)
 * Also creates further class variables:
 * width, height, and the following three values needed for storing coordinates in single ints for watershed:
 * intEncodeXMask, intEncodeYMask and intEncodeShift.
 * E.g., for width between 129 and 256, xMask=0xff and yMask = 0xffffff00 are bitwise masks
 * for x and y, respectively, and shift=8 is the bit shift to get y from the y-masked value
 * Returns as class variables: the arrays of the offsets to the 8 neighboring pixels
 * and the array maskAndShift for watershed
 */
void MaximumFinder::makeDirectionOffsets(cv::Mat &ip)
{
  width     = ip.cols;
  height    = ip.rows;
  int shift = 0;
  int mult  = 1;
  do {
    shift++;
    mult *= 2;
  } while(mult < width);
  intEncodeXMask = mult - 1;
  intEncodeYMask = ~intEncodeXMask;
  intEncodeShift = shift;
  // IJ.log("masks (hex):"+Integer.toHexString(xMask)+","+Integer.toHexString(xMask)+"; shift="+shift);
  dirOffset = new int[12]{-width, -width + 1, +1, +width + 1, +width, +width - 1, -1, -width - 1};
  // dirOffset is created last, so check for it being null before makeDirectionOffsets
  //(in case we have multiple threads using the same MaximumFinder)
}

/** returns whether the neighbor in a given direction is within the image
 * NOTE: it is assumed that the pixel x,y itself is within the image!
 * Uses class variables width, height: dimensions of the image
 * @param x         x-coordinate of the pixel that has a neighbor in the given direction
 * @param y         y-coordinate of the pixel that has a neighbor in the given direction
 * @param direction the direction from the pixel towards the neighbor (see makeDirectionOffsets)
 * @return          true if the neighbor is within the image (provided that x, y is within)
 */
bool MaximumFinder::isWithin(int x, int y, int direction)
{
  int xmax = width - 1;
  int ymax = height - 1;
  switch(direction) {
    case 0:
      return (y > 0);
    case 1:
      return (x < xmax && y > 0);
    case 2:
      return (x < xmax);
    case 3:
      return (x < xmax && y < ymax);
    case 4:
      return (y < ymax);
    case 5:
      return (x > 0 && y < ymax);
    case 6:
      return (x > 0);
    case 7:
      return (x > 0 && y > 0);
  }
  return false;    // to make the compiler happy :-)
}    // isWithin

/** add work done in the meanwhile and show progress */
