
///
/// \file      rolling_ball_sliding_paraboloid.cpp
/// \author    Joachim Danmayr
/// \date      2023-02-20
///
/// \brief     C++ implementation of rolling ball algorithm based on
///            ImageJ rolling ball suggested by Michael Castle and Janice Keller
///            https://imagej.net/plugins/rolling-ball-background-subtraction
///
///            ImageJ's Subtract Background command in version versions up to 1.39e,
///            but with "preview" added and support for multiprocessor machines when
///            processing stacks. This plugin does not support Float (32-bit) images.
///            ImageJ uses a different algorithm since 1.39f (slighly modified in 1.39k).
///
///            This algorithm produces better results than the new ImageJ code for
///            some 16-bit images, but it has the disadvantage of producing artifacts
///            for many images if the ball radius is >=10.
///
///            Based on the NIH Image Pascal version by Michael Castle and Janice
///            Keller of the University of Michigan Mental Health Research
///            Institute. Rolling ball algorithm inspired by Stanley
///            Sternberg's article, "Biomedical Image Processing",
///            IEEE Computer, January 1983.
///
/// \ref       https://imagej.nih.gov/ij/source/ij/plugin/filter/BackgroundSubtracter.java
/// \ref       https://github.com/imagej/ImageJ/blob/master/ij/plugin/filter/BackgroundSubtracter.java
///

#include <cfloat>
#include <climits>
#include <cstddef>
#include "rolling_ball.hpp"

namespace joda::cmd {

///
/// \brief     Create background for a float image by sliding a paraboloid over the image
/// \author
/// \return
///
void RollingBallBackground::slidingParaboloidFloatBackground(cv::Mat &fp, float radiusIn, bool invert, bool doPresmooth, bool correctCorners) const
{
  // float[] pixels   = (float[]) fp.getPixels();    // this will become the background
  int width        = fp.cols;
  int height       = fp.rows;
  uint64_t length  = width * height;
  auto *cache      = new float[static_cast<size_t>(std::max(width, height))]{0};    // work array for lineSlideParabola
  int *nextPoint   = new int[static_cast<size_t>(std::max(width, height))]{0};      // work array for lineSlideParabola
  float coeff2     = 0.5F / radiusIn;                                               // 2nd-order coefficient of the polynomial approximating the ball
  float coeff2diag = 1.0F / radiusIn;                                               // same for diagonal directions where step is sqrt2

  if(invert) {
    for(int i = 0; i < static_cast<int>(fp.total()); i++) {
      fp.at<float>(i) = -fp.at<float>(i);
    }
  }

  float shiftBy = 0;
  if(doPresmooth) {
    shiftBy = static_cast<float>(filter3x3(fp, MAXIMUM));    // 3x3 maximum to remove dust etc.
    filter3x3(fp, MEAN);                                     // smoothing to remove noise
  }
  if(correctCorners) {
    this->correctCorners(fp, coeff2, cache, nextPoint);    // modify corner data, avoids subtracting corner particles
  }

  /* Slide the parabola over the image in different directions */
  /* Doing the diagonal directions at the end is faster (diagonal lines are denser,
   * so there are more such lines, and the algorithm gets faster with each iteration) */
  filter1D(fp, X_DIRECTION, coeff2, cache, nextPoint);
  filter1D(fp, Y_DIRECTION, coeff2, cache, nextPoint);
  filter1D(fp, X_DIRECTION, coeff2, cache, nextPoint);    // redo for better accuracy
  filter1D(fp, DIAGONAL_1A, coeff2diag, cache, nextPoint);
  filter1D(fp, DIAGONAL_1B, coeff2diag, cache, nextPoint);
  filter1D(fp, DIAGONAL_2A, coeff2diag, cache, nextPoint);
  filter1D(fp, DIAGONAL_2B, coeff2diag, cache, nextPoint);
  filter1D(fp, DIAGONAL_1A, coeff2diag, cache, nextPoint);    // redo for better accuracy
  filter1D(fp, DIAGONAL_1B, coeff2diag, cache, nextPoint);

  if(invert) {
    for(uint64_t i = 0; i < length; i++) {
      fp.at<float>(static_cast<int>(i)) = -(fp.at<float>(static_cast<int>(i)) - shiftBy);
    }
  } else if(doPresmooth) {
    for(uint64_t i = 0; i < length; i++) {
      fp.at<float>(static_cast<int>(i)) -= shiftBy;    // correct for shift by 3x3 maximum
    }
  }

  delete[] cache;
  delete[] nextPoint;
}

///
/// \brief     Filter by subtracting a sliding parabola for all lines in one direction, x, y or one of
///            the two diagonal directions (diagonals are processed only for half the image per call).
/// \author
/// \return
///
void RollingBallBackground::filter1D(cv::Mat &fp, int direction, float coeff2, float *cache, int *nextPoint) const
{
  int width     = fp.cols;
  int height    = fp.rows;
  int startLine = 0;    // index of the first line to handle
  int nLines    = 0;    // index+1 of the last line to handle (initialized to avoid compile-time error)
  int lineInc   = 0;    // increment from one line to the next in pixels array
  int pointInc  = 0;    // increment from one point to the next along the line
  int length    = 0;    // length of the line
  switch(direction) {
    case X_DIRECTION:    // lines parallel to x direction
      nLines   = height;
      lineInc  = width;
      pointInc = 1;
      length   = width;
      break;
    case Y_DIRECTION:    // lines parallel to y direction
      nLines   = width;
      lineInc  = 1;
      pointInc = width;
      length   = height;
      break;
    case DIAGONAL_1A:          // lines parallel to x=y, starting at x axis
      nLines   = width - 2;    // the algorithm makes no sense for lines shorter than 3 pixels
      lineInc  = 1;
      pointInc = width + 1;
      break;
    case DIAGONAL_1B:    // lines parallel to x=y, starting at y axis
      startLine = 1;
      nLines    = height - 2;
      lineInc   = width;
      pointInc  = width + 1;
      break;
    case DIAGONAL_2A:    // lines parallel to x=-y, starting at x axis
      startLine = 2;
      nLines    = width;
      lineInc   = 1;
      pointInc  = width - 1;
      break;
    case DIAGONAL_2B:    // lines parallel to x=-y, starting at x=width-1, y=variable
      startLine = 0;
      nLines    = height - 2;
      lineInc   = width;
      pointInc  = width - 1;
      break;
  }
  for(int i = startLine; i < nLines; i++) {
    int startPixel = i * lineInc;
    if(direction == DIAGONAL_2B) {
      startPixel += width - 1;
    }
    switch(direction) {
      case DIAGONAL_1A:
        length = std::min(height, width - i);
        break;
      case DIAGONAL_1B:
        length = std::min(width, height - i);
        break;
      case DIAGONAL_2A:
        length = std::min(height, i + 1);
        break;
      case DIAGONAL_2B:
        length = std::min(width, height - i);
        break;
    }
    RollingBallBackground::lineSlideParabola(fp, startPixel, pointInc, length, coeff2, cache, nextPoint, nullptr);
  }
}    // void filter1D

/// \brief Process one straight line in the image by sliding a parabola along the line
///        (from the bottom) and setting the values to make all points reachable by
///        the parabola
///  @param pixels    Image data, will be modified by parabolic interpolation
///                   where the parabola does not touch.
///  @param start     Index of first pixel of the line in pixels array
///  @param inc       Increment of index in pixels array
///  @param length    Number of points the line consists of
///  @param coeff2    2nd order coefficient of the polynomial describing the parabola,
///                   must be positive (although a parabola with negative curvature is
///                   actually used)
///  @param cache     Work array, length at least <code>length</code>. Will usually remain
///                   in the CPU cache and may therefore speed up the code.
///  @param nextPoint Work array. Will hold the index of the next point with sufficient local
///                   curvature to get touched by the parabola.
///  @param correctedEdges Should be a 2-element array used for output or null.
///  @return          The correctedEdges array (if non-null on input) with the two estimated
///                   edge pixel values corrected for edge particles.
///
float *RollingBallBackground::lineSlideParabola(cv::Mat &pixels, int start, int inc, int length, float coeff2, float *cache, int *nextPoint,
                                                float *correctedEdges) const
{
  float minValue      = FLT_MAX;
  int lastpoint       = 0;
  int firstCorner     = length - 1;    // the first point except the edge that is touched
  int lastCorner      = 0;             // the last point except the edge that is touched
  float vPrevious1    = 0.0F;
  float vPrevious2    = 0.0F;
  float curvatureTest = 1.999F * coeff2;    // not 2: numeric scatter of 2nd derivative
  /* copy data to cache, determine the minimum, and find points with local curvature such
   * that the parabola can touch them - only these need to be examined futher on */
  for(int i = 0, p = start; i < length; i++, p += inc) {
    float v  = pixels.at<float>(p);
    cache[i] = v;
    if(v < minValue) {
      minValue = v;
    }
    if(i >= 2 && vPrevious1 + vPrevious1 - vPrevious2 - v < curvatureTest) {
      nextPoint[lastpoint] = i - 1;    // point i-1 may be touched
      lastpoint            = i - 1;
    }
    vPrevious2 = vPrevious1;
    vPrevious1 = v;
  }
  nextPoint[lastpoint]  = length - 1;
  nextPoint[length - 1] = INT_MAX;    // breaks the search loop

  int i1 = 0;    // i1 and i2 will be the two points where the parabola touches
  while(i1 < length - 1) {
    float v1                = cache[i1];
    float minSlope          = FLT_MAX;
    int i2                  = 0;    //(initialized to avoid compile-time error)
    int searchTo            = length;
    int recalculateLimitNow = 0;    // when 0, limits for searching will be recalculated
    /* find the second point where the parabola through point i1,v1 touches: */
    for(int j = nextPoint[i1]; j < searchTo; j = nextPoint[j], recalculateLimitNow++) {
      float v2    = cache[j];
      float slope = (v2 - v1) / static_cast<float>(j - i1) + coeff2 * static_cast<float>(j - i1);
      if(slope < minSlope) {
        minSlope            = slope;
        i2                  = j;
        recalculateLimitNow = -3;
      }
      if(recalculateLimitNow == 0) {    // time-consuming recalculation of search limit: wait a bit after slope is updated
        auto b        = static_cast<double>(0.5F * minSlope / coeff2);
        int maxSearch = i1 + static_cast<int>(b + std::sqrt(b * b + static_cast<double>((v1 - minValue) / coeff2)) +
                                              1.0);    //(numeric overflow may make this negative)
        if(maxSearch < searchTo && maxSearch > 0) {
          searchTo = maxSearch;
        }
      }
    }
    if(i1 == 0) {
      firstCorner = i2;
    }
    if(i2 == length - 1) {
      lastCorner = i1;
    }
    /* interpolate between the two points where the parabola touches: */
    for(int j = i1 + 1, p = start + j * inc; j < i2; j++, p += inc) {
      pixels.at<float>(p) = v1 + static_cast<float>(j - i1) * (minSlope - static_cast<float>(j - i1) * coeff2);
    }
    i1 = i2;    // continue from this new point
  }             // while (i1<length-1)
  /* Now calculate estimated edge values without an edge particle, allowing for vignetting
   * described as a 6th-order polynomial: */
  if(correctedEdges != nullptr) {
    if(4 * firstCorner >= length) {
      firstCorner = 0;    // edge particles must be < 1/4 image size
    }
    if(4 * (length - 1 - lastCorner) >= length) {
      lastCorner = length - 1;
    }
    float v1     = cache[firstCorner];
    float v2     = cache[lastCorner];
    float slope  = (v2 - v1) / static_cast<float>(lastCorner - firstCorner);    // of the line through the two outermost non-edge touching points
    float value0 = v1 - slope * static_cast<float>(firstCorner);                // offset of this line
    float coeff6 = 0;                                                           // coefficient of 6th order polynomial
    float mid    = 0.5F * static_cast<float>(lastCorner + firstCorner);
    for(int i = (length + 2) / 3; i <= (2 * length) / 3; i++) {    // compare with mid-image pixels to detect vignetting
      float dx    = (i - mid) * 2.0F / static_cast<float>(lastCorner - firstCorner);
      float poly6 = dx * dx * dx * dx * dx * dx - 1.0F;    // the 6th order polynomial, zero at firstCorner and lastCorner
      if(cache[i] < value0 + slope * static_cast<float>(i) + coeff6 * poly6) {
        coeff6 = -(value0 + slope * static_cast<float>(i) - cache[i]) / poly6;
      }
    }
    float dx          = (static_cast<float>(firstCorner) - mid) * 2.0F / static_cast<float>(lastCorner - firstCorner);
    correctedEdges[0] = value0 + coeff6 * (dx * dx * dx * dx * dx * dx - 1.0F) + coeff2 * static_cast<float>(firstCorner * firstCorner);
    dx                = (static_cast<float>(lastCorner) - mid) * 2.0F / static_cast<float>(lastCorner - firstCorner);
    correctedEdges[1] = value0 + static_cast<float>(length - 1) * slope + coeff6 * (dx * dx * dx * dx * dx * dx - 1.0F) +
                        coeff2 * static_cast<float>(length - 1 - lastCorner) * static_cast<float>(length - 1 - lastCorner);
  }
  return correctedEdges;
}    // void lineSlideParabola

///
/// \brief Detect corner particles and adjust corner pixels if a particle is there.
///        Analyzing the directions parallel to the edges and the diagonals, we
///        average over the 3 correction values (found for the 3 directions)
///
void RollingBallBackground::correctCorners(cv::Mat &pixels, float coeff2, float *cache, int *nextPoint) const
{
  int width            = pixels.cols;
  int height           = pixels.rows;
  auto *corners        = new float[4]{0};    //(0,0); (xmax,0); (ymax,0); (xmax,ymax)
  auto *correctedEdges = new float[2]{0};
  correctedEdges       = lineSlideParabola(pixels, 0, 1, width, coeff2, cache, nextPoint, correctedEdges);
  corners[0]           = correctedEdges[0];
  corners[1]           = correctedEdges[1];
  correctedEdges       = lineSlideParabola(pixels, (height - 1) * width, 1, width, coeff2, cache, nextPoint, correctedEdges);
  corners[2]           = correctedEdges[0];
  corners[3]           = correctedEdges[1];
  correctedEdges       = lineSlideParabola(pixels, 0, width, height, coeff2, cache, nextPoint, correctedEdges);
  corners[0] += correctedEdges[0];
  corners[2] += correctedEdges[1];
  correctedEdges = lineSlideParabola(pixels, width - 1, width, height, coeff2, cache, nextPoint, correctedEdges);
  corners[1] += correctedEdges[0];
  corners[3] += correctedEdges[1];
  int diagLength   = std::min(width, height);    // length of a 45-degree line from a corner
  float coeff2diag = 2 * coeff2;
  correctedEdges   = lineSlideParabola(pixels, 0, 1 + width, diagLength, coeff2diag, cache, nextPoint, correctedEdges);
  corners[0] += correctedEdges[0];
  correctedEdges = lineSlideParabola(pixels, width - 1, -1 + width, diagLength, coeff2diag, cache, nextPoint, correctedEdges);
  corners[1] += correctedEdges[0];
  correctedEdges = lineSlideParabola(pixels, (height - 1) * width, 1 - width, diagLength, coeff2diag, cache, nextPoint, correctedEdges);
  corners[2] += correctedEdges[0];
  correctedEdges = lineSlideParabola(pixels, width * height - 1, -1 - width, diagLength, coeff2diag, cache, nextPoint, correctedEdges);
  corners[3] += correctedEdges[0];
  if(pixels.at<float>(0) > corners[0] / 3) {
    pixels.at<float>(0) = corners[0] / 3;
  }
  if(pixels.at<float>(width - 1) > corners[1] / 3) {
    pixels.at<float>(width - 1) = corners[1] / 3;
  }
  if(pixels.at<float>((height - 1) * width) > corners[2] / 3) {
    pixels.at<float>((height - 1) * width) = corners[2] / 3;
  }
  if(pixels.at<float>(width * height - 1) > corners[3] / 3) {
    pixels.at<float>(width * height - 1) = corners[3] / 3;
  }

  delete[] corners;
  delete[] correctedEdges;
}

}    // namespace joda::cmd
