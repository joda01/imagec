#pragma once

#include <cstdint>
#include <list>
#include <set>
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ThresholdAdaptiveSettings : public SettingBase
{
public:
  enum class Methods
  {
    BERNSEN,
    CONTRAST,
    MEAN,
    MEDIAN,
    MID_GRAY,
    NIBLACK,
    OTSU,
    PHANSALKAR,
    SAUVOLA
  };

  struct ThresholdAdaptive
  {
    //
    // Which threshold algorithm should be used
    // [MANUAL, LI, MIN_ERROR, TRIANGLE]
    //
    Methods method = Methods::MEAN;

    //
    // Kernel size
    //
    int32_t kernelSize = 3;

    //
    // Grayscale to assign to
    //
    int32_t pixelClassId = 1;

    //
    // Contrast threshold
    //
    int32_t contrastThreshold = 0;

    //
    // This value is added to the calculated factor
    //
    int32_t thresholdOffset = 0;

    /////////////////////////////////////////////////////
    void check() const
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ThresholdAdaptive, method, contrastThreshold, thresholdOffset, pixelClassId, kernelSize);
  };

  std::list<ThresholdAdaptive> modelClasses;

  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ThresholdAdaptiveSettings, modelClasses);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ThresholdAdaptiveSettings::Methods, {
                                                                     {ThresholdAdaptiveSettings::Methods::BERNSEN, "Bernsen"},
                                                                     {ThresholdAdaptiveSettings::Methods::CONTRAST, "Contrast"},
                                                                     {ThresholdAdaptiveSettings::Methods::MEAN, "Mean"},
                                                                     {ThresholdAdaptiveSettings::Methods::MEDIAN, "Median"},
                                                                     {ThresholdAdaptiveSettings::Methods::MID_GRAY, "MidGray"},
                                                                     {ThresholdAdaptiveSettings::Methods::NIBLACK, "NiBlack"},
                                                                     {ThresholdAdaptiveSettings::Methods::OTSU, "Otsu"},
                                                                     {ThresholdAdaptiveSettings::Methods::PHANSALKAR, "Phansalkar"},
                                                                     {ThresholdAdaptiveSettings::Methods::SAUVOLA, "Sauvola"},
                                                                 })

}    // namespace joda::settings
