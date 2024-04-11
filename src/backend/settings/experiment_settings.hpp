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

#include <nlohmann/json.hpp>

namespace joda::settings {

class ExperimentSettings
{
  enum class GroupBy
  {
    OFF,
    DIRECTORY,
    FILENAME
  };

  //
  // Image grouping option [NONE, FOLDER, FILENAME]
  //
  GroupBy groupBy = GroupBy::OFF;

  //
  // Used to extract coordinates of a well form the image name
  // Regex with 3 groupings: _((.)([0-9]+))_
  //
  std::string filenameRegex = "_((.)([0-9]+))_";

  //
  // Matrix of image numbers how the images are ordered in a map.
  // First dimension of the vector are the rows, second the columns
  //
  std::vector<std::vector<int32_t>> wellImageOrder;

  // map TaskState values to JSON as strings
  NLOHMANN_JSON_SERIALIZE_ENUM(GroupBy, {
                                            {GroupBy::OFF, "Off"},
                                            {GroupBy::DIRECTORY, "Directory"},
                                            {GroupBy::FILENAME, "Filename"},
                                        })

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ExperimentSettings, groupBy, filenameRegex, wellImageOrder);
};

}    // namespace joda::settings
