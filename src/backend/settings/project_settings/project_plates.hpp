
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
#include <string>
#include <vector>
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct Temp
{
  std::vector<std::vector<int32_t>> order;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Temp, order);
};

inline auto vectorToString(const std::vector<std::vector<int32_t>> &wellOrder) -> std::string
{
  try {
    Temp tm{.order = wellOrder};
    nlohmann::json j = tm;
    j                = j["order"];
    return j.dump();
  } catch(...) {
    return "[[1,2,3,4],[5,6,7,8]]";
  }
}

inline auto stringToVector(const std::string &wellOrder) -> std::vector<std::vector<int32_t>>
{
  try {
    nlohmann::json wellImageOrderJson = nlohmann::json::parse(wellOrder);
    nlohmann::json obj;
    obj["order"] = wellImageOrderJson;
    Temp tm      = nlohmann::json::parse(obj.dump());
    return tm.order;
  } catch(...) {
    return {};
  }
}

struct Plate
{
  enum class GroupBy
  {
    OFF,
    DIRECTORY,
    FILENAME
  };
  //
  // Plate ID
  //
  uint8_t plateId = 0;

  //
  // Plate Name
  //
  std::string name;

  //
  // Plate notes
  //
  std::string notes;

  //
  // Number of rows the plate has
  //
  uint32_t rows = 0;

  //
  // Number of cols the plate has
  //
  uint32_t cols = 0;

  //
  // Folder where images for this plate are placed in
  //
  std::string imageFolder;

  //
  // Matrix of image numbers how the images are ordered in a map.
  // First dimension of the vector are the rows, second the columns
  //
  std::vector<std::vector<int32_t>> wellImageOrder = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};

  //
  // Image grouping option [NONE, FOLDER, FILENAME]
  //
  GroupBy groupBy = GroupBy::OFF;

  //
  // Used to extract coordinates of a well form the image name
  // Regex with two or three groupings: _((.)([0-9]+))_
  //
  std::string filenameRegex = "_((.)([0-9]+))_([0-9]+)";

  void check() const
  {
    CHECK(!imageFolder.empty(), "Image folder must not be empty!");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Plate, plateId, name, notes, rows, cols, imageFolder, groupBy,
                                                       filenameRegex, wellImageOrder);
};

// map TaskState values to JSON as strings
NLOHMANN_JSON_SERIALIZE_ENUM(Plate::GroupBy, {
                                                 {Plate::GroupBy::OFF, "Off"},
                                                 {Plate::GroupBy::DIRECTORY, "Directory"},
                                                 {Plate::GroupBy::FILENAME, "Filename"},
                                             })

}    // namespace joda::settings
