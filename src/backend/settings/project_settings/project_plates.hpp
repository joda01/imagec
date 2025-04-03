
///
/// \file      project_experiment_setup.hpp
/// \author    Joachim Danmayr
/// \date      2024-04-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///

#pragma once

#include <set>
#include <string>
#include <vector>
#include "backend/enums/enums_grouping.hpp"
#include "backend/settings/project_settings/project_plate_setup.hpp"
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
  // Folder where images for this plate are placed in
  //
  std::string imageFolder;

  //
  // Image grouping option [NONE, FOLDER, FILENAME]
  //
  joda::enums::GroupBy groupBy = enums::GroupBy::OFF;

  //
  // Used to extract coordinates of a well form the image name
  // Regex with two or three groupings: _((.)([0-9]+))_
  //
  std::string filenameRegex = "_((.)([0-9]+))_([0-9]+)";

  //
  // How the plate looks like
  //
  PlateSetup plateSetup;

  void check() const
  {
    CHECK_ERROR(!imageFolder.empty(), "Image folder must not be empty!");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Plate, plateId, name, notes, plateSetup, imageFolder, groupBy, filenameRegex);
};

}    // namespace joda::settings
