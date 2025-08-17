///
/// \file      pipeline_meta.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
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
  std::string icon = {};

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
  // Optional author
  //
  std::optional<std::string> author;

  //
  // Optional organization
  //
  std::optional<std::string> organization;

  //
  // Optional webpage
  //
  std::optional<std::string> webpage;

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

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(SettingsMeta, name, notes, color, icon, revision, uid, modifiedAt, group, category, tags,
                                                       author, organization, webpage);
};

};    // namespace joda::settings
