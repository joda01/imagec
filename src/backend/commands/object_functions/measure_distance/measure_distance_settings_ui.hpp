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
#include "backend/commands/classification/classifier_filter.hpp"
#include "backend/commands/command.hpp"
#include "backend/enums/enums_classes.hpp"
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_base.hpp"
#include "ui/gui/container/setting/setting_combobox_multi.hpp"
#include "ui/gui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "measure_distance_settings.hpp"

namespace joda::ui::gui {

class MeasureDistance : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Measure distance";
  inline static std::string ICON              = "tool-measure";
  inline static std::string DESCRIPTION       = "Measure the distance of all objects in two object classes.";
  inline static std::vector<std::string> TAGS = {"distance", "measure", "object"};

  MeasureDistance(joda::settings::PipelineStep &pipelineStep, settings::MeasureDistanceSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::OBJECT}, {InOuts::OBJECT}}), mSettings(settings),
      mParent(parent)
  {
    //
    //
    classesIn = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, {}, "Classes in");
    classesIn->setValue(settings.inputClasses);
    classesIn->connectWithSetting(&settings.inputClasses);

    //
    //
    classesInSecond = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, {}, "Classes in");
    classesInSecond->setValue(settings.inputClassesSecond);
    classesInSecond->connectWithSetting(&settings.inputClassesSecond);

    //
    //
    auto *tab = addTab(
        "Input class", [] {}, false);
    addSetting(tab, "Input classes", {{classesIn.get(), true, 0}, {classesInSecond.get(), true, 0}});

    // auto *addClassifier = addActionButton("Add class", "icons8-genealogy");
    //  connect(addClassifier, &QAction::triggered, this, &Classifier::addClassifier);
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBoxMultiClassificationIn> classesIn;
  std::unique_ptr<SettingComboBoxMultiClassificationIn> classesInSecond;

  settings::MeasureDistanceSettings &mSettings;
  QWidget *mParent;
};

}    // namespace joda::ui::gui
