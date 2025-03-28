///
/// \file      pipeline_meta.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <optional>
#include <string>
#include <vector>
#include "backend/helper/helper.hpp"
#include "backend/helper/json_optional_parser_helper.hpp"
#include <nlohmann/json.hpp>
#include "setting.hpp"

namespace joda::settings {

struct SettingsMeta
{
  //
  // User defined name of the channel
  //
  std::string name;

  //
  // Some notes in markdown language
  //
  std::string notes;

  //
  // Color of the channel
  // [#B91717]
  //
  std::string color = "#B91717";

  //
  // Base 64 encoded PNG icon
  //
  std::string icon;

  //
  // Revision of this pipeline
  //
  std::string revision;

  //
  // Optional uuid identifying this object unique
  //
  std::string uid;

  //
  // Optional group
  //
  std::optional<std::string> group;

  //
  // Optional category
  //
  std::optional<std::string> category;

  //
  // Optional tags
  //
  std::vector<std::string> tags;

  //
  // Modified at date
  //
  mutable std::optional<std::string> modifiedAt = std::nullopt;

  void check() const
  {
  }

  void setModifiedAtDateToNow() const
  {
    modifiedAt = helper::timepointToIsoString(std::chrono::system_clock::now());
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(SettingsMeta, name, notes, color, icon, revision, uid, modifiedAt, group, category, tags);
};

};    // namespace joda::settings
