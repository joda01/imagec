

#include <cstddef>
#include <vector>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

using Polygon = std::vector<cv::Point>;

class Wand
{
public:
  Wand(cv::Mat &)
  {
  }
};

class MaximumFinder
{
public:
  static inline int SINGLE_POINTS = 0;
  /** Output type all points around the maximum within the tolerance */

  static inline int IN_TOLERANCE = 1;
  /** Output type watershed-segmented image */

  static inline int SEGMENTED = 2;
  /** Do not create image, only mark points */

  static inline int POINT_SELECTION = 3;
  /** Do not create an image, just list x, y of maxima in the Results table */

  static inline int LIST = 4;
  /** Do not create an image, just count maxima and add count to Results table */

  static inline int COUNT = 5;
  /** what type of output to create (see constants above)*/

  static inline double NO_THRESHOLD = -808080.0;

  MaximumFinder() = default;

  cv::Mat findMaxima(cv::Mat &ip, double tolerance, bool strict, double threshold, int outputType, bool excludeOnEdges,
                     bool isEDM);

  cv::Mat findMaxima(cv::Mat &ip, double tolerance, int outputType, bool excludeOnEdges);

  cv::Mat findMaxima(cv::Mat &ip, double tolerance, double threshold, int outputType, bool excludeOnEdges, bool isEDM);

  void makeDirectionOffsets(cv::Mat &ip);
  long *getSortedMaxPoints(cv::Mat &ip, cv::Mat &typeP, bool excludeEdgesNow, bool isEDM, float globalMin,
                           float globalMax, double threshold, size_t &maxPointSize);

  void analyzeAndMarkMaxima(const cv::Mat &ip, cv::Mat &typeP, long *maxPoints, size_t maxPointsSize,
                            bool excludeEdgesNow, bool isEDM, float globalMin, double tolerance, bool strict,
                            int outputType, float maxSortingError);

  bool isWithin(int x, int y, int direction);

  float trueEdmHeight(int x, int y, const cv::Mat &ip);

  cv::Mat make8bit(cv::Mat &ip, cv::Mat &typeP, bool isEDM, float globalMin, float globalMax, double threshold);

  void cleanupMaxima(cv::Mat &outIp, cv::Mat &typeP, long *maxPoints, size_t maxPointSize);
  bool watershedSegment(cv::Mat &ip);
  void cleanupExtraLines(cv::Mat &ip);
  static void watershedPostProcess(cv::Mat &ip);
  void deleteEdgeParticles(cv::Mat &ip, cv::Mat &typeP);
  void setNPasses(int nPasses);
  Polygon getMaxima(cv::Mat &ip, double tolerance, bool strict, bool excludeOnEdges);
  void removeLineFrom(uint8_t *pixels, int x, int y);
  int nRadii(uint8_t *pixels, int x, int y);
  void deleteParticle(int x, int y, cv::Mat &ip, Wand &wand);
  int *makeFateTable();
  int processLevel(int pass, cv::Mat &ip, int *fateTable, int levelStart, int levelNPoints, int *coordinates,
                   int *setPointList);

  Polygon getMaxima(cv::Mat &ip, double tolerance, bool excludeOnEdges);

private:
  static int *findMaxima(double *xx, size_t xxSize, double tolerance, int edgeMode);
  static int *findMaxima(double *xx, size_t xxSize, double tolerance, bool excludeOnEdges);
  static int *findMinima(double *xx, size_t xxSize, double tolerance, bool excludeEdges);
  static int *findMinima(double *xx, size_t xxSize, double tolerance, int edgeMode);

  // filter params
  /** prominence: maximum height difference between points that are not counted as separate maxima */

  static inline double tolerance = 10;
  /** strict=off allows one maximum even if it is not higher than the prominence above all other pixels */

  static inline bool strict = false;
  /** Output type single points */

  static inline int outputType;
  /** what type of output to create was chosen in the dialog (see constants above)*/

  static inline int dialogOutputType = POINT_SELECTION;

  /** whether to exclude maxima at the edge of the image*/

  static inline bool excludeOnEdges;
  /** whether to accept maxima only in the thresholded height range*/

  static inline bool useMinThreshold;
  /** whether to find darkest points on light background */

  static inline bool lightBackground;

  // the following are class variables for having shorter argument lists

  int width, height;    // image dimensions

  int intEncodeXMask;    // needed for encoding x & y in a single int (watershed): mask for x

  int intEncodeYMask;    // needed for encoding x & y in a single int (watershed): mask for y

  int intEncodeShift;    // needed for encoding x & y in a single int (watershed): shift of y
  /** directions to 8 neighboring pixels, clockwise: 0=North (-y), 1=NE, 2=East (+x), ... 7=NW */

  int *dirOffset = nullptr;    // pixel offsets of neighbor pixels for direct addressing

  Polygon xyCoordinates;    // maxima found by findMaxima() POINT_SELECTION, LIST, COUNT
  static inline int DIR_X_OFFSET[] = {0, 1, 1, 1, 0, -1, -1, -1};
  static inline int DIR_Y_OFFSET[] = {-1, -1, 0, 1, 1, 1, 0, -1};
  /** the following constants are used to set bits corresponding to pixel types */
  static inline char MAXIMUM    = (char) 1;     // marks local maxima (irrespective of noise tolerance)
  static inline char LISTED     = (char) 2;     // marks points currently in the list
  static inline char PROCESSED  = (char) 4;     // marks points processed previously
  static inline char MAX_AREA   = (char) 8;     // marks areas near a maximum, within the tolerance
  static inline char EQUAL      = (char) 16;    // marks contigous maximum points of equal level
  static inline char MAX_POINT  = (char) 32;    // marks a single point standing for a maximum
  static inline char ELIMINATED = (char) 64;    // marks maxima that have been eliminated before watershed
                                                /** type masks corresponding to the output types */
  static inline char outputTypeMasks[] = {MAX_POINT, MAX_AREA, MAX_AREA};
  static inline float SQRT2            = 1.4142135624f;
};
