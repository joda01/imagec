///
/// \file      random_forest_settings_ui.cpp
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "random_forest_settings_ui.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
RandomForest::RandomForest(joda::settings::PipelineStep &pipelineStep, settings::RandomForestSettings &settings, QWidget *parent) :
    Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::OBJECT}, {InOuts::OBJECT}}), mSettings(settings),
    mParent(parent)
{
  auto *modelTab = addTab(
      "Base", [] {}, false);

  //
  // Base settings
  //
  mClassesIn = SettingBase::create<SettingComboBoxClassificationIn>(parent, {}, "Training classes");
  mClassesIn->setValue(settings.trainingClasses);
  mClassesIn->connectWithSetting(&settings.trainingClasses);

  addSetting(modelTab, "Input", {{mClassesIn.get(), true, 0}});
}
}    // namespace joda::ui::gui
