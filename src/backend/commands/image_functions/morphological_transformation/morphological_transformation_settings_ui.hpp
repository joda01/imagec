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
#include "morphological_transformation_settings.hpp"

namespace joda::ui::gui {

class MorphologicalTransform : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Morphological Transform";
  inline static std::string ICON              = "geometric-flowers";
  inline static std::string DESCRIPTION       = "...";
  inline static std::vector<std::string> TAGS = {};

  MorphologicalTransform(joda::settings::PipelineStep &pipelineStep, settings::MorphologicalTransformSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), ICON.data(), parent, {{InOuts::BINARY}, {InOuts::BINARY}}), mParent(parent)
  {
    auto *modelTab = addTab(
        "Base", [] {}, false);

    //
    // Options
    //
    mFunction = SettingBase::create<SettingComboBox<joda::settings::MorphologicalTransformSettings::Function>>(parent, {}, "Function");
    mFunction->addOptions({
        {.key = joda::settings::MorphologicalTransformSettings::Function::UNKNOWN, .label = "Unknown", .icon = generateIcon("ampersand")},
        {.key = joda::settings::MorphologicalTransformSettings::Function::ERODE, .label = "Erode", .icon = generateIcon("ampersand")},
        {.key = joda::settings::MorphologicalTransformSettings::Function::DILATE, .label = "Dilate", .icon = generateIcon("ampersand")},
        {.key = joda::settings::MorphologicalTransformSettings::Function::OPEN, .label = "Open", .icon = generateIcon("ampersand")},
        {.key = joda::settings::MorphologicalTransformSettings::Function::CLOSE, .label = "Close", .icon = generateIcon("ampersand")},
        {.key = joda::settings::MorphologicalTransformSettings::Function::GRADIENT, .label = "Gradient", .icon = generateIcon("ampersand")},
        {.key = joda::settings::MorphologicalTransformSettings::Function::TOPHAT, .label = "Top hat", .icon = generateIcon("ampersand")},
        {.key = joda::settings::MorphologicalTransformSettings::Function::BLACKHAT, .label = "Blak hat", .icon = generateIcon("ampersand")},
        {.key = joda::settings::MorphologicalTransformSettings::Function::HITMISS, .label = "Hitmiss", .icon = generateIcon("ampersand")},
    });

    mFunction->setValue(settings.function);
    mFunction->connectWithSetting(&settings.function);

    //
    // Options
    //
    mShape = SettingBase::create<SettingComboBox<joda::settings::MorphologicalTransformSettings::Shape>>(parent, {}, "Shape");
    mShape->addOptions({
        {.key = joda::settings::MorphologicalTransformSettings::Shape::ELLIPSE, .label = "Ellipse", .icon = generateIcon("oval")},
        {.key = joda::settings::MorphologicalTransformSettings::Shape::RECTANGLE, .label = "Rectangle", .icon = generateIcon("rectangle")},
        {.key = joda::settings::MorphologicalTransformSettings::Shape::CROSS, .label = "Cross", .icon = generateIcon("star")},
    });

    mShape->setValue(settings.shape);
    mShape->connectWithSetting(&settings.shape);

    //
    //
    //
    mKernelSize = SettingBase::create<SettingComboBox<int32_t>>(parent, generateIcon("matrix"), "Kernel size");
    mKernelSize->addOptions({{-1, "Off"},
                             {3, "3x3"},
                             {5, "5x5"},
                             {7, "7x7"},
                             {9, "9x9"},
                             {11, "11x11"},
                             {13, "13x13"},
                             {15, "15x15"},
                             {17, "17x17"},
                             {19, "19x19"},
                             {21, "21x21"},
                             {23, "23x23"}});
    mKernelSize->setValue(settings.kernelSize);
    mKernelSize->connectWithSetting(&settings.kernelSize);
    mKernelSize->setShortDescription("Kernel: ");

    //
    //
    addSetting(modelTab, {{mFunction.get(), true, 0}, {mShape.get(), true, 0}, {mKernelSize.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<joda::settings::MorphologicalTransformSettings::Function>> mFunction;
  std::unique_ptr<SettingComboBox<joda::settings::MorphologicalTransformSettings::Shape>> mShape;
  std::unique_ptr<SettingComboBox<int32_t>> mKernelSize;

  /////////////////////////////////////////////////////

private slots:
};

}    // namespace joda::ui::gui
