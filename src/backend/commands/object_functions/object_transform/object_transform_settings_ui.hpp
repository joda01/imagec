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
#include "object_transform_settings.hpp"

namespace joda::ui::gui {

class ObjectTransform : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Object transform";
  inline static std::string ICON              = "transform-shear-right";
  inline static std::string DESCRIPTION       = "Change the object shape and or size";
  inline static std::vector<std::string> TAGS = {"transform", "scale"};

  ObjectTransform(joda::settings::PipelineStep &pipelineStep, settings::ObjectTransformSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::OBJECT}, {InOuts::OBJECT}}), mParent(parent)
  {
    auto *modelTab = addTab(
        "Base", [] {}, false);

    mInput = SettingBase::create<SettingComboBoxClassificationIn>(parent, {}, "Input class");
    mInput->setValue(settings.inputClasses);
    mInput->connectWithSetting(&settings.inputClasses);

    mOutput = SettingBase::create<SettingComboBoxClassesOut>(parent, {}, "Output class");
    mOutput->setValue(settings.outputClasses);
    mOutput->connectWithSetting(&settings.outputClasses);

    //
    // Options
    //
    mFunction = SettingBase::create<SettingComboBox<joda::settings::ObjectTransformSettings::Function>>(parent, {}, "Function");
    mFunction->addOptions(
        {{.key = joda::settings::ObjectTransformSettings::Function::SCALE, .label = "Scale (factor=scale)", .icon = {}},
         {.key = joda::settings::ObjectTransformSettings::Function::SNAP_AREA, .label = "Snap area (factor=snap area size)", .icon = {}},
         {.key = joda::settings::ObjectTransformSettings::Function::MIN_CIRCLE, .label = "Min. circle (factor=min radius)", .icon = {}},
         {.key = joda::settings::ObjectTransformSettings::Function::EXACT_CIRCLE, .label = "Draw circle (factor=radius)", .icon = {}},
         {.key = joda::settings::ObjectTransformSettings::Function::FIT_ELLIPSE, .label = "Fit ellipse (factor=scale)", .icon = {}}});
    mFunction->setValue(settings.function);
    mFunction->connectWithSetting(&settings.function);

    mScaleFactor = SettingBase::create<SettingLineEdit<float>>(parent, generateSvgIcon("skrooge_type"), "Factor [0-65535]");
    mScaleFactor->setPlaceholderText("[0 - 65535]");
    mScaleFactor->setUnit("x");
    mScaleFactor->setMinMax(0, 65535);
    mScaleFactor->setValue(settings.factor);
    mScaleFactor->connectWithSetting(&settings.factor);
    mScaleFactor->setShortDescription("Scale ");

    addSetting(modelTab, "Input", {{mInput.get(), true, 0}, {mOutput.get(), true, 0}, {mFunction.get(), true, 0}, {mScaleFactor.get(), false, 0}});
  }

private:
  /////////////////////////////////////////////////////
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<joda::settings::ObjectTransformSettings::Function>> mFunction;
  std::unique_ptr<SettingComboBoxClassificationIn> mInput;
  std::unique_ptr<SettingComboBoxClassesOut> mOutput;
  std::shared_ptr<SettingLineEdit<float>> mScaleFactor;

  /////////////////////////////////////////////////////

private slots:
};

}    // namespace joda::ui::gui
