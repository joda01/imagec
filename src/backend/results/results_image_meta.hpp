

#pragma once

#include <cstdint>
#include "backend/image_processing/detection/detection_response.hpp"
#include <nlohmann/json.hpp>

namespace nlohmann {
template <>
struct adl_serializer<std::chrono::system_clock::time_point>
{
  static void to_json(nlohmann::json &j, const std::chrono::system_clock::time_point &tp)
  {
    // Convert time_point to time_t
    std::time_t time = std::chrono::system_clock::to_time_t(tp);

    // Convert time_t to tm structure
    std::tm timeInfo = *std::localtime(&time);

    // Format tm structure into a string
    std::stringstream ss;
    ss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");

    // Assign the string to JSON
    j = ss.str();
  }

  // Convert JSON to std::chrono::system_clock::time_point
  static void from_json(const nlohmann::json &j, std::chrono::system_clock::time_point &tp)
  {
    std::tm tm = {};
    std::istringstream ss(j.get<std::string>());
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
  }
};
}    // namespace nlohmann

namespace joda::results {

///
/// \class      JobMeta
/// \author     Joachim Danmayr
/// \brief      Job information
///
struct JobMeta
{
  std::string swVersion;
  std::string buildTime;
  std::string jobName;
  std::chrono::system_clock::time_point timeStarted;
  std::chrono::system_clock::time_point timeFinished;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(JobMeta, swVersion, buildTime, jobName, timeStarted, timeFinished);
};

///
/// \class      PositionInWell
/// \author     Joachim Danmayr
/// \brief      Position in well
///
struct ImgPositionInWell
{
  int32_t img = -1;
  int32_t x   = -1;
  int32_t y   = -1;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ImgPositionInWell, img, x, y);
};

///
/// \class      ImageMeta
/// \author     Joachim Danmayr
/// \brief      Image meta data
///
struct ImageMeta
{
  std::string imageFileName;
  int64_t height = 0;
  int64_t width  = 0;
  ImgPositionInWell imgPosInWell;    /// Position of the image on the plate

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ImageMeta, height, width, imgPosInWell, imageFileName);
};

///
/// \class      ExperimentMeta
/// \author     Joachim Danmayr
/// \brief      Experiment meta data
///
struct ExperimentMeta
{
  //
  // Used to extract coordinates of a well form the image name
  // Regex with 3 groupings: _((.)([0-9]+))_
  //
  std::string filenameRegex = "_((.)([0-9]+))_([0-9]+)";

  //
  // Matrix of image numbers how the images are ordered in a map.
  // First dimension of the vector are the rows, second the columns
  //
  std::vector<std::vector<int32_t>> wellImageOrder = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};

  auto transformMatrix(int32_t &sizeX, int32_t &sizeY) const -> std::map<int32_t, ImgPositionInWell>;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ExperimentMeta, wellImageOrder, filenameRegex);
};

///
/// \class      PositionInWell
/// \author     Joachim Danmayr
/// \brief      Position in well
///
struct WellPosOnPlate
{
  int32_t x = -1;
  int32_t y = -1;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(WellPosOnPlate, x, y);
};

///
/// \class      GroupMeta
/// \author     Joachim Danmayr
/// \brief      Group meta information
///
struct GroupMeta
{
  std::string name;                 ///< Name of the group
  WellPosOnPlate wellPosOnPlate;    /// Position of the group on the plate

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(GroupMeta, name, wellPosOnPlate);
};

///
/// \class      ChannelMeta
/// \author     Joachim Danmayr
/// \brief      Channel meta information
///
struct ChannelMeta
{
  std::string name;    ///< Name of the channel
  joda::func::ResponseDataValidity valid =
      joda::func::ResponseDataValidity::VALID;    ///< True if the value is valid, else false
  bool invalidateAllObjects = false;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(ChannelMeta, name, valid, invalidateAllObjects);
};

///
/// \class      MeasureChannelMeta
/// \author     Joachim Danmayr
/// \brief      Information about the measurement channel
///
struct MeasureChannelMeta
{
  std::string name;    ///< Name of the channel
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(MeasureChannelMeta, name);
};

///
/// \class      ObjectMeta
/// \author     Joachim Danmayr
/// \brief      Information about the measurement channel
///
struct ObjectMeta
{
  std::string name;    ///< Name for the object
  bool valid;          ///< True if the object is valid, else false
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(ObjectMeta, name, valid);
};

}    // namespace joda::results
