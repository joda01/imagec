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
#include <cstdint>
#include <string>
#include "backend/commands/command.hpp"
#include "ui/container/command/command.hpp"
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/helper/setting_generator.hpp"
#include "validator_threshold_settings.hpp"

namespace joda::ui {

class ThresholdValidator : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Threshold filter";
  inline static std::string ICON  = "icons8-filter-50.png";

  ThresholdValidator(joda::settings::PipelineStep &pipelineStep, settings::ThresholdValidatorSettings &settings,
                     QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent)
  {
    //
    //
    mHistThreshold = SettingBase::create<SettingLineEdit<float>>(parent, "", "Hist. min threshold");
    mHistThreshold->setDefaultValue(0.8);
    mHistThreshold->setPlaceholderText("[0 - 1]");
    mHistThreshold->setUnit("");
    mHistThreshold->setMinMax(0, UINT32_MAX);
    mHistThreshold->setValue(settings.histMinThresholdFilterFactor);
    mHistThreshold->connectWithSetting(&settings.histMinThresholdFilterFactor);
    mHistThreshold->setShortDescription("Factor. ");

    addSetting({{mHistThreshold.get(), true}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingLineEdit<float>> mHistThreshold;
};

}    // namespace joda::ui
