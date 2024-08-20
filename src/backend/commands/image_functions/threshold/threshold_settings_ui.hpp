///
/// \file      command.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qwidget.h>
#include "backend/commands/command.hpp"
#include "ui/container/command/command.hpp"
#include "ui/container/setting/setting.hpp"
#include "ui/helper/layout_generator.hpp"
#include "threshold_settings.hpp"

namespace joda::ui {

class Threshold : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Threshold";
  inline static std::string ICON  = "icons8-grayscale-50.png";

  Threshold(settings::ThresholdSettings &settings, QWidget *parent) : Command(parent)
  {
    //
    //
    //
    mThresholdAlgorithm = std::shared_ptr<Setting<joda::settings::ThresholdSettings::Mode, int>>(
        new Setting<joda::settings::ThresholdSettings::Mode, int>(
            "icons8-grayscale-50.png", "Threshold", "Threshold algorithm", "",
            joda::settings::ThresholdSettings::Mode::MANUAL,
            {{joda::settings::ThresholdSettings::Mode::MANUAL, "Manual"},
             {joda::settings::ThresholdSettings::Mode::LI, "Li"},
             {joda::settings::ThresholdSettings::Mode::MIN_ERROR, "Min. error"},
             {joda::settings::ThresholdSettings::Mode::TRIANGLE, "Triangle"},
             {joda::settings::ThresholdSettings::Mode::MOMENTS, "Moments"},
             {joda::settings::ThresholdSettings::Mode::OTSU, "Otsu"}},
            parent, "threshold_algorithm.json"));
    mThresholdAlgorithm->setValue(settings.mode);
    mThresholdAlgorithm->connectWithSetting(&settings.mode, nullptr);

    //
    //
    //
    mThresholdValueMin = std::shared_ptr<Setting<uint16_t, uint16_t>>(
        new Setting<uint16_t, uint16_t>("", "[0 - 65535]", "Min. threshold", "min.", 1000, 0, 65535, parent, ""));
    mThresholdValueMin->setValue(settings.thresholdMin);
    mThresholdValueMin->connectWithSetting(&settings.thresholdMin, nullptr);

    //
    //
    //
    mThresholdValueMax = std::shared_ptr<Setting<uint16_t, uint16_t>>(
        new Setting<uint16_t, uint16_t>("", "[0 - 65535]", "Max. threshold", "max.", 65535, 0, 65535, parent, ""));
    mThresholdValueMax->setValue(settings.thresholdMax);
    mThresholdValueMax->connectWithSetting(&settings.thresholdMax, nullptr);

    addSetting(TITLE.data(), ICON.data(),
               {{mThresholdAlgorithm, true}, {mThresholdValueMin, true}, {mThresholdValueMax, false}});
  }

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<Setting<joda::settings::ThresholdSettings::Mode, int>> mThresholdAlgorithm;
  std::shared_ptr<Setting<uint16_t, uint16_t>> mThresholdValueMin;
  std::shared_ptr<Setting<uint16_t, uint16_t>> mThresholdValueMax;
};

}    // namespace joda::ui
