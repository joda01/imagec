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

#include <qaction.h>
#include <qwidget.h>
#include <cstdint>
#include "backend/commands/command.hpp"
#include "backend/enums/enums_classes.hpp"
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_base.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/gui/container/setting/setting_combobox_classification_in.hpp"
#include "ui/gui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "fill_holes_settings.hpp"

namespace joda::ui::gui {

class FillHoles : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE       = "Fill holes";
  inline static std::string ICON        = "fill-color";
  inline static std::string DESCRIPTION = "...";

  FillHoles(joda::settings::PipelineStep &pipelineStep, settings::FillHolesSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), ICON.data(), parent, {{InOuts::BINARY}, {InOuts::BINARY}}), mParent(parent)
  {
    auto *modelTab = addTab(
        "Base", [] {}, false);

    //
    // Options
    //
    mFunction = SettingBase::create<SettingComboBox<joda::settings::FillHolesSettings::HierarchyMode>>(parent, {}, "Function");
    mFunction->addOptions({
        {.key = joda::settings::FillHolesSettings::HierarchyMode::OUTER, .label = "Outer", .icon = generateIcon("ampersand")},
        {.key = joda::settings::FillHolesSettings::HierarchyMode::INNER, .label = "Inner", .icon = generateIcon("ampersand")},
        {.key = joda::settings::FillHolesSettings::HierarchyMode::INNER_AND_OUTER, .label = "Inner & Outer", .icon = generateIcon("ampersand")},
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
  std::unique_ptr<SettingComboBox<joda::settings::FillHolesSettings::HierarchyMode>> mFunction;

  /////////////////////////////////////////////////////

private slots:
};

}    // namespace joda::ui::gui
