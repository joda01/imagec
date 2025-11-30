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
#include "ui/gui/editor/widget_pipeline/widget_command/command.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "nop_settings.hpp"

namespace joda::ui::gui {

class Nop : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "NOP";
  inline static std::string ICON              = "hourglass";
  inline static std::string DESCRIPTION       = "No operation";
  inline static std::vector<std::string> TAGS = {"nop"};

  Nop(joda::settings::AnalyzeSettings *analyzeSettings, joda::settings::PipelineStep &pipelineStep, settings::NopSettings &settings,
      QWidget *parent) :
      Command(analyzeSettings, pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent,
              {{InOuts::IMAGE, InOuts::BINARY}, {InOuts::IMAGE}})
  {
    //
    //
    //
    mRepeat = SettingBase::create<SettingComboBox<int32_t>>(parent, {}, "Repeat");
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

    addSetting({{mRepeat.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<SettingComboBox<int>> mRepeat;
};

}    // namespace joda::ui::gui
