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
  inline static std::string ICON              = "distribute-graph";
  inline static std::string DESCRIPTION       = "Simple operations based on the image shape.";
  inline static std::vector<std::string> TAGS = {"transform", "morphological", "erosion",  "erode",   "dilation",  "dilate",
                                                 "open",      "close",         "gradient", "top hat", "black hat", "hitmiss"};

  MorphologicalTransform(joda::settings::PipelineStep &pipelineStep, settings::MorphologicalTransformSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent,
              {{InOuts::IMAGE, InOuts::BINARY}, {InOuts::OUTPUT_EQUAL_TO_INPUT}}),
      mParent(parent)
  {
    auto *modelTab = addTab(
        "Base", [] {}, false);

    //
    // Options
    //
    mFunction = SettingBase::create<SettingComboBox<joda::settings::MorphologicalTransformSettings::Function>>(parent, {}, "Function");
    mFunction->addOptions({
        {.key = joda::settings::MorphologicalTransformSettings::Function::UNKNOWN, .label = "Unknown", .icon = {}},
        {.key = joda::settings::MorphologicalTransformSettings::Function::ERODE, .label = "Erode", .icon = {}},
        {.key = joda::settings::MorphologicalTransformSettings::Function::DILATE, .label = "Dilate", .icon = {}},
        {.key = joda::settings::MorphologicalTransformSettings::Function::OPEN, .label = "Open", .icon = {}},
        {.key = joda::settings::MorphologicalTransformSettings::Function::CLOSE, .label = "Close", .icon = {}},
        {.key = joda::settings::MorphologicalTransformSettings::Function::GRADIENT, .label = "Gradient", .icon = {}},
        {.key = joda::settings::MorphologicalTransformSettings::Function::TOPHAT, .label = "Top hat", .icon = {}},
        {.key = joda::settings::MorphologicalTransformSettings::Function::BLACKHAT, .label = "Blak hat", .icon = {}},
        {.key = joda::settings::MorphologicalTransformSettings::Function::HITMISS, .label = "Hitmiss", .icon = {}},
    });

    mFunction->setValue(settings.function);
    mFunction->connectWithSetting(&settings.function);

    //
    // Options
    //
    mShape = SettingBase::create<SettingComboBox<joda::settings::MorphologicalTransformSettings::Shape>>(parent, {}, "Shape");
    mShape->addOptions({
        {.key = joda::settings::MorphologicalTransformSettings::Shape::ELLIPSE, .label = "Ellipse", .icon = generateSvgIcon("choice-round")},
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
    mIterations = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateSvgIcon("skrooge_type"), "Iterations [1-256]");
    mIterations->setPlaceholderText("[1 - 256]");
    mIterations->setUnit("x");
    mIterations->setMinMax(-1, 256);
    mIterations->setValue(settings.iterations);
    mIterations->connectWithSetting(&settings.iterations);
    mIterations->setShortDescription("Repeat ");

    //
    //
    addSetting(modelTab, {{mFunction.get(), true, 0}, {mShape.get(), true, 0}, {mKernelSize.get(), true, 0}, {mIterations.get(), false, 0}});
  }

private:
  /////////////////////////////////////////////////////
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<joda::settings::MorphologicalTransformSettings::Function>> mFunction;
  std::unique_ptr<SettingComboBox<joda::settings::MorphologicalTransformSettings::Shape>> mShape;
  std::unique_ptr<SettingComboBox<int32_t>> mKernelSize;
  std::unique_ptr<SettingLineEdit<int32_t>> mIterations;

  /////////////////////////////////////////////////////

private slots:
};

}    // namespace joda::ui::gui
