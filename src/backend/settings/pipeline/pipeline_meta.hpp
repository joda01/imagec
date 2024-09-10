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

#include <string>
#include "../setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct PipelineMeta
{
  //
  // User defined name of the channel
  //
  std::string name;

  //
  // Color of the channel
  // [#B91717]
  //
  std::string color = "#B91717";

  //
  // Base 64 encoded PNG icon
  //
  std::string icon;

  void check()
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(PipelineMeta, name, color, icon);
};

};    // namespace joda::settings
