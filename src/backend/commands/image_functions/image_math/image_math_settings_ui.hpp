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
#include "image_math_settings.hpp"

namespace joda::ui {

class ImageMath : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Image math";
  inline static std::string ICON  = "ratio";

  ImageMath(joda::settings::PipelineStep &pipelineStep, settings::ImageMathSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent, {InOuts::IMAGE, InOuts::IMAGE}), mParent(parent)
  {
    auto *modelTab = addTab("Base", [] {});

    //
    // Options
    //
    mFunction = SettingBase::create<SettingComboBox<joda::settings::ImageMathSettings::Function>>(parent, {}, "Function");
    mFunction->addOptions({
        {.key = joda::settings::ImageMathSettings::Function::INVERT, .label = "Invert", .icon = generateIcon("ampersand")},
        {.key = joda::settings::ImageMathSettings::Function::ADD, .label = "Add", .icon = generateIcon("ampersand")},
        {.key = joda::settings::ImageMathSettings::Function::SUBTRACT, .label = "Subtract", .icon = generateIcon("ampersand")},
        {.key = joda::settings::ImageMathSettings::Function::MULTIPLY, .label = "Multiply", .icon = generateIcon("ampersand")},
        {.key = joda::settings::ImageMathSettings::Function::DIVIDE, .label = "Divide", .icon = generateIcon("ampersand")},
        {.key = joda::settings::ImageMathSettings::Function::AND, .label = "AND", .icon = generateIcon("ampersand")},
        {.key = joda::settings::ImageMathSettings::Function::OR, .label = "OR", .icon = generateIcon("ampersand")},
        {.key = joda::settings::ImageMathSettings::Function::XOR, .label = "XOR", .icon = generateIcon("ampersand")},
        {.key = joda::settings::ImageMathSettings::Function::MIN, .label = "Min", .icon = generateIcon("ampersand")},
        {.key = joda::settings::ImageMathSettings::Function::MAX, .label = "Max", .icon = generateIcon("ampersand")},
        {.key = joda::settings::ImageMathSettings::Function::AVERAGE, .label = "Average", .icon = generateIcon("ampersand")},
        {.key = joda::settings::ImageMathSettings::Function::DIFFERENCE, .label = "Difference", .icon = generateIcon("ampersand")},
    });

    mFunction->setValue(settings.function);
    mFunction->connectWithSetting(&settings.function);
    connect(mFunction.get(), &SettingBase::valueChanged, [this]() {
      if(mFunction->getValue() != joda::settings::ImageMathSettings::Function::INVERT) {
        // mInoutSecond->getEditableWidget()->setVisible(true);
      } else {
        // mInoutSecond->getEditableWidget()->setVisible(false);
      }
    });

    addSetting(modelTab, "Function", {{mFunction.get(), true, 0}});

    //
    //
    cStackIndex = generateCStackCombo<SettingComboBox<int32_t>>("Image channel", parent);
    cStackIndex->setValue(settings.inputImageSecond.imagePlane.cStack);
    cStackIndex->connectWithSetting(&settings.inputImageSecond.imagePlane.cStack);

    //
    //
    zProjection = generateZProjection(true, parent);
    zProjection->setValue(settings.inputImageSecond.zProjection);
    zProjection->connectWithSetting(&settings.inputImageSecond.zProjection);

    //
    //
    addSetting(modelTab, "Input image channels", {{cStackIndex.get(), true, 0}, {zProjection.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<joda::settings::ImageMathSettings::Function>> mFunction;
  std::unique_ptr<SettingComboBox<int32_t>> cStackIndex;
  std::unique_ptr<SettingComboBox<enums::ZProjection>> zProjection;

  /////////////////////////////////////////////////////

private slots:
};

}    // namespace joda::ui
