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
#include "validator_noise_settings.hpp"

namespace joda::ui {

class NoiseValidator : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Noise filter";
  inline static std::string ICON  = "icons8-sort-by-price-50.png";

  NoiseValidator(settings::NoiseValidatorSettings &settings, QWidget *parent) :
      Command(TITLE.data(), ICON.data(), parent)
  {
    mClassesIn = generateClassDropDown<SettingComboBox<enums::ClassId>>("Input class", parent);
    mClassesIn->setValue(settings.classIn);
    mClassesIn->connectWithSetting(&settings.classIn);

    //
    //
    mMaxNrOfObjects = SettingBase::create<SettingLineEdit<uint32_t>>(parent, "", "Max. objects");
    mMaxNrOfObjects->setDefaultValue(100000);
    mMaxNrOfObjects->setPlaceholderText("[0 - " + QString(std::to_string(UINT32_MAX).data()) + "]");
    mMaxNrOfObjects->setUnit("");
    mMaxNrOfObjects->setMinMax(0, UINT32_MAX);
    mMaxNrOfObjects->setValue(settings.maxObjects);
    mMaxNrOfObjects->connectWithSetting(&settings.maxObjects);
    mMaxNrOfObjects->setShortDescription("Nr. ");

    addSetting({{mClassesIn.get(), true}, {mMaxNrOfObjects.get(), true}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<enums::ClassId>> mClassesIn;
  std::unique_ptr<SettingLineEdit<uint32_t>> mMaxNrOfObjects;
};

}    // namespace joda::ui