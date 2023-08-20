///
/// \file      rolling_ball.cpp
/// \author    Joachim Danmayr
/// \date      2023-02-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
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

#include "rolling_ball.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include "par_for.h"

namespace joda::func::img {

///
/// \class      RollingBall
/// \author     Joachim Danmayr
/// \brief      Representation of the rolling ball
///
class RollingBall
{
public:
  char *data;
  int patchwidth;
  int shrinkfactor;

  RollingBall(int radius)
  {
    int arcTrimPer;
    if(radius <= 10) {
      shrinkfactor = 1;
      arcTrimPer   = 12;    // trim 24% in x and y
    } else if(radius <= 30) {
      shrinkfactor = 2;
      arcTrimPer   = 12;    // trim 24% in x and y
    } else if(radius <= 100) {
      shrinkfactor = 4;
      arcTrimPer   = 16;    // trim 32% in x and y
    } else {
      shrinkfactor = 8;
      arcTrimPer   = 20;    // trim 40% in x and y
    }
    buildRollingBall(shrinkfactor, radius, arcTrimPer);
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
  void buildRollingBall(int shrinkfactor, int ballradius, int arcTrimPer)
  {
    int rsquare;                  // rolling ball radius squared
    int xtrim;                    // # of pixels trimmed off each end of ball to make patch
    int xval, yval;               // x,y-values on patch relative to center of rolling ball
    int smallballradius, diam;    // radius and diameter of rolling ball
    int temp;                     // value must be >=0 to take square root
    int halfpatchwidth;           // distance in x or y from center of patch to any edge
    int ballsize;                 // size of rolling ball array

    this->shrinkfactor = shrinkfactor;
    smallballradius    = ballradius / shrinkfactor;
    if(smallballradius < 1)
      smallballradius = 1;
    rsquare        = smallballradius * smallballradius;
    diam           = smallballradius * 2;
    xtrim          = (arcTrimPer * diam) / 100;    // only use a patch of the rolling ball
    patchwidth     = diam - xtrim - xtrim;
    halfpatchwidth = smallballradius - xtrim;
    ballsize       = (patchwidth + 1) * (patchwidth + 1);
    data           = new char[ballsize];

    for(int i = 0; i < ballsize; i++) {
      xval = i % (patchwidth + 1) - halfpatchwidth;
      yval = i / (patchwidth + 1) - halfpatchwidth;
      temp = rsquare - (xval * xval) - (yval * yval);
      if(temp >= 0)
        data[i] = (char) std::round(std::sqrt(temp));
      else
        data[i] = 0;
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
  if(ip.channels() == 3) {
    subtractRGBBackround(ip, radius);
  } else {
    subtractBackround(ip, radius);
  }
}

void RollingBallBackground::subtractRGBBackround(cv::Mat &ip, int ballRadius) const
{
  int width  = ip.cols;
  int height = ip.rows;
  char H[width * height];
  char S[width * height];
  char B[width * height];
  //  ip.getHSB(H, S, B);
  //  ByteProcessor brightness = new ByteProcessor(width, height, B, null);
  //  subtractBackround(brightness, radius);
  //  ip.setHSB(H, S, (byte[]) brightness.getPixels());
}

/** Implements a rolling-ball algorithm for the removal of smooth continuous background
    from a two-dimensional gel image.  It rolls the ball (actually a square patch on the
    top of a sphere) on a low-resolution (by a factor of 'shrinkfactor' times) copy of
    the original image in order to increase speed with little loss in accuracy.  It uses
    interpolation and extrapolation to blow the shrunk image to full size.
*/
void RollingBallBackground::subtractBackround(cv::Mat &ip, int ballRadius) const
{
  if(invert) {
    ip.convertTo(ip, CV_64F);
    cv::invert(ip, ip);
    ip.convertTo(ip, CV_16U);
  }

  RollingBall ball(ballRadius);

  // new ImagePlus("ball", new ByteProcessor(ball.patchwidth+1, ball.patchwidth+1, ball.data, null)).show();
  // ImageProcessor smallImage = ip.resize(ip.cols/ball.shrinkfactor, ip.rows/ball.shrinkfactor);

  auto smallImage = shrinkImage(ip, ball.shrinkfactor);

  // new ImagePlus("small image", smallImage).show();
  std::shared_ptr<cv::Mat> background;

  background = rollBall(ball, ip, smallImage);
  interpolateBackground(background, ball);
  extrapolateBackground(background, ball);

  //  showProgress(0.9);

  // ip.copyBits(background, 0, 0, Blitter.SUBTRACT);
  cv::subtract(ip, *background, ip);

  if(invert) {
    ip.convertTo(ip, CV_64F);
    cv::invert(ip, ip);
    ip.convertTo(ip, CV_16U);
  }
  //  showProgress(1.0);
}

/** 'Rolls' a filtering object over a (shrunken) image in order to find the
    image's smooth continuous background.  For the purpose of explaining this
    algorithm, imagine that the 2D grayscale image has a third (height) dimension
    defined by the intensity value at every point in the image.  The center of
    the filtering object, a patch from the top of a sphere having radius BallRadius,
    is moved along each scan line of the image so that the patch is tangent to the
    image at one or more points with every other point on the patch below the
    corresponding (x,y) point of the image.  Any point either on or below the patch
    during this process is considered part of the background.  Shrinking the image
    before running this procedure is advised due to the fourth-degree complexity
    of the algorithm.
*/
std::shared_ptr<cv::Mat> RollingBallBackground::rollBall(RollingBall &ball, cv::Mat &image,
                                                         std::shared_ptr<cv::Mat> smallImage) const
{
  int halfpatchwidth;       // distance in x or y from patch center to any edge
  int ptsbelowlastpatch;    // number of points we may ignore because they were below last patch
  int xpt2, ypt2;           // current (x,y) point in the patch relative to upper left corner
  int xval, yval;           // location in ball in shrunken image coordinates
  int zdif;                 // difference in z (height) between point on ball and point on image
  int zmin;                 // smallest zdif for ball patch with center at current point
  int zctr;                 // current height of the center of the sphere of which the patch is a part
  int zadd;                 // height of a point on patch relative to the xy-plane of the shrunken image
  int ballpt;               // index to array storing the precomputed ball patch
  int imgpt;                // index to array storing the shrunken image
  int backgrpt;             // index to array storing the calculated background
  int ybackgrpt;            // displacement to current background scan line
  int p1, p2;               // temporary indexes to background, ball, or small image
  int ybackgrinc;           // distance in memory between two shrunken y-points in background
  int smallimagewidth;      // length of a scan line in shrunken image
  int left, right, top, bottom;
  int width                           = image.cols;
  int height                          = image.rows;
  int swidth                          = smallImage->cols;
  int sheight                         = smallImage->rows;
  std::shared_ptr<cv::Mat> background = std::make_shared<cv::Mat>(height, width, image.type());
  int shrinkfactor                    = ball.shrinkfactor;
  int leftroll                        = 0;
  int rightroll                       = width / shrinkfactor - 1;
  int toproll                         = 0;
  int bottomroll                      = height / shrinkfactor - 1;
  left                                = 1;
  right                               = rightroll - leftroll - 1;
  top                                 = 1;
  bottom                              = bottomroll - toproll - 1;
  smallimagewidth                     = swidth;
  int patchwidth                      = ball.patchwidth;
  halfpatchwidth                      = patchwidth / 2;
  ybackgrinc                          = shrinkfactor * width;    // real dist btwn 2 adjacent (dy=1) shrunk pts
  zctr                                = 0;                       // start z-center in the xy-plane

  auto start = std::chrono::steady_clock::now();

  for(int ypt = top; ypt <= (bottom + patchwidth); ypt++) {
    for(int xpt = left; xpt <= (right + patchwidth); xpt++) {    // while patch is tangent to edges or within image...
      // xpt is far right edge of ball patch
      // do we have to move the patch up or down to make it tangent to but not above image?...
      zmin   = 0xffff;    // highest could ever be 255
      ballpt = 0;
      ypt2   = ypt - patchwidth;    // ypt2 is top edge of ball patch
      imgpt  = ypt2 * smallimagewidth + xpt - patchwidth;
      while(ypt2 <= ypt) {
        xpt2 = xpt - patchwidth;    // xpt2 is far left edge of ball patch
        while(xpt2 <= xpt) {        // check every point on ball patch
          // only examine points on
          if((xpt2 >= left) && (xpt2 <= right) && (ypt2 >= top) && (ypt2 <= bottom)) {
            p1   = ballpt;
            p2   = imgpt;
            zdif = (smallImage->at<unsigned short>(p2) & 0xffff) -
                   (zctr + (ball.data[p1] & 0xffff));    // curve - circle points
            if(zdif < zmin)                              // keep most negative, since ball should always be below curve
              zmin = zdif;
          }    // if xpt2,ypt2
          ballpt++;
          xpt2++;
          imgpt++;
        }    // while xpt2
        ypt2++;
        imgpt = imgpt - patchwidth - 1 + smallimagewidth;
      }    // while ypt2
      if(zmin != 0)
        zctr += zmin;    // move ball up or down if we find a new minimum
      if(zmin < 0)
        ptsbelowlastpatch = halfpatchwidth;    // ignore left half of ball patch when dz < 0
      else
        ptsbelowlastpatch = 0;
      // now compare every point on ball with background,  and keep highest number
      yval      = ypt - patchwidth;
      ypt2      = 0;
      ballpt    = 0;
      ybackgrpt = (yval - top + 1) * ybackgrinc;
      while(ypt2 <= patchwidth) {
        xval = xpt - patchwidth + ptsbelowlastpatch;
        xpt2 = ptsbelowlastpatch;
        ballpt += ptsbelowlastpatch;
        backgrpt = ybackgrpt + (xval - left + 1) * shrinkfactor;
        while(xpt2 <= patchwidth) {    // for all the points in the ball patch
          if((xval >= left) && (xval <= right) && (yval >= top) && (yval <= bottom)) {
            p1   = ballpt;
            zadd = zctr + (ball.data[p1] & 0xffff);
            p1   = backgrpt;
            // if (backgrpt>=backgroundpixels.length) backgrpt = 0; //(debug)
            if(zadd > (background->at<unsigned short>(p1) & 0xffff)) {    // keep largest adjustment}
              background->at<unsigned short>(p1) = (unsigned short) zadd;
            }
          }
          ballpt++;
          xval++;
          xpt2++;
          backgrpt += shrinkfactor;    // move to next point in x
        }                              // while xpt2
        yval++;
        ypt2++;
        ybackgrpt += ybackgrinc;    // move to next point in y
      }                             // while ypt2
    }                               // for xpt
  }

  auto end                                      = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "RollBall: " << elapsed_seconds.count() << "s"
            << "\n";

  return background;
}

/** Creates a lower resolution image for ball-rolling. */
std::shared_ptr<cv::Mat> RollingBallBackground::shrinkImage(cv::Mat &ip, int shrinkfactor) const
{
  int width   = ip.cols;
  int height  = ip.rows;
  int swidth  = width / shrinkfactor;
  int sheight = height / shrinkfactor;
  auto ip2    = ip;
  // std::shared_ptr<cv::Mat>ip2 = new cv::Mat(*ip);
  //   ip2.smooth();
  // cv::medianBlur(*ip2, *ip2, 3);
  std::shared_ptr<cv::Mat> smallImage = std::make_shared<cv::Mat>(swidth, sheight, ip.type());
  int xmaskmin, ymaskmin, min, thispixel;
  for(int y = 0; y < sheight; y++) {
    for(int x = 0; x < swidth; x++) {
      xmaskmin = shrinkfactor * x;
      ymaskmin = shrinkfactor * y;
      min      = 65535;
      for(int j = 0; j < shrinkfactor; j++) {
        for(int k = 0; k < shrinkfactor; k++) {
          thispixel = ip2.at<unsigned short>(ymaskmin + k, xmaskmin + j);
          if(thispixel < min)
            min = thispixel;
        }
      }
      smallImage->at<unsigned short>(y, x) = min;    // each point in small image is minimum of its neighborhood
    }
  }
  // new ImagePlus("smallImage", smallImage).show();
  return smallImage;
}

/** Uses bilinear interpolation to find the points in the full-scale background
        given the points from the shrunken image background.  Since the shrunken background
        is found from an image composed of minima (over a sufficiently large mask), it
        is certain that no point in the full-scale interpolated background has a higher
        pixel value than the corresponding point in the original image
    */
void RollingBallBackground::interpolateBackground(std::shared_ptr<cv::Mat> background, RollingBall &ball) const
{
  int hloc, vloc;              // position of current pixel in calculated background
  int vinc;                    // memory offset from current calculated pos to current interpolated pos
  int lastvalue, nextvalue;    // calculated pixel values between which we are interpolating
  int p;                       // pointer to current interpolated pixel value
  int bglastptr, bgnextptr;    // pointers to calculated pixel values between which we are interpolating

  int width        = background->cols;
  int height       = background->rows;
  int shrinkfactor = ball.shrinkfactor;
  int leftroll     = 0;
  int rightroll    = width / shrinkfactor - 1;
  int toproll      = 0;
  int bottomroll   = height / shrinkfactor - 1;
  // auto pixels      = background->ptr();

  vloc = 0;
  for(int j = 1; j <= (bottomroll - toproll - 1); j++) {    // interpolate to find background interior
    hloc = 0;
    vloc += shrinkfactor;
    for(int i = 1; i <= (rightroll - leftroll); i++) {
      hloc += shrinkfactor;
      bgnextptr = vloc * width + hloc;
      bglastptr = bgnextptr - shrinkfactor;
      nextvalue = background->at<unsigned short>(bgnextptr) & 0xffff;
      lastvalue = background->at<unsigned short>(bglastptr) & 0xffff;
      for(int ii = 1; ii <= (shrinkfactor - 1); ii++) {    // interpolate horizontally
        p = bgnextptr - ii;
        background->at<unsigned short>(p) =
            (unsigned short) (lastvalue + (shrinkfactor - ii) * (nextvalue - lastvalue) / shrinkfactor);
      }
      for(int ii = 0; ii <= (shrinkfactor - 1); ii++) {    // interpolate vertically
        bglastptr = (vloc - shrinkfactor) * width + hloc - ii;
        bgnextptr = vloc * width + hloc - ii;
        lastvalue = background->at<unsigned short>(bglastptr) & 0xffff;
        nextvalue = background->at<unsigned short>(bgnextptr) & 0xffff;
        vinc      = 0;
        for(int jj = 1; jj <= (shrinkfactor - 1); jj++) {
          vinc = vinc - width;
          p    = bgnextptr + vinc;
          background->at<unsigned short>(p) =
              (unsigned short) (lastvalue + (shrinkfactor - jj) * (nextvalue - lastvalue) / shrinkfactor);
        }    // for jj
      }      // for ii
    }        // for i
  }          // for j
}

/** Uses linear extrapolation to find pixel values on the top, left, right,
    and bottom edges of the background.  First it finds the top and bottom
    edge points by extrapolating from the edges of the calculated and
    interpolated background interior.  Then it uses the edge points on the
    new calculated, interpolated, and extrapolated background to find all
    of the left and right edge points.  If extrapolation yields values
    below zero or above 255, then they are set to zero and 255 respectively.
*/
void RollingBallBackground::extrapolateBackground(std::shared_ptr<cv::Mat> background, RollingBall &ball) const
{
  int edgeslope;               // difference of last two consecutive pixel values on an edge
  int pvalue;                  // current extrapolated pixel value
  int lastvalue, nextvalue;    // calculated pixel values from which we are extrapolating
  int p;                       // pointer to current extrapolated pixel value
  int bglastptr, bgnextptr;    // pointers to calculated pixel values from which we are extrapolating

  int width        = background->cols;
  int height       = background->rows;
  int shrinkfactor = ball.shrinkfactor;
  int leftroll     = 0;
  int rightroll    = width / shrinkfactor - 1;
  int toproll      = 0;
  int bottomroll   = height / shrinkfactor - 1;
  // auto pixels      = background->ptr();

  for(int hloc = shrinkfactor; hloc <= (shrinkfactor * (rightroll - leftroll) - 1); hloc++) {
    // extrapolate on top and bottom
    bglastptr = shrinkfactor * width + hloc;
    bgnextptr = (shrinkfactor + 1) * width + hloc;
    lastvalue = background->at<unsigned short>(bglastptr) & 0xffff;
    nextvalue = background->at<unsigned short>(bgnextptr) & 0xffff;
    edgeslope = nextvalue - lastvalue;
    p         = bglastptr;
    pvalue    = lastvalue;
    for(int jj = 1; jj <= shrinkfactor; jj++) {
      p      = p - width;
      pvalue = pvalue - edgeslope;
      if(pvalue < 0)
        background->at<unsigned short>(p) = 0;
      else if(pvalue > 0xffff)
        background->at<unsigned short>(p) = (unsigned short) 0xffff;
      else
        background->at<unsigned short>(p) = (unsigned short) pvalue;
    }    // for jj
    bglastptr = (shrinkfactor * (bottomroll - toproll - 1) - 1) * width + hloc;
    bgnextptr = shrinkfactor * (bottomroll - toproll - 1) * width + hloc;
    lastvalue = background->at<unsigned short>(bglastptr) & 0xffff;
    nextvalue = background->at<unsigned short>(bgnextptr) & 0xffff;
    edgeslope = nextvalue - lastvalue;
    p         = bgnextptr;
    pvalue    = nextvalue;
    for(int jj = 1; jj <= ((height - 1) - shrinkfactor * (bottomroll - toproll - 1)); jj++) {
      p += width;
      pvalue += edgeslope;
      if(pvalue < 0)
        background->at<unsigned short>(p) = 0;
      else if(pvalue > 0xffff)
        background->at<unsigned short>(p) = (unsigned short) 0xffff;
      else
        background->at<unsigned short>(p) = (unsigned short) pvalue;
    }    // for jj
  }      // for hloc
  for(int vloc = 0; vloc < height; vloc++) {
    // extrapolate on left and right
    bglastptr = vloc * width + shrinkfactor;
    bgnextptr = bglastptr + 1;
    lastvalue = background->at<unsigned short>(bglastptr) & 0xffff;
    nextvalue = background->at<unsigned short>(bgnextptr) & 0xffff;
    edgeslope = nextvalue - lastvalue;
    p         = bglastptr;
    pvalue    = lastvalue;
    for(int ii = 1; ii <= shrinkfactor; ii++) {
      p--;
      pvalue = pvalue - edgeslope;
      if(pvalue < 0)
        background->at<unsigned short>(p) = 0;
      else if(pvalue > 0xffff)
        background->at<unsigned short>(p) = (unsigned short) 0xffff;
      else
        background->at<unsigned short>(p) = (unsigned short) pvalue;
    }    // for ii
    bgnextptr = vloc * width + shrinkfactor * (rightroll - leftroll - 1) - 1;
    bglastptr = bgnextptr - 1;
    lastvalue = background->at<unsigned short>(bglastptr) & 0xffff;
    nextvalue = background->at<unsigned short>(bgnextptr) & 0xffff;
    edgeslope = nextvalue - lastvalue;
    p         = bgnextptr;
    pvalue    = nextvalue;
    for(int ii = 1; ii <= ((width - 1) - shrinkfactor * (rightroll - leftroll - 1) + 1); ii++) {
      p++;
      pvalue = pvalue + edgeslope;
      if(pvalue < 0)
        background->at<unsigned short>(p) = 0;
      else if(pvalue > 0xffff)
        background->at<unsigned short>(p) = (unsigned short) 0xffff;
      else
        background->at<unsigned short>(p) = (unsigned short) pvalue;
    }    // for ii
  }      // for vloc
}

void RollingBallBackground::setNPasses(int nPasses)
{
  this->nPasses = nPasses;
  pass          = 0;
}
}    // namespace joda::func::img
