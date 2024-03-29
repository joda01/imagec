///
/// \file      maximum_finder.hpp
/// \author    Joachim Danmayr
/// \date      2023-02-20
/// \brief     C++ implementation of maximum finder ported from imageJ
///            https://github.com/imagej/ImageJ/blob/master/ij/plugin/filter/MaximumFinder.java
///

#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

namespace joda::func::img {

using Polygon = std::vector<cv::Point>;

class Wand
{
public:
  explicit Wand(cv::Mat & /*unused*/)
  {
  }
};

class MaximumFinder
{
public:
  /////////////////////////////////////////////////////
  static inline int SINGLE_POINTS   = 0;    ///< Output type all points around the maximum within the tolerance
  static inline int IN_TOLERANCE    = 1;    ///< Output type watershed-segmented image
  static inline int SEGMENTED       = 2;    ///< Do not create image, only mark points
  static inline int POINT_SELECTION = 3;    ///< Do not create an image, just list x, y of maxima in the Results table
  static inline int LIST  = 4;    ///< Do not create an image, just count maxima and add count to Results table
  static inline int COUNT = 5;    ///< what type of output to create (see constants above)
  static inline double NO_THRESHOLD = -808080.0;

  MaximumFinder() = default;
  cv::Mat findMaxima(cv::Mat &ip, double tolerance, bool strict, double threshold, int outputType, bool excludeOnEdges,
                     bool isEDM);

  cv::Mat findMaxima(cv::Mat &ip, double tolerance, int outputType, bool excludeOnEdges);

  cv::Mat findMaxima(cv::Mat &ip, double tolerance, double threshold, int outputType, bool excludeOnEdges, bool isEDM);

private:
  /////////////////////////////////////////////////////
  void makeDirectionOffsets(cv::Mat &ip);
  std::shared_ptr<int64_t> getSortedMaxPoints(cv::Mat &ip, cv::Mat &typeP, bool excludeEdgesNow, bool isEDM,
                                              float globalMin, float globalMax, double threshold, size_t &maxPointSize);

  void analyzeAndMarkMaxima(const cv::Mat &ip, cv::Mat &typeP, std::shared_ptr<int64_t> maxPoints, size_t maxPointsSize,
                            bool excludeEdgesNow, bool isEDM, float globalMin, double tolerance, bool strict,
                            int outputType, float maxSortingError);

  bool isWithin(int x, int y, int direction);
  float trueEdmHeight(int x, int y, const cv::Mat &ip);
  cv::Mat make8bit(cv::Mat &ip, cv::Mat &typeP, bool isEDM, float globalMin, float globalMax, double threshold);
  void cleanupMaxima(cv::Mat &outIp, cv::Mat &typeP, std::shared_ptr<int64_t> maxPoints, size_t maxPointSize);
  bool watershedSegment(cv::Mat &ip);
  void cleanupExtraLines(cv::Mat &ip);
  static void watershedPostProcess(cv::Mat &ip);
  void deleteEdgeParticles(cv::Mat &ip, cv::Mat &typeP);
  void setNPasses(int nPasses);
  Polygon getMaxima(cv::Mat &ip, double tolerance, bool strict, bool excludeOnEdges);
  void removeLineFrom(uint8_t *pixels, int x, int y);
  int nRadii(uint8_t *pixels, int x, int y);
  void deleteParticle(int x, int y, cv::Mat &ip, Wand &wand);
  std::shared_ptr<int> makeFateTable();
  int processLevel(int pass, cv::Mat &ip, std::shared_ptr<int> fateTable, int levelStart, int levelNPoints,
                   std::shared_ptr<int> coordinates, std::shared_ptr<int> setPointList);
  Polygon getMaxima(cv::Mat &ip, double tolerance, bool excludeOnEdges);
  static std::shared_ptr<int> findMaxima(std::shared_ptr<double> xx, size_t xxSize, double tolerance, int edgeMode);
  static std::shared_ptr<int> findMaxima(std::shared_ptr<double> xx, size_t xxSize, double tolerance,
                                         bool excludeOnEdges);
  static std::shared_ptr<int> findMinima(std::shared_ptr<double> xx, size_t xxSize, double tolerance,
                                         bool excludeEdges);
  static std::shared_ptr<int> findMinima(std::shared_ptr<double> xx, size_t xxSize, double tolerance, int edgeMode);

  /// FILTER PARAMS//////////////////////////////////////////////////
  double tolerance =
      10;    ///< prominence: maximum height difference between points that are not counted as separate maxima
  bool strict =
      false;    ///< strict=off allows one maximum even if it is not higher than the prominence above all other pixels
  int outputType;                  ///< Output type single points
  bool excludeOnEdges = false;     ///< whether to exclude maxima at the edge of the image
                                   ///< whether to accept maxima only in the thresholded height range
  bool useMinThreshold = false;    ///< whether to find darkest points on light background
  bool lightBackground = false;
  int width = 0, height = 0;    ///< image dimensions
  int intEncodeXMask = 0;       ///< needed for encoding x & y in a single int (watershed): mask for x
  int intEncodeYMask = 0;       ///< needed for encoding x & y in a single int (watershed): mask for y
  int intEncodeShift = 0;       ///< needed for encoding x & y in a single int (watershed): shift of y

  /** directions to 8 neighboring pixels, clockwise: 0=North (-y), 1=NE, 2=East (+x), ... 7=NW */
  std::shared_ptr<int> dirOffset = nullptr;    // pixel offsets of neighbor pixels for direct addressing
  Polygon xyCoordinates;                       // maxima found by findMaxima() POINT_SELECTION, LIST, COUNT
  static const inline int DIR_X_OFFSET[] = {0, 1, 1, 1, 0, -1, -1, -1};
  static const inline int DIR_Y_OFFSET[] = {-1, -1, 0, 1, 1, 1, 0, -1};

  /// the following constants are used to set bits corresponding to pixel types
  static const inline char MAXIMUM = static_cast<char>(1);    ///< marks local maxima (irrespective of noise tolerance)
  static const inline char LISTED  = static_cast<char>(2);    ///< marks points currently in the list
  static const inline char PROCESSED = static_cast<char>(4);     ///< marks points processed previously
  static const inline char MAX_AREA  = static_cast<char>(8);     ///< marks areas near a maximum, within the tolerance
  static const inline char EQUAL     = static_cast<char>(16);    ///< marks contigous maximum points of equal level
  static const inline char MAX_POINT = static_cast<char>(32);    ///< marks a single point standing for a maximum
  static const inline char ELIMINATED =
      static_cast<char>(64);    // marks maxima that have been eliminated before watershed

  static const inline char outputTypeMasks[] = {MAX_POINT, MAX_AREA,
                                                MAX_AREA};    ///< type masks corresponding to the output types
  static const inline float SQRT2            = 1.4142135624F;
};
}    // namespace joda::func::img
