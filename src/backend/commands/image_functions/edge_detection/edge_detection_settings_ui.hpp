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
#include "edge_detection_settings.hpp"

namespace joda::ui::gui {

class EdgeDetection : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Edge detection";
  inline static std::string ICON  = "triangle";

  EdgeDetection(joda::settings::PipelineStep &pipelineStep, settings::EdgeDetectionSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent, {{InOuts::IMAGE, InOuts::BINARY}, {InOuts::IMAGE}})
  {
    //
    //
    mEdgeDetectionMode =
        SettingBase::create<SettingComboBox<settings::EdgeDetectionSettings::Mode>>(parent, generateIcon("triangle"), "Edge detection mode");
    mEdgeDetectionMode->addOptions(
        {{settings::EdgeDetectionSettings::Mode::SOBEL, "Sobel"}, {settings::EdgeDetectionSettings::Mode::CANNY, "Canny"}});
    mEdgeDetectionMode->setValue(settings.mode);
    mEdgeDetectionMode->connectWithSetting(&settings.mode);

    addSetting({{mEdgeDetectionMode.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<settings::EdgeDetectionSettings::Mode>> mEdgeDetectionMode;
};

}    // namespace joda::ui::gui
