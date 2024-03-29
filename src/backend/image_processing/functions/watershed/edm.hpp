

#include <opencv2/core/mat.hpp>

class Edm
{
public:
  Edm();
  static cv::Mat makeFloatEDM(cv::Mat &ip, int backgroundValue, bool edgesAreBackground);

  static inline int NO_POINT = -1;    // no nearest point in array of nearest points

  static void edmLine(cv::Mat &bPixels, cv::Mat &fPixels, int **pointBufs, int width, int offset, int y,
                      int backgroundValue, int yDist);
  static float minDist2(int *points, int pPrev, int pDiag, int x, int y, int distSqr);
};
