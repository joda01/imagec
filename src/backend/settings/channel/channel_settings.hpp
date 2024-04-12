
#pragma once

#include "backend/settings/channel/channel_settings_filter.hpp"
#include "backend/settings/detection/detection_settings.hpp"
#include "backend/settings/preprocessing/functions/margin_crop.hpp"
#include "backend/settings/preprocessing/preprocessing_settings.hpp"
#include <nlohmann/json.hpp>
#include "channel_reporting_settings.hpp"
#include "channel_settings_cross.hpp"
#include "channel_settings_meta.hpp"

namespace joda::settings {

class ChannelSettings
{
public:
  struct Preprocessing
  {
    std::optional<MarginCrop> $cropMargin   = std::nullopt;
    std::optional<ZStackProcessing> $zStack = std::nullopt;
    std::vector<PreprocessingPipelineSteps> pipeline;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Preprocessing, $cropMargin, $zStack, pipeline);
  };

  ChannelSettingsMeta meta;
  DetectionSettings detection;
  ChannelSettingsFilter filter;
  Preprocessing preprocessing;
  CrossChannelSettings crossChannel;
  ChannelReportingSettings reporting;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelSettings, meta, detection, filter, preprocessing, crossChannel,
                                              reporting);
};
}    // namespace joda::settings
