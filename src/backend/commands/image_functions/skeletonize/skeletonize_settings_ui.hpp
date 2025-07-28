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
#include "skeletonize_settings.hpp"

namespace joda::ui::gui {

class Skeletonize : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Skeletonize";
  inline static std::string ICON              = "snowflake";
  inline static std::string DESCRIPTION       = "Obtains the skeletons from binary images by thinning regions";
  inline static std::vector<std::string> TAGS = {"skeletonize", "skeletons", "thinning", "topological skeleton"};

  Skeletonize(joda::settings::PipelineStep &pipelineStep, settings::SkeletonizeSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::BINARY}, {InOuts::BINARY}})
  {
    //
    //
    mSkeletonizeMode = SettingBase::create<SettingComboBox<settings::SkeletonizeSettings::Mode>>(parent, {}, "Skeletonize mode");
    mSkeletonizeMode->addOptions(
        {{settings::SkeletonizeSettings::Mode::ZHANGSUEN, "Zhangsuen"}, {settings::SkeletonizeSettings::Mode::GUOHALL, "Guohall"}});
    mSkeletonizeMode->setValue(settings.mode);
    mSkeletonizeMode->connectWithSetting(&settings.mode);

    addSetting({{mSkeletonizeMode.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<settings::SkeletonizeSettings::Mode>> mSkeletonizeMode;
};

}    // namespace joda::ui::gui
