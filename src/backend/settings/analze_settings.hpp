///
/// \file      analyze_settings.hpp
/// \author    Joachim Danmayr
/// \date      2023-04-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <cstddef>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include "channel/channel_settings.hpp"
#include "vchannel/vchannel_settings.hpp"
#include <catch2/catch_config.hpp>
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>
#include "experiment_settings.hpp"

namespace joda::settings {

using json = nlohmann::json;

class AnalyzeSettings final
{
public:
  AnalyzeSettings()                                   = default;
  AnalyzeSettings(const AnalyzeSettings &)            = delete;
  AnalyzeSettings &operator=(const AnalyzeSettings &) = delete;

  ExperimentSettings options;
  std::vector<ChannelSettings> channels;
  VChannelSettings vChannels;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnalyzeSettings, options, channels, vChannels);
};
}    // namespace joda::settings