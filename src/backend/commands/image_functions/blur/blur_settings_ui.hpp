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
#include "blur_settings.hpp"

namespace joda::ui {

class Blur : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Blur";
  inline static std::string ICON  = "icons8-blur-50.png";

  Blur(settings::BlurSettings &settings, QWidget *parent) : Command(TITLE.data(), ICON.data(), parent)
  {
    //
    //
    mBlurMode =
        SettingBase::create<SettingComboBox<settings::BlurSettings::Mode>>(parent, "icons8-blur-50.png", "Blur mode");
    mBlurMode->addOptions({{settings::BlurSettings::Mode::BLUR_MORE, "Smoothing"},
                           {settings::BlurSettings::Mode::GAUSSIAN, "Gaussian blur"}});
    mBlurMode->setValue(settings.mode);
    mBlurMode->connectWithSetting(&settings.mode);

    //
    //
    //
    mKernelSize = SettingBase::create<SettingComboBox<int32_t>>(parent, "", "Kernel size");
    mKernelSize->addOptions({{-1, "Off"},
                             {3, "3x3"},
                             {4, "4x4"},
                             {5, "5x5"},
                             {7, "7x7"},
                             {9, "9x9"},
                             {11, "11x11"},
                             {13, "13x13"},
                             {15, "15x15"},
                             {17, "17x17"},
                             {19, "19x19"},
                             {21, "21x21"},
                             {23, "23x23"}});
    mKernelSize->setValue(settings.kernelSize);
    mKernelSize->connectWithSetting(&settings.kernelSize);
    mKernelSize->setShortDescription("Kernel: ");

    //
    //
    //
    mRepeat = SettingBase::create<SettingComboBox<int32_t>>(parent, "", "Repeat");
    mRepeat->addOptions({{1, "1x"},
                         {2, "2x"},
                         {3, "3x"},
                         {4, "4x"},
                         {5, "5x"},
                         {6, "6x"},
                         {7, "7x"},
                         {8, "8x"},
                         {9, "9x"},
                         {10, "10x"},
                         {11, "11x"},
                         {12, "12x"},
                         {13, "13x"}});
    mRepeat->setValue(settings.kernelSize);
    mRepeat->connectWithSetting(&settings.repeat);

    addSetting({{mBlurMode.get(), true}, {mKernelSize.get(), true}, {mRepeat.get(), false}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<settings::BlurSettings::Mode>> mBlurMode;
  std::shared_ptr<SettingComboBox<int>> mKernelSize;
  std::shared_ptr<SettingComboBox<int>> mRepeat;
};

}    // namespace joda::ui