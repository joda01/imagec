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
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/layout_generator.hpp"
#include "threshold_settings.hpp"

namespace joda::ui {

class Threshold : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Threshold";
  inline static std::string ICON  = "icons8-grayscale-50.png";

  Threshold(settings::ThresholdSettings &settings, QWidget *parent) : Command(TITLE.data(), ICON.data(), parent)
  {
    //
    //
    //
    mThresholdAlgorithm = SettingBase::create<SettingComboBox<joda::settings::ThresholdSettings::Mode>>(
        parent, "icons8-grayscale-50.png", "Threshold algorithm");
    mThresholdAlgorithm->addOptions({{joda::settings::ThresholdSettings::Mode::MANUAL, "Manual"},
                                     {joda::settings::ThresholdSettings::Mode::LI, "Li"},
                                     {joda::settings::ThresholdSettings::Mode::MIN_ERROR, "Min. error"},
                                     {joda::settings::ThresholdSettings::Mode::TRIANGLE, "Triangle"},
                                     {joda::settings::ThresholdSettings::Mode::MOMENTS, "Moments"},
                                     {joda::settings::ThresholdSettings::Mode::OTSU, "Otsu"}});
    mThresholdAlgorithm->setValue(settings.mode);
    mThresholdAlgorithm->connectWithSetting(&settings.mode);

    //
    //
    //
    mThresholdValueMin = SettingBase::create<SettingLineEdit<uint16_t>>(parent, "", "Min. threshold");
    mThresholdValueMin->setPlaceholderText("[0 - 65535]");
    mThresholdValueMin->setUnit("");
    mThresholdValueMin->setMinMax(0, 65535);
    mThresholdValueMin->setValue(settings.thresholdMin);
    mThresholdValueMin->connectWithSetting(&settings.thresholdMin);
    mThresholdValueMin->setShortDescription("Min. ");
    mThresholdValueMin->connectWithSetting(&settings.thresholdMin);

    //
    //
    //
    mThresholdValueMax = SettingBase::create<SettingLineEdit<uint16_t>>(parent, "", "Min. threshold");
    mThresholdValueMax->setPlaceholderText("[0 - 65535]");
    mThresholdValueMax->setUnit("");
    mThresholdValueMax->setMinMax(0, 65535);
    mThresholdValueMax->setValue(settings.thresholdMax);
    mThresholdValueMax->connectWithSetting(&settings.thresholdMax);
    mThresholdValueMax->setShortDescription("Max. ");
    mThresholdValueMax->connectWithSetting(&settings.thresholdMax);

    addSetting({{mThresholdAlgorithm.get(), true}, {mThresholdValueMin.get(), true}, {mThresholdValueMax.get(), true}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<joda::settings::ThresholdSettings::Mode>> mThresholdAlgorithm;
  std::shared_ptr<SettingLineEdit<uint16_t>> mThresholdValueMin;
  std::shared_ptr<SettingLineEdit<uint16_t>> mThresholdValueMax;
};

}    // namespace joda::ui
