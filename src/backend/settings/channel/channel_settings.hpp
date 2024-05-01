
#pragma once

#include "backend/settings/channel/channel_settings_filter.hpp"
#include "backend/settings/channel/channel_settings_image_filter.hpp"
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
    std::optional<MarginCrop> $cropMargin = std::nullopt;
    ZStackProcessing $zStack;
    std::vector<PreprocessingPipelineSteps> pipeline;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Preprocessing, $cropMargin, $zStack, pipeline);
  };

  ChannelSettingsMeta meta;
  DetectionSettings detection;
  ChannelSettingsFilter objectFilter;
  ChannelImageFilter imageFilter;
  Preprocessing preprocessing;
  CrossChannelSettings crossChannel;
  ChannelReportingSettings reporting;

  [[nodiscard]] const std::string &schema() const
  {
    return configSchema;
  }

private:
  std::string configSchema = "https://imagec.org/schemas/v1/channel-settings.json";

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelSettings, meta, detection, objectFilter, imageFilter,
                                              preprocessing, crossChannel, reporting, configSchema);
};
}    // namespace joda::settings
