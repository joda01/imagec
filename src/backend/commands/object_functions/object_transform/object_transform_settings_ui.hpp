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
#include "backend/enums/enums_clusters.hpp"
#include "ui/container/command/command.hpp"
#include "ui/container/setting/setting_base.hpp"
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/container/setting/setting_combobox_classification_in.hpp"
#include "ui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/helper/setting_generator.hpp"
#include "object_transform_settings.hpp"

namespace joda::ui {

class ObjectTransform : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Object transform";
  inline static std::string ICON  = "resize";

  ObjectTransform(joda::settings::PipelineStep &pipelineStep, settings::ObjectTransformSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent, {InOuts::OBJECT, InOuts::OBJECT}), mParent(parent)
  {
    auto *modelTab = addTab("Base", [] {});

    mInput = SettingBase::create<SettingComboBoxClassificationIn>(parent, generateIcon("circle"), "First operand");
    mInput->setValue(settings.inputObject);
    mInput->connectWithSetting(&settings.inputObject);

    //
    // Options
    //
    mFunction = SettingBase::create<SettingComboBox<joda::settings::ObjectTransformSettings::Function>>(parent, {}, "Function");
    mFunction->addOptions({{.key = joda::settings::ObjectTransformSettings::Function::SCALE, .label = "Scale", .icon = generateIcon("ampersand")}});
    mFunction->setValue(settings.function);
    mFunction->connectWithSetting(&settings.function);

    mScaleFactor = SettingBase::create<SettingLineEdit<float>>(parent, generateIcon("diameter"), "Scale factor [0-256]");
    mScaleFactor->setPlaceholderText("[0 - 256]");
    mScaleFactor->setUnit("x");
    mScaleFactor->setMinMax(0, 256);
    mScaleFactor->setValue(settings.scaleFactor);
    mScaleFactor->connectWithSetting(&settings.scaleFactor);
    mScaleFactor->setShortDescription("Scale ");

    addSetting(modelTab, "Input", {{mInput.get(), true, 0}, {mFunction.get(), true, 0}, {mScaleFactor.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<joda::settings::ObjectTransformSettings::Function>> mFunction;
  std::unique_ptr<SettingComboBoxClassificationIn> mInput;
  std::shared_ptr<SettingLineEdit<float>> mScaleFactor;

  /////////////////////////////////////////////////////

private slots:
};

}    // namespace joda::ui
