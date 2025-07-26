///
/// \file      rank_filter_ui.hpp
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
#include "ui/gui/editor/widget_pipeline/widget_command/command.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "rank_filter_settings.hpp"

namespace joda::ui::gui {

class RankFilter : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Rank filter";
  inline static std::string ICON              = "chart-bar-horizontal";
  inline static std::string DESCRIPTION       = "Each pixel is set to the minimum, maximum or median value of its neighborhood.";
  inline static std::vector<std::string> TAGS = {"rank", "filter", "maximum", "minimum", "median"};

  RankFilter(joda::settings::PipelineStep &pipelineStep, settings::RankFilterSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::IMAGE, InOuts::BINARY}, {InOuts::IMAGE}})
  {
    //
    //
    mMode = SettingBase::create<SettingComboBox<joda::settings::RankFilterSettings::Mode>>(parent, {}, "Mode");
    mMode->addOptions({{joda::settings::RankFilterSettings::Mode::MEAN, "Mean"},
                       {joda::settings::RankFilterSettings::Mode::MEDIAN, "Median"},
                       {joda::settings::RankFilterSettings::Mode::VARIANCE, "Variance"},
                       {joda::settings::RankFilterSettings::Mode::MAX, "Maximum"},
                       {joda::settings::RankFilterSettings::Mode::MIN, "Minimum"}});
    mMode->setValue(settings.mode);
    mMode->connectWithSetting(&settings.mode);

    //
    //
    mRadius = SettingBase::create<SettingLineEdit<float>>(parent, {}, "Radius [0-256]");
    mRadius->setPlaceholderText("[0 - 256]");
    mRadius->setUnit("px");
    mRadius->setMinMax(0, 256);
    mRadius->setValue(settings.radius);
    mRadius->connectWithSetting(&settings.radius);
    mRadius->setShortDescription("Size. ");

    addSetting({{mMode.get(), true, 0}, {mRadius.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<joda::settings::RankFilterSettings::Mode>> mMode;
  std::shared_ptr<SettingLineEdit<float>> mRadius;
};

}    // namespace joda::ui::gui
