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
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "validator_noise_settings.hpp"

namespace joda::ui::gui {

class NoiseValidator : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE       = "Noise filter";
  inline static std::string ICON        = "sort-by-price";
  inline static std::string DESCRIPTION = "...";

  NoiseValidator(joda::settings::PipelineStep &pipelineStep, settings::NoiseValidatorSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), ICON.data(), parent, {{InOuts::OBJECT}, {InOuts::OBJECT}})
  {
    mClassesIn = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, generateIcon("circle"), "Input class");
    mClassesIn->setValue(settings.inputClasses);
    mClassesIn->connectWithSetting(&settings.inputClasses);

    //
    //
    mMaxNrOfObjects = SettingBase::create<SettingLineEdit<uint32_t>>(parent, generateIcon("sort-by-price"), "Max. objects");
    mMaxNrOfObjects->setDefaultValue(100000);
    mMaxNrOfObjects->setPlaceholderText("[0 - " + QString(std::to_string(INT32_MAX).data()) + "]");
    mMaxNrOfObjects->setUnit("");
    mMaxNrOfObjects->setValue(settings.maxObjects);
    mMaxNrOfObjects->connectWithSetting(&settings.maxObjects);
    mMaxNrOfObjects->setShortDescription("Nr. ");

    addSetting({{mClassesIn.get(), true, 0}, {mMaxNrOfObjects.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBoxMultiClassificationIn> mClassesIn;
  std::unique_ptr<SettingLineEdit<uint32_t>> mMaxNrOfObjects;
};

}    // namespace joda::ui::gui
