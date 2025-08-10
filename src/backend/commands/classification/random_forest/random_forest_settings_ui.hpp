///
/// \file      random_forest_ui.hpp
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

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
#include "random_forest_settings.hpp"

namespace joda::ui::gui {

class RandomForest : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Random forest";
  inline static std::string ICON              = "swap";
  inline static std::string DESCRIPTION       = "Change the classification of objects based on different parameters.";
  inline static std::vector<std::string> TAGS = {"classification", "reclassify", "copy", "move", "incell", "is in"};

  RandomForest(joda::settings::PipelineStep &pipelineStep, settings::RandomForestSettings &settings, QWidget *parent);

private:
  /////////////////////////////////////////////////////
  settings::RandomForestSettings &mSettings;
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBoxClassificationIn> mClassesIn;
};

}    // namespace joda::ui::gui
