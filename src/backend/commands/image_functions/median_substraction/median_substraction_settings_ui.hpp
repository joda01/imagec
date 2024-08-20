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
#include "median_substraction_settings.hpp"

namespace joda::ui {

class MedianSubtraction : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Median subtraction";
  inline static std::string ICON  = "icons8-baseline-50.png";

  MedianSubtraction(settings::MedianSubtractSettings &settings, QWidget *parent) : Command(parent)
  {
    mMedianBackgroundSubtraction = std::shared_ptr<Setting<int, int>>(
        new Setting<int, int>("icons8-baseline-50.png", "Kernel size", "Median background subtraction", "", -1,
                              {{-1, "Off"},
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
                               {23, "23x23"}},
                              parent, "median_background_subtraction.json"));
    mMedianBackgroundSubtraction->setValue(settings.kernelSize);
    mMedianBackgroundSubtraction->connectWithSetting(&settings.kernelSize, nullptr);

    addSetting(TITLE.data(), ICON.data(), {{mMedianBackgroundSubtraction, true}});
  }

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<Setting<int, int>> mMedianBackgroundSubtraction;
};

}    // namespace joda::ui
