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
#include "watershed_settings.hpp"

namespace joda::ui {

class Watershed : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Watershed";
  inline static std::string ICON  = "icons8-split-50";

  Watershed(settings::WatershedSettings &settings, QWidget *parent) : Command(TITLE.data(), ICON.data(), parent)
  {
    mFindTolerance = std::shared_ptr<Setting<float, int>>(new Setting<float, int>("icons8-split-50", "Find tolerance",
                                                                                  "Find tolerance", "%", 0.5,
                                                                                  {
                                                                                      {0, "Off"},
                                                                                      {0.5, "Default (0.5)"},
                                                                                      {0.6, "0.6"},
                                                                                      {0.7, "0.7"},
                                                                                      {0.8, "0.8"},
                                                                                      {0.9, "0.9"},
                                                                                  },
                                                                                  parent, ""));

    mFindTolerance->setValue(settings.maximumFinderTolerance);
    mFindTolerance->connectWithSetting(&settings.maximumFinderTolerance, nullptr);
    addSetting({{mFindTolerance.get(), true}});
  }

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<Setting<float, int>> mFindTolerance;
};

}    // namespace joda::ui
