

#include "edm.hpp"
#include <opencv2/core/hal/interface.h>
#include <opencv2/core.hpp>
#include <opencv2/core/cvstd.hpp>
#include <opencv2/core/mat.hpp>

/**
 * Creates the Euclidian Distance Map of a (binary) uint8_t image.
 * @param ip                The input image, not modified; must be a uint8_tProcessor.
 * @param backgroundValue   Pixels in the input with this value are interpreted as background.
 *                          Note: for pixel value 255, write either -1 or (uint8_t)255.
 * @param edgesAreBackground Whether out-of-image pixels are considered background
 * @return                  The EDM, containing the distances to the nearest background pixel.
 *                          Returns null if the thread is interrupted.
 */

cv::Mat Edm::makeFloatEDM(cv::Mat &ip, int backgroundValue, bool edgesAreBackground)
{
  int width  = ip.cols;
  int height = ip.rows;
  cv::Mat fp = cv::Mat::zeros(height, width, CV_32FC1);
  // uint8_t *bPixels     = (uint8_t[]) ip.getPixels();
  // float[] fPixels      = (float[]) fp.getPixels();

  for(int i = 0; i < width * height; i++) {
    if(ip.at<uint8_t>(i) != backgroundValue) {
      fp.at<float>(i) = std::numeric_limits<float>::max();
    }
  }
  int **pointBufs = new int *[2];    // two buffers for two passes; low short contains x, high short y

  for(int i = 0; i < 2; ++i) {
    pointBufs[i] = new int[width];
  }

  int yDist = std::numeric_limits<int>::max();    // this value is used only if edges are not background
  // pass 1 & 2: increasing y
  for(int x = 0; x < width; x++) {
    pointBufs[0][x] = NO_POINT;
    pointBufs[1][x] = NO_POINT;
  }
  for(int y = 0; y < height; y++) {
    if(edgesAreBackground)
      yDist = y + 1;    // distance to nearest background point (along y)
    edmLine(ip, fp, pointBufs, width, y * width, y, backgroundValue, yDist);
  }
  // pass 3 & 4: decreasing y
  for(int x = 0; x < width; x++) {
    pointBufs[0][x] = NO_POINT;
    pointBufs[1][x] = NO_POINT;
  }
  for(int y = height - 1; y >= 0; y--) {
    if(edgesAreBackground)
      yDist = height - y;
    edmLine(ip, fp, pointBufs, width, y * width, y, backgroundValue, yDist);
  }

  // fp.sqrt();
  cv::sqrt(fp, fp);
  return fp;
}    //  FloatProcessor makeFloatEDM

// Handle a line; two passes: left-to-right and right-to-left

void Edm::edmLine(cv::Mat &bPixels, cv::Mat &fPixels, int **pointBufs, int width, int offset, int y,
                  int backgroundValue, int yDist)
{
  int *points = pointBufs[0];    // the buffer for the left-to-right pass
  int pPrev   = NO_POINT;
  int pDiag   = NO_POINT;    // point at (-/+1, -/+1) to current one (-1,-1 in the first pass)
  int pNextDiag;
  bool edgesAreBackground = yDist != std::numeric_limits<int>::max();
  int distSqr             = std::numeric_limits<int>::max();    // this value is used only if edges are not background
  for(int x = 0; x < width; x++, offset++) {
    pNextDiag = points[x];
    if(bPixels.at<uint8_t>(offset) == backgroundValue) {
      points[x] = x | y << 16;    // remember coordinates as a candidate for nearest background point
    } else {                      // foreground pixel:
      if(edgesAreBackground)
        distSqr = (x + 1 < yDist) ? (x + 1) * (x + 1) : yDist * yDist;    // distance from edge
      float dist2 = minDist2(points, pPrev, pDiag, x, y, distSqr);
      if(fPixels.at<float>(offset) > dist2)
        fPixels.at<float>(offset) = dist2;
    }
    pPrev = points[x];
    pDiag = pNextDiag;
  }
  offset--;                 // now points to the last pixel in the line
  points = pointBufs[1];    // the buffer for the right-to-left pass. Low short contains x, high short y
  pPrev  = NO_POINT;
  pDiag  = NO_POINT;
  for(int x = width - 1; x >= 0; x--, offset--) {
    pNextDiag = points[x];
    if(bPixels.at<uint8_t>(offset) == backgroundValue) {
      points[x] = x | y << 16;    // remember coordinates as a candidate for nearest background point
    } else {                      // foreground pixel:
      if(edgesAreBackground)
        distSqr = (width - x < yDist) ? (width - x) * (width - x) : yDist * yDist;
      float dist2 = minDist2(points, pPrev, pDiag, x, y, distSqr);
      if(fPixels.at<float>(offset) > dist2)
        fPixels.at<float>(offset) = dist2;
    }
    pPrev = points[x];
    pDiag = pNextDiag;
  }
}    // private void edmLine

// Calculates minimum distance^2 of x,y from the following three points:
//  - points[x] (nearest point found for previous line, same x)
//  - pPrev (nearest point found for same line, previous x), and
//  - pDiag (nearest point found for diagonal, i.e., previous line, previous x)
// Sets array element points[x] to the coordinates of the point having the minimum distance to x,y
// If the distSqr parameter is lower than the distance^2, then distSqr is used
// Returns to the minimum distance^2 obtained

float Edm::minDist2(int *points, int pPrev, int pDiag, int x, int y, int distSqr)
{
  int p0           = points[x];    // the nearest background point for the same x in the previous line
  int nearestPoint = p0;
  if(p0 != NO_POINT) {
    int x0       = p0 & 0xffff;
    int y0       = (p0 >> 16) & 0xffff;
    int dist1Sqr = (x - x0) * (x - x0) + (y - y0) * (y - y0);
    if(dist1Sqr < distSqr)
      distSqr = dist1Sqr;
  }
  if(pDiag != p0 && pDiag != NO_POINT) {
    int x1       = pDiag & 0xffff;
    int y1       = (pDiag >> 16) & 0xffff;
    int dist1Sqr = (x - x1) * (x - x1) + (y - y1) * (y - y1);
    if(dist1Sqr < distSqr) {
      nearestPoint = pDiag;
      distSqr      = dist1Sqr;
    }
  }
  if(pPrev != pDiag && pPrev != NO_POINT) {
    int x1       = pPrev & 0xffff;
    int y1       = (pPrev >> 16) & 0xffff;
    int dist1Sqr = (x - x1) * (x - x1) + (y - y1) * (y - y1);
    if(dist1Sqr < distSqr) {
      nearestPoint = pPrev;
      distSqr      = dist1Sqr;
    }
  }
  points[x] = nearestPoint;
  return (float) distSqr;
}    // private float minDist2
