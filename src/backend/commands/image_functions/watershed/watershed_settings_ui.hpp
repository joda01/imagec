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
#include "watershed_settings.hpp"

namespace joda::ui::gui {

class Watershed : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Watershed";
  inline static std::string ICON              = "split";
  inline static std::string DESCRIPTION       = "Trys to separate clustered objects.";
  inline static std::vector<std::string> TAGS = {"watershed", "object separation", "separation"};

  Watershed(joda::settings::PipelineStep &pipelineStep, settings::WatershedSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::BINARY}, {InOuts::BINARY}})
  {
    mFindTolerance = SettingBase::create<SettingComboBox<float>>(parent, generateIcon("split"), "Find tolerance");
    mFindTolerance->addOptions({
        {0.5, "Default (0.5)"},
        {0.6, "0.6"},
        {0.7, "0.7"},
        {0.8, "0.8"},
        {0.9, "0.9"},
    });
    mFindTolerance->setValue(settings.maximumFinderTolerance);
    mFindTolerance->connectWithSetting(&settings.maximumFinderTolerance);
    mFindTolerance->setUnit("%");

    addSetting({{mFindTolerance.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<float>> mFindTolerance;
};

}    // namespace joda::ui::gui
