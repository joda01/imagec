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
#include "edge_detection_settings.hpp"

namespace joda::ui {

class EdgeDetection : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Edge detection";
  inline static std::string ICON  = "icons8-triangle-50.png";

  EdgeDetection(settings::EdgeDetectionSettings &settings, QWidget *parent) : Command(TITLE.data(), ICON.data(), parent)
  {
    //
    //
    mEdgeDetectionMode = SettingBase::create<SettingComboBox<settings::EdgeDetectionSettings::Mode>>(
        parent, "icons8-triangle-50.png", "Edge detection mode");
    mEdgeDetectionMode->addOptions({{settings::EdgeDetectionSettings::Mode::OFF, "Off"},
                                    {settings::EdgeDetectionSettings::Mode::SOBEL, "Sobel"},
                                    {settings::EdgeDetectionSettings::Mode::CANNY, "Canny"}});
    mEdgeDetectionMode->setValue(settings.mode);
    mEdgeDetectionMode->connectWithSetting(&settings.mode);

    addSetting({{mEdgeDetectionMode.get(), true}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<settings::EdgeDetectionSettings::Mode>> mEdgeDetectionMode;
};

}    // namespace joda::ui