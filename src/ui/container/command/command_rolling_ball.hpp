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
#include "backend/commands/image_functions/rolling_ball/rolling_ball_settings.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "ui/container/setting/setting.hpp"
#include "ui/helper/layout_generator.hpp"

#include "command.hpp"

namespace joda::ui::qt {

class CommandRollingBall : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Rolling ball";
  inline static std::string ICON  = "icons8-bubble-50.png";

  CommandRollingBall(settings::PipelineStep &settings, QWidget *parent) : Command(parent)
  {
    mBallType = std::shared_ptr<Setting<joda::settings::RollingBallSettings::BallType, int32_t>>(
        new Setting<joda::settings::RollingBallSettings::BallType, int32_t>(
            "icons8-bubble-50.png", "", "Ball type", "", joda::settings::RollingBallSettings::BallType::BALL,
            {{joda::settings::RollingBallSettings::BallType::BALL, "Ball"},
             {joda::settings::RollingBallSettings::BallType::PARABOLOID, "Paraboloid"}},
            parent, "rolling_ball.json"));

    mBallSize = std::shared_ptr<Setting<int32_t, int32_t>>(new Setting<int, int32_t>(
        "", "[0 - " + QString::number(INT32_MAX) + "]", "Ball size", "px", std::nullopt, 0, INT32_MAX, parent, ""));

    mBallType->connectWithSetting(&settings.$rollingBall->ballType, nullptr);
    mBallSize->connectWithSetting(&settings.$rollingBall->ballSize, nullptr);

    addSetting(TITLE.data(), ICON.data(), {mBallType, mBallSize});
  }

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<Setting<joda::settings::RollingBallSettings::BallType, int32_t>> mBallType;
  std::shared_ptr<Setting<int32_t, int32_t>> mBallSize;
};

}    // namespace joda::ui::qt
