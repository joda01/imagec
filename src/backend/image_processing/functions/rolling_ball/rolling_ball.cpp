///
/// \file      rolling_ball.cpp
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

#include "rolling_ball.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "backend/duration_count/duration_count.h"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::func::img {

///
/// \class      RollingBall
/// \author     Joachim Danmayr
/// \brief      Representation of the rolling ball
///
class RollingBall
{
public:
  float *data      = nullptr;
  int width        = 0;
  int shrinkFactor = 0;

  RollingBall(double radius)
  {
    int arcTrimPer = 0;
    if(radius <= 10) {
      shrinkFactor = 1;
      arcTrimPer   = 24;    // trim 24% in x and y
    } else if(radius <= 30) {
      shrinkFactor = 2;
      arcTrimPer   = 24;    // trim 24% in x and y
    } else if(radius <= 100) {
      shrinkFactor = 4;
      arcTrimPer   = 32;    // trim 32% in x and y
    } else {
      shrinkFactor = 8;
      arcTrimPer   = 40;    // trim 40% in x and y
    }
    buildRollingBall(radius, arcTrimPer);
  }

  ~RollingBall()
  {
    delete[] data;
  }

  ///
  /// \brief        Computes the location of each point on the rolling ball patch relative to the
  ///               center of the sphere containing it.  The patch is located in the top half
  ///               of this sphere.  The vertical axis of the sphere passes through the center of
  ///               the patch.  The projection of the patch in the xy-plane below is a square.
  ///
  /// \author       Joachim Danmayr
  /// \param[in]
  /// \param[in]
  ///
  void buildRollingBall(double ballradius, int arcTrimPer)
  {
    double rsquare;            // rolling ball radius squared
    int xtrim;                 // # of pixels trimmed off each end of ball to make patch
    int xval, yval;            // x,y-values on patch relative to center of rolling ball
    double smallballradius;    // radius of rolling ball (downscaled in x,y and z when image is shrunk)
    int halfWidth;             // distance in x or y from center of patch to any edge (patch "radius")

    smallballradius = ballradius / shrinkFactor;
    if(smallballradius < 1)
      smallballradius = 1;
    rsquare   = smallballradius * smallballradius;
    xtrim     = (int) (double) (arcTrimPer * smallballradius) / 100.0F;    // only use a patch of the rolling ball
    halfWidth = (int) std::round(smallballradius - xtrim);
    width     = 2 * halfWidth + 1;
    data      = new float[width * width];

    for(int y = 0, p = 0; y < width; y++)
      for(int x = 0; x < width; x++, p++) {
        xval        = x - halfWidth;
        yval        = y - halfWidth;
        double temp = rsquare - xval * xval - yval * yval;
        data[p]     = temp > 0. ? (float) (std::sqrt(temp)) : 0.0F;
        //-Float.MAX_VALUE might be better than 0f, but gives different results than earlier versions
      }
  }
};

///
/// \brief              Applies the rolling ball algorithm on the given image
/// \author             Joachim Danmayr
/// \param[in,out]  ip  Image the rolling ball algorithm should be applied on
///                     Result is written back to the same variable.
///
void RollingBallBackground::execute(cv::Mat &ip) const
{
  // Settings
  bool createBackground = false;
  bool doPresmooth      = true;
  bool correctCorners   = true;
  bool invert           = false;

  ///////////////7
  RollingBall *ball = nullptr;
  if(!mUseSlidingParaboloid) {
    ball = new RollingBall(radius);
  }

  cv::Mat fp;
  ip.convertTo(fp, CV_32FC1);
  if(mUseSlidingParaboloid) {
    slidingParaboloidFloatBackground(fp, (float) radius, invert, doPresmooth, correctCorners);
  } else {
    rollingBallFloatBackground(fp, radius, invert, doPresmooth, ball);
  }

  if(createBackground) {
    // ip.setPixels(channelNumber, fp);
  } else {
    // subtract the background now
    float offset = invert ? 65535.5f : 0.5f;    // includes 0.5 for rounding when converting float to short
    for(int p = 0; p < fp.total(); p++) {
      float value = (ip.at<unsigned short>(p) & 0xffff) - fp.at<float>(p) + offset;
      if(value < 0.0f) {
        value = 0.0f;
      }

      if(value > 65535.0F) {
        value = 65535.0f;
      }
      ip.at<uint16_t>(p) = static_cast<uint16_t>(value);
    }
  }
}

/** Create background for a float image by rolling a ball over
 * the image. */
void RollingBallBackground::rollingBallFloatBackground(cv::Mat &fp, float radius, bool invert, bool doPresmooth,
                                                       RollingBall *ball) const
{
  bool shrink = ball->shrinkFactor > 1;
  if(invert) {
    for(int i = 0; i < fp.total(); i++) {
      fp.at<float>(i) = -fp.at<float>(i);
    }
  }
  if(doPresmooth) {
    filter3x3(fp, MEAN);
  }
  cv::Mat smallImage = shrink ? shrinkImage(fp, ball->shrinkFactor) : fp;
  rollBall(ball, smallImage);

  if(shrink) {
    enlargeImage(smallImage, fp, ball->shrinkFactor);
  }

  if(invert) {
    for(int i = 0; i < fp.total(); i++) {
      fp.at<float>(i) = -fp.at<float>(i);
    }
  }
}

cv::Mat RollingBallBackground::shrinkImage(const cv::Mat &ip, int shrinkFactor) const
{
  int width          = ip.cols;
  int height         = ip.rows;
  int sWidth         = ((float) width + (float) shrinkFactor - 1) / (float) shrinkFactor;
  int sHeight        = ((float) height + (float) shrinkFactor - 1) / (float) shrinkFactor;
  cv::Mat smallImage = cv::Mat(sHeight, sWidth, CV_32FC1, cv::Scalar(0));
  float min, thispixel;
  for(int ySmall = 0; ySmall < sHeight; ySmall++) {
    for(int xSmall = 0; xSmall < sWidth; xSmall++) {
      min = std::numeric_limits<float>::max();
      for(int j = 0, y = shrinkFactor * ySmall; j < shrinkFactor && y < height; j++, y++) {
        for(int k = 0, x = shrinkFactor * xSmall; k < shrinkFactor && x < width; k++, x++) {
          thispixel = ip.at<float>(x + y * width);
          if(thispixel < min)
            min = thispixel;
        }
      }
      smallImage.at<float>(xSmall + ySmall * sWidth) =
          min;    // each point in small image is minimum of its neighborhood
    }
  }
  return smallImage;
}

void RollingBallBackground::enlargeImage(const cv::Mat &smallImage, cv::Mat &fp, int shrinkFactor) const
{
  int width       = fp.cols;
  int height      = fp.rows;
  int smallWidth  = smallImage.cols;
  int smallHeight = smallImage.rows;
  int xSmallIndices[width];    // index of first point in smallImage
  float xWeights[width];       // weight of this point
  makeInterpolationArrays(xSmallIndices, xWeights, width, smallWidth, shrinkFactor);
  int ySmallIndices[height];
  float yWeights[height];
  makeInterpolationArrays(ySmallIndices, yWeights, height, smallHeight, shrinkFactor);
  float line0[width];
  float line1[width];
  for(int x = 0; x < width; x++)    // x-interpolation of the first smallImage line
    line1[x] = smallImage.at<float>(xSmallIndices[x]) * xWeights[x] +
               smallImage.at<float>(xSmallIndices[x] + 1) * (1.0f - xWeights[x]);
  int ySmallLine0 = -1;    // line0 corresponds to this y of smallImage
  for(int y = 0; y < height; y++) {
    if(ySmallLine0 < ySmallIndices[y]) {
      float *swap = line0;    // previous line1 -> line0
      memcpy(line0, line1, sizeof(line1));
      memcpy(line1, swap, sizeof(line1));
      ySmallLine0++;
      int sYPointer = (ySmallIndices[y] + 1) * smallWidth;    // points to line0 + 1 in smallImage
      for(int x = 0; x < width; x++)                          // x-interpolation of the new smallImage line -> line1
        line1[x] = smallImage.at<float>(sYPointer + xSmallIndices[x]) * xWeights[x] +
                   smallImage.at<float>(sYPointer + xSmallIndices[x] + 1) * (1.0f - xWeights[x]);
    }
    float weight = yWeights[y];
    for(int x = 0, p = y * width; x < width; x++, p++)
      fp.at<float>(p) = line0[x] * weight + line1[x] * (1.0f - weight);
  }
}

/** Create arrays of indices and weigths for interpolation.
 <pre>
 Example for shrinkFactor = 4:
    small image pixel number         |       0       |       1       |       2       | ...
    full image pixel number          | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |10 |11 | ...
    smallIndex for interpolation(0)  | 0 | 0 | 0 | 0 | 0 | 0 | 1 | 1 | 1 | 1 | 2 | 2 | ...
 (0) Note: This is smallIndex for the left pixel; for the right pixel used for interpolation
           it is higher by one
 </pre>
 */
void RollingBallBackground::makeInterpolationArrays(int *smallIndices, float *weights, int length, int smallLength,
                                                    int shrinkFactor)
{
  for(int i = 0; i < length; i++) {
    int smallIndex = (i - shrinkFactor / 2) / shrinkFactor;
    if(smallIndex >= smallLength - 1)
      smallIndex = smallLength - 2;
    smallIndices[i] = smallIndex;
    float distance =
        (i + 0.5f) / shrinkFactor - (smallIndex + 0.5f);    // distance of pixel centers (in smallImage pixels)
    weights[i] = 1.0f - distance;
  }
}

void RollingBallBackground::rollBall(RollingBall *ball, cv::Mat &fp) const
{
  int width     = fp.cols;
  int height    = fp.rows;
  auto *zBall   = ball->data;
  int ballWidth = ball->width;
  int radius    = ballWidth / 2;
  float cache[width * ballWidth];    // temporarily stores the pixels we work on

  for(int y = -radius; y < height + radius; y++) {    // for all positions of the ball center:
    int nextLineToWriteInCache = (y + radius) % ballWidth;
    int nextLineToRead         = y + radius;    // line of the input not touched yet
    if(nextLineToRead < height) {
      // std::copy(pixels + nextLineToRead * width, pixels + (nextLineToRead + 1) * width,cache + nextLineToWriteInCache
      // * width);

      std::copy(reinterpret_cast<float *>(fp.data) + nextLineToRead * width,
                reinterpret_cast<float *>(fp.data) + (nextLineToRead + 1) * width,
                cache + nextLineToWriteInCache * width);

      for(int x = 0, p = nextLineToRead * width; x < width; x++, p++) {
        fp.at<float>(p) = -std::numeric_limits<float>::max();    // unprocessed pixels start at minus infinity
      }
    }
    int y0 = y - radius;    // the first line to see whether the ball touches
    if(y0 < 0)
      y0 = 0;
    int yBall0 = y0 - y + radius;    // y coordinate in the ball corresponding to y0
    int yend   = y + radius;         // the last line to see whether the ball touches
    if(yend >= height)
      yend = height - 1;
    for(int x = -radius; x < width + radius; x++) {
      float z = std::numeric_limits<float>::max();    // the height of the ball (ball is in position x,y)
      int x0  = x - radius;
      if(x0 < 0)
        x0 = 0;
      int xBall0 = x0 - x + radius;
      int xend   = x + radius;
      if(xend >= width)
        xend = width - 1;
      for(int yp = y0, yBall = yBall0; yp <= yend; yp++, yBall++) {    // for all points inside the ball
        int cachePointer = (yp % ballWidth) * width + x0;
        for(int xp = x0, bp = xBall0 + yBall * ballWidth; xp <= xend; xp++, cachePointer++, bp++) {
          float zReduced = cache[cachePointer] - zBall[bp];
          if(z > zReduced) {    // does this point imply a greater height?
            z = zReduced;
          }
        }
      }
      for(int yp = y0, yBall = yBall0; yp <= yend; yp++, yBall++)    // raise pixels to ball surface
        for(int xp = x0, p = xp + yp * width, bp = xBall0 + yBall * ballWidth; xp <= xend; xp++, p++, bp++) {
          float zMin = z + zBall[bp];
          if(fp.at<float>(p) < zMin)
            fp.at<float>(p) = zMin;
        }
      // if (x>=0&&y>=0&&x<width&&y<height) bgPixels[x+y*width] = z; //debug, ball height output
    }
  }

  // new ImagePlus("bg rolled", fp.duplicate()).show();
}

double RollingBallBackground::filter3x3(cv::Mat &ip, int type) const
{
  int width      = ip.cols;
  int height     = ip.rows;
  double shiftBy = 0;
  for(int y = 0; y < height; y++) {
    shiftBy += filter3(ip, width, y * width, 1, type);
  }
  for(int x = 0; x < width; x++) {
    shiftBy += filter3(ip, height, x, width, type);
  }
  return shiftBy / static_cast<double>(width) / static_cast<double>(height);
}

/** Filter a line: maximum or average of 3-pixel neighborhood */
double RollingBallBackground::filter3(cv::Mat &ip, int length, int pixel0, int inc, int type) const
{
  double shiftBy = 0;
  double v3      = ip.at<float>(pixel0);    // will be pixel[i+1]
  double v2      = v3;                      // will be pixel[i]
  double v1;                                // will be pixel[i-1]
  for(int i = 0, p = pixel0; i < length; i++, p += inc) {
    v1 = v2;
    v2 = v3;
    if(i < length - 1)
      v3 = ip.at<float>(p + inc);
    if(type == MAXIMUM) {
      double max = v1 > v3 ? v1 : v3;
      if(v2 > max) {
        max = v2;
      }
      shiftBy += max - v2;
      ip.at<float>(p) = max;
    } else
      ip.at<float>(p) = (v1 + v2 + v3) * 0.33333333f;
  }
  return shiftBy;
}

}    // namespace joda::func::img
