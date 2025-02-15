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
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "rolling_ball_settings.hpp"

namespace joda::ui::gui {

class RollingBallBackground : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Rolling ball";
  inline static std::string ICON  = "bubble";

  RollingBallBackground(joda::settings::PipelineStep &pipelineStep, settings::RollingBallSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent, {{InOuts::IMAGE}, {InOuts::IMAGE}})
  {
    //
    //
    mBallType = SettingBase::create<SettingComboBox<joda::settings::RollingBallSettings::BallType>>(parent, generateIcon("bubble"), "Ball type");
    mBallType->addOptions(
        {{joda::settings::RollingBallSettings::BallType::BALL, "Ball"}, {joda::settings::RollingBallSettings::BallType::PARABOLOID, "Paraboloid"}});
    mBallType->setValue(settings.ballType);
    mBallType->connectWithSetting(&settings.ballType);

    //
    //
    mBallSize = SettingBase::create<SettingLineEdit<int>>(parent, generateIcon("diameter"), "Ball size [0-256]");
    mBallSize->setPlaceholderText("[0 - 256]");
    mBallSize->setUnit("px");
    mBallSize->setMinMax(0, 256);
    mBallSize->setValue(settings.ballSize);
    mBallSize->connectWithSetting(&settings.ballSize);
    mBallSize->setShortDescription("Size. ");

    addSetting({{mBallType.get(), true, 0}, {mBallSize.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<joda::settings::RollingBallSettings::BallType>> mBallType;
  std::shared_ptr<SettingLineEdit<int>> mBallSize;
};

}    // namespace joda::ui::gui
