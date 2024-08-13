#pragma once

#include <qsettings.h>
#include <cstdint>
#include <set>
#include "backend/enums/enums_clusters.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ProjectImageSetup : public Setting
{
  enum class ZStackHandling
  {
    EXACT_ONE,
    INTENSITY_PROJECTION,
    EACH_ONE
  };

  enum class TStackHandling
  {
    EXACT_ONE,
    EACH_ONE
  };

  enum class CStackHandling
  {
    EXACT_ONE,
    EACH_ONE
  };

  enum class GroupBy
  {
    OFF,
    DIRECTORY,
    FILENAME
  };

  ZStackHandling zStackHandling = ZStackHandling::INTENSITY_PROJECTION;
  TStackHandling tStackHandling = TStackHandling::EACH_ONE;
  CStackHandling cStackHandling = CStackHandling::EXACT_ONE;

  std::string imageInputDirectory;
  std::string resultsOutputFolder;

  //
  // Image grouping option [NONE, FOLDER, FILENAME]
  //
  GroupBy groupBy = GroupBy::OFF;

  //
  // Used to extract coordinates of a well form the image name
  // Regex with 3 groupings: _((.)([0-9]+))_
  //
  std::string filenameRegex = "_((.)([0-9]+))_([0-9]+)";

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ProjectImageSetup, imageInputDirectory, resultsOutputFolder,
                                              zStackHandling, tStackHandling, cStackHandling, groupBy, filenameRegex);

  void check() const override
  {
  }
};

NLOHMANN_JSON_SERIALIZE_ENUM(ProjectImageSetup::ZStackHandling,
                             {{ProjectImageSetup::ZStackHandling::EXACT_ONE, "ExactOne"},
                              {ProjectImageSetup::ZStackHandling::EACH_ONE, "EachOne"},
                              {ProjectImageSetup::ZStackHandling::INTENSITY_PROJECTION, "IntensityProjection"}});

NLOHMANN_JSON_SERIALIZE_ENUM(ProjectImageSetup::TStackHandling,
                             {
                                 {ProjectImageSetup::TStackHandling::EXACT_ONE, "ExactOne"},
                                 {ProjectImageSetup::TStackHandling::EACH_ONE, "EachOne"},
                             });

NLOHMANN_JSON_SERIALIZE_ENUM(ProjectImageSetup::CStackHandling,
                             {
                                 {ProjectImageSetup::CStackHandling::EXACT_ONE, "ExactOne"},
                                 {ProjectImageSetup::CStackHandling::EACH_ONE, "EachOne"},
                             });

// map TaskState values to JSON as strings
NLOHMANN_JSON_SERIALIZE_ENUM(ProjectImageSetup::GroupBy, {
                                                             {ProjectImageSetup::GroupBy::OFF, "Off"},
                                                             {ProjectImageSetup::GroupBy::DIRECTORY, "Directory"},
                                                             {ProjectImageSetup::GroupBy::FILENAME, "Filename"},
                                                         })

}    // namespace joda::settings
