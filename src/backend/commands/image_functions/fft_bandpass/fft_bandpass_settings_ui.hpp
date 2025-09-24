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

#include <qaction.h>
#include <qwidget.h>
#include <cstdint>
#include "backend/commands/command.hpp"
#include "backend/enums/enums_classes.hpp"
#include "ui/gui/editor/widget_pipeline/widget_command/command.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_base.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox_classes_out.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox_classification_in.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_line_edit.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "fft_bandpass_settings.hpp"

namespace joda::ui::gui {

class FFTBandpass : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "FFT Bandpass";
  inline static std::string ICON              = "paint-bucket";
  inline static std::string DESCRIPTION       = "Bandpass filter";
  inline static std::vector<std::string> TAGS = {"fft", "bandpass", "filter"};

  FFTBandpass(joda::settings::PipelineStep &pipelineStep, settings::FFTBandpassSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::IMAGE}, {InOuts::IMAGE}}), mParent(parent)
  {
    auto *modelTab = addTab(
        "Base", [] {}, false);

    //
    // Options
    //
    mFunction = SettingBase::create<SettingComboBox<joda::settings::FFTBandpassSettings::HierarchyMode>>(parent, {}, "Function");
    mFunction->addOptions({
        {.key = joda::settings::FFTBandpassSettings::HierarchyMode::OUTER, .label = "Outer", .icon = {}},
        {.key = joda::settings::FFTBandpassSettings::HierarchyMode::INNER, .label = "Inner", .icon = {}},
        {.key = joda::settings::FFTBandpassSettings::HierarchyMode::INNER_AND_OUTER, .label = "Inner & Outer", .icon = {}},
    });
    mFunction->setValue(settings.hierarchyMode);
    mFunction->connectWithSetting(&settings.hierarchyMode);

    //
    //
    addSetting(modelTab, {{mFunction.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<joda::settings::FFTBandpassSettings::HierarchyMode>> mFunction;

  /////////////////////////////////////////////////////

private slots:
};

}    // namespace joda::ui::gui
