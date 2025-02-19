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
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "median_substraction_settings.hpp"

namespace joda::ui::gui {

class MedianSubtraction : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Median subtraction";
  inline static std::string ICON              = "baseline";
  inline static std::string DESCRIPTION       = "Supress image background.";
  inline static std::vector<std::string> TAGS = {"noise reduction", "background subtraction", "noise"};

  MedianSubtraction(joda::settings::PipelineStep &pipelineStep, settings::MedianSubtractSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::IMAGE}, {InOuts::IMAGE}})
  {
    mMedianBackgroundSubtraction = SettingBase::create<SettingComboBox<int32_t>>(parent, generateIcon("baseline"), "Median background subtraction");
    mMedianBackgroundSubtraction->addOptions({{-1, "Off"},
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

    mMedianBackgroundSubtraction->setValue(settings.kernelSize);
    mMedianBackgroundSubtraction->connectWithSetting(&settings.kernelSize);

    addSetting({{mMedianBackgroundSubtraction.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<SettingComboBox<int>> mMedianBackgroundSubtraction;
};

}    // namespace joda::ui::gui
