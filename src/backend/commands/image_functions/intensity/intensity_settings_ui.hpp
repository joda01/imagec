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
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "intensity_settings.hpp"

namespace joda::ui::gui {

class IntensityTransformation : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Intensity";
  inline static std::string ICON              = "brightness";
  inline static std::string DESCRIPTION       = "...";
  inline static std::vector<std::string> TAGS = {};

  IntensityTransformation(joda::settings::PipelineStep &pipelineStep, settings::IntensityTransformationSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), ICON.data(), parent, {{InOuts::IMAGE}, {InOuts::IMAGE}})
  {
    //
    //
    mIntensityMode =
        SettingBase::create<SettingComboBox<settings::IntensityTransformationSettings::Mode>>(parent, generateIcon("mode"), "Correction mode");
    mIntensityMode->addOptions({{settings::IntensityTransformationSettings::Mode::MANUAL, "Manual"},
                                {settings::IntensityTransformationSettings::Mode::AUTOMATIC, "Automatic"}});
    mIntensityMode->setValue(settings.mode);
    mIntensityMode->connectWithSetting(&settings.mode);

    //
    //
    //
    mContrast = SettingBase::create<SettingLineEdit<float>>(parent, generateIcon("contrast"), "Contrast");
    mContrast->setValue(settings.contrast);
    mContrast->connectWithSetting(&settings.contrast);
    mContrast->setPlaceholderText("[1-3]");
    mContrast->setShortDescription("Contrast: ");

    //
    //
    //
    mBrightness = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("brightness"), "Brightness");
    mBrightness->setValue(settings.brightness);
    mBrightness->connectWithSetting(&settings.brightness);
    mBrightness->setPlaceholderText("[-32768, +32767]");
    mBrightness->setShortDescription("Brightness: ");

    //
    //
    //
    mGamma = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("matrix"), "Gamma");
    mGamma->setValue(settings.gamma);
    mGamma->connectWithSetting(&settings.gamma);
    mGamma->setShortDescription("Gamma: ");

    addSetting({{mIntensityMode.get(), true, 0}, {mContrast.get(), true, 0}, {mBrightness.get(), true, 0}, {mGamma.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<settings::IntensityTransformationSettings::Mode>> mIntensityMode;
  std::shared_ptr<SettingLineEdit<float>> mContrast;
  std::shared_ptr<SettingLineEdit<int32_t>> mBrightness;
  std::shared_ptr<SettingLineEdit<int32_t>> mGamma;
};

}    // namespace joda::ui::gui
