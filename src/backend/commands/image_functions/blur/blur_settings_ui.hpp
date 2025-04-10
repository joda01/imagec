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
#include "backend/commands/command.hpp"
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "blur_settings.hpp"

namespace joda::ui::gui {

class Blur : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Blur";
  inline static std::string ICON              = "blurimage";
  inline static std::string DESCRIPTION       = "Reduce noise within the image";
  inline static std::vector<std::string> TAGS = {"blur", "gaussian", "noise reduction", "noise", "smooth"};

  Blur(joda::settings::PipelineStep &pipelineStep, settings::BlurSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::IMAGE, InOuts::BINARY}, {InOuts::IMAGE}})
  {
    //
    //
    mBlurMode = SettingBase::create<SettingComboBox<settings::BlurSettings::Mode>>(parent, generateIcon("blur"), "Blur mode");
    mBlurMode->addOptions({{settings::BlurSettings::Mode::BLUR_MORE, "Smoothing"}, {settings::BlurSettings::Mode::GAUSSIAN, "Gaussian blur"}});
    mBlurMode->setValue(settings.mode);
    mBlurMode->connectWithSetting(&settings.mode);

    //
    //
    //
    mKernelSize = SettingBase::create<SettingComboBox<int32_t>>(parent, generateIcon("matrix"), "Kernel size");
    mKernelSize->addOptions({{-1, "Off"},
                             {3, "3x3"},
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
    mRepeat = SettingBase::create<SettingComboBox<int32_t>>(parent, generateIcon("repeat"), "Repeat");
    mRepeat->addOptions({{1, "x1"},
                         {2, "x2"},
                         {3, "x3"},
                         {4, "x4"},
                         {5, "x5"},
                         {6, "x6"},
                         {7, "x7"},
                         {8, "x8"},
                         {9, "x9"},
                         {10, "x10"},
                         {11, "x11"},
                         {12, "x12"},
                         {13, "x13"}});
    mRepeat->setValue(settings.repeat);
    mRepeat->connectWithSetting(&settings.repeat);

    addSetting({{mBlurMode.get(), true, 0}, {mKernelSize.get(), true, 0}, {mRepeat.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<settings::BlurSettings::Mode>> mBlurMode;
  std::shared_ptr<SettingComboBox<int>> mKernelSize;
  std::shared_ptr<SettingComboBox<int>> mRepeat;
};

}    // namespace joda::ui::gui
