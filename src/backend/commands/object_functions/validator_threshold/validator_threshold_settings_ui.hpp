///
/// \file      command.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <qwidget.h>
#include <cstdint>
#include <string>
#include "backend/commands/command.hpp"
#include "ui/gui/editor/widget_pipeline/widget_command/command.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "validator_threshold_settings.hpp"

namespace joda::ui::gui {

class ThresholdValidator : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Threshold filter";
  inline static std::string ICON              = "funnel";
  inline static std::string DESCRIPTION       = "Exclude overexposed images from statistics.";
  inline static std::vector<std::string> TAGS = {"filter", "threshold", "overexposed"};

  ThresholdValidator(joda::settings::PipelineStep &pipelineStep, settings::ThresholdValidatorSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::BINARY}, {InOuts::BINARY}})
  {
    //
    //
    mHistThreshold = SettingBase::create<SettingLineEdit<float>>(parent, {}, "Hist. min threshold");
    mHistThreshold->setDefaultValue(0.8F);
    mHistThreshold->setPlaceholderText("[0 - 1]");
    mHistThreshold->setUnit("");
    mHistThreshold->setMinMax(0, std::numeric_limits<float>::max());
    mHistThreshold->setValue(settings.histMinThresholdFilterFactor);
    mHistThreshold->connectWithSetting(&settings.histMinThresholdFilterFactor);
    mHistThreshold->setShortDescription("Factor. ");

    addSetting({{mHistThreshold.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingLineEdit<float>> mHistThreshold;
};

}    // namespace joda::ui::gui
