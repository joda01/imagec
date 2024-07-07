///
/// \file      project_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-04-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

#pragma once

#include <set>
#include <nlohmann/json.hpp>

namespace joda::settings {

class ExperimentSettings
{
public:
  enum class GroupBy
  {
    OFF,
    DIRECTORY,
    FILENAME
  };

  struct Address
  {
    std::string country;
    std::string organization;
    std::string streetAddress;
    std::string postalCode;
    std::string city;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Address, country, organization, streetAddress, postalCode, city);
  };

  //
  // Image grouping option [NONE, FOLDER, FILENAME]
  //
  GroupBy groupBy = GroupBy::OFF;

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

  //
  // Notes to the experiment.
  //
  std::string notes;

  //
  // Names of the scientists doing this analysis.
  //
  std::vector<std::string> scientistsNames;

  //
  // The address of the experiment.
  //
  Address address;

  //
  // This is not stored in the JSON > Working directory
  //
  std::string wotkingDirectory;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ExperimentSettings, groupBy, filenameRegex, wellImageOrder, notes,
                                              scientistsNames, address);
};

// map TaskState values to JSON as strings
NLOHMANN_JSON_SERIALIZE_ENUM(ExperimentSettings::GroupBy, {
                                                              {ExperimentSettings::GroupBy::OFF, "Off"},
                                                              {ExperimentSettings::GroupBy::DIRECTORY, "Directory"},
                                                              {ExperimentSettings::GroupBy::FILENAME, "Filename"},
                                                          })

}    // namespace joda::settings
