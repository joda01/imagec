

///
/// \file      edm.hpp
/// \author    Joachim Danmayr
/// \date      2023-02-20
/// \brief     C++ implementation of edm algorithmr ported from imageJ
///            https://github.com/imagej/ImageJ/blob/master/ij/plugin/filter/EDM.java
///

#pragma once

#include <opencv2/core/mat.hpp>

namespace joda::image::func {

class Edm
{
public:
  /////////////////////////////////////////////////////
  static cv::Mat makeFloatEDM(cv::Mat &ip, int backgroundValue, bool edgesAreBackground);
  static void edmLine(cv::Mat &bPixels, cv::Mat &fPixels, int **pointBufs, int width, int offset, int y,
                      int backgroundValue, int yDist);
  static float minDist2(int *points, int pPrev, int pDiag, int x, int y, int distSqr);

private:
  /////////////////////////////////////////////////////
  static inline int NO_POINT = -1;    // no nearest point in array of nearest points
};

}    // namespace joda::image::func
