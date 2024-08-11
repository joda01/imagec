
///
/// \file      project_experiment_setup.hpp
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
#include <vector>
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ExperimentSetup
{
  enum class GroupBy
  {
    OFF,
    DIRECTORY,
    FILENAME
  };

  struct Plate
  {
    uint32_t rows = 0;
    uint32_t cols = 0;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Plate, rows, cols);
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
  // Size of the used plate
  //
  Plate plateSize;

  //
  // Matrix of image numbers how the images are ordered in a map.
  // First dimension of the vector are the rows, second the columns
  //
  std::vector<std::vector<int32_t>> wellImageOrder = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ExperimentSetup, filenameRegex, wellImageOrder, plateSize);
};

// map TaskState values to JSON as strings
NLOHMANN_JSON_SERIALIZE_ENUM(ExperimentSetup::GroupBy, {
                                                           {ExperimentSetup::GroupBy::OFF, "Off"},
                                                           {ExperimentSetup::GroupBy::DIRECTORY, "Directory"},
                                                           {ExperimentSetup::GroupBy::FILENAME, "Filename"},
                                                       })

}    // namespace joda::settings
