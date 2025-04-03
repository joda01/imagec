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
#include "backend/commands/object_functions/colocalization/colocalization_settings.hpp"
#include "backend/enums/enums_classes.hpp"

#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/gui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "colocalization_settings.hpp"

namespace joda::ui::gui {

class Colocalization : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Colocalization";
  inline static std::string ICON              = "venn-diagram";
  inline static std::string DESCRIPTION       = "Calculates the overlapping are of two or more image channels.";
  inline static std::vector<std::string> TAGS = {"colocalization", "object", "coloc"};

  Colocalization(joda::settings::PipelineStep &pipelineStep, settings::ColocalizationSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::OBJECT}, {InOuts::OBJECT}}), mSettings(settings),
      mParent(parent)
  {
    auto *modelTab = addTab(
        "Base", [] {}, false);

    mClassesIn = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, generateIcon("venn-diagram"), "Classes to coloc.");
    mClassesIn->setValue(settings.inputClasses);
    mClassesIn->connectWithSetting(&settings.inputClasses);

    addSetting(modelTab, "Input", {{mClassesIn.get(), true, 0}});

    //
    //
    //
    mClassOutput = SettingBase::create<SettingComboBoxClassesOut>(parent, generateIcon("circle"), "Output class");
    mClassOutput->setValue(settings.outputClass);
    mClassOutput->connectWithSetting(&settings.outputClass);

    //
    //
    mMinIntersection = SettingBase::create<SettingLineEdit<float>>(parent, generateIcon("query-inner-join"), "Min. intersection");
    mMinIntersection->setDefaultValue(0.1);
    mMinIntersection->setPlaceholderText("[0 - 1]");
    mMinIntersection->setUnit("%");
    mMinIntersection->setMinMax(0, 1);
    mMinIntersection->setValue(settings.minIntersection);
    mMinIntersection->connectWithSetting(&settings.minIntersection);
    mMinIntersection->setShortDescription("Cls. ");

    auto *col = addSetting(modelTab, "Output", {{mClassOutput.get(), true, 0}, {mMinIntersection.get(), false, 0}});
  }

private:
  /////////////////////////////////////////////////////
  settings::ColocalizationSettings &mSettings;
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBoxClassesOut> mClassOutput;
  std::unique_ptr<SettingLineEdit<float>> mMinIntersection;
  std::unique_ptr<SettingComboBoxMultiClassificationIn> mClassesIn;

  /////////////////////////////////////////////////////
};

}    // namespace joda::ui::gui
