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
#include "image_math_settings.hpp"

namespace joda::ui::gui {

class ImageMath : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Image math";
  inline static std::string ICON              = "math-operations";
  inline static std::string DESCRIPTION       = "Apply basic mathematical operations on one or two images.";
  inline static std::vector<std::string> TAGS = {"invert", "math", "subtract", "add", "plus", "minus"};

  ImageMath(joda::settings::PipelineStep &pipelineStep, settings::ImageMathSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent,
              {{InOuts::IMAGE, InOuts::BINARY}, {InOuts::OUTPUT_EQUAL_TO_INPUT}}),
      mParent(parent)
  {
    auto *modelTab = addTab(
        "Base", [] {}, false);

    //
    // Options
    //
    mFunction = SettingBase::create<SettingComboBox<joda::settings::ImageMathSettings::Function>>(parent, {}, "Function");
    mFunction->addOptions({
        {.key = joda::settings::ImageMathSettings::Function::INVERT, .label = "Invert", .icon = {}},
        {.key = joda::settings::ImageMathSettings::Function::ADD, .label = "Add", .icon = {}},
        {.key = joda::settings::ImageMathSettings::Function::SUBTRACT, .label = "Subtract", .icon = {}},
        {.key = joda::settings::ImageMathSettings::Function::MULTIPLY, .label = "Multiply", .icon = {}},
        {.key = joda::settings::ImageMathSettings::Function::DIVIDE, .label = "Divide", .icon = {}},
        {.key = joda::settings::ImageMathSettings::Function::AND, .label = "AND", .icon = {}},
        {.key = joda::settings::ImageMathSettings::Function::OR, .label = "OR", .icon = {}},
        {.key = joda::settings::ImageMathSettings::Function::XOR, .label = "XOR", .icon = {}},
        {.key = joda::settings::ImageMathSettings::Function::MIN, .label = "Min", .icon = {}},
        {.key = joda::settings::ImageMathSettings::Function::MAX, .label = "Max", .icon = {}},
        {.key = joda::settings::ImageMathSettings::Function::AVERAGE, .label = "Average", .icon = {}},
        {.key = joda::settings::ImageMathSettings::Function::DIFFERENCE_TYPE, .label = "Difference", .icon = {}},
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

    mOperatorOrder = SettingBase::create<SettingComboBox<joda::settings::ImageMathSettings::OperationOrder>>(parent, {}, "Operation order");
    mOperatorOrder->addOptions({{.key = joda::settings::ImageMathSettings::OperationOrder::AoB, .label = "A o B", .icon = {}},
                                {.key = joda::settings::ImageMathSettings::OperationOrder::BoA, .label = "B o A", .icon = {}}});

    mOperatorOrder->setValue(settings.operatorOrder);
    mOperatorOrder->connectWithSetting(&settings.operatorOrder);

    addSetting(modelTab, "Function", {{mFunction.get(), true, 0}, {mOperatorOrder.get(), false, 0}});

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
    //
    zStackIndex = generateStackIndexCombo(true, "Z-Channel", parent);
    zStackIndex->setValue(settings.inputImageSecond.imagePlane.zStack);
    zStackIndex->connectWithSetting(&settings.inputImageSecond.imagePlane.zStack);

    //
    //
    //
    mMemoryScope = SettingBase::create<SettingComboBox<enums::MemoryScope>>(parent, {}, "Storage scope");
    mMemoryScope->addOptions({{enums::MemoryScope::PIPELINE, "Pipeline"}, {enums::MemoryScope::ITERATION, "Iteration"}});
    mMemoryScope->setValue(settings.memoryScope);
    mMemoryScope->connectWithSetting(&settings.memoryScope);
    mMemoryScope->setShortDescription("");

    //
    //
    //
    mMemoryIdx = SettingBase::create<SettingComboBox<enums::MemoryIdx::Enum>>(parent, {}, "From cache");
    mMemoryIdx->addOptions({{enums::MemoryIdx::NONE, "None"},
                            {enums::MemoryIdx::M0, "M0"},
                            {enums::MemoryIdx::M1, "M1"},
                            {enums::MemoryIdx::M2, "M2"},
                            {enums::MemoryIdx::M3, "M3"},
                            {enums::MemoryIdx::M4, "M4"},
                            {enums::MemoryIdx::M5, "M5"},
                            {enums::MemoryIdx::M6, "M6"},
                            {enums::MemoryIdx::M7, "M7"},
                            {enums::MemoryIdx::M8, "M8"},
                            {enums::MemoryIdx::M9, "M9"},
                            {enums::MemoryIdx::M10, "M10"}});
    mMemoryIdx->setValue(settings.inputImageSecond.memoryId);
    mMemoryIdx->connectWithSetting(&settings.inputImageSecond.memoryId);
    mMemoryIdx->setShortDescription("Cache: ");

    //
    //
    addSetting(modelTab, "Input image channels",
               {{cStackIndex.get(), true, 0},
                {zProjection.get(), true, 0},
                {zStackIndex.get(), false, 0},
                {mMemoryScope.get(), false, 0},
                {mMemoryIdx.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<joda::settings::ImageMathSettings::Function>> mFunction;
  std::unique_ptr<SettingComboBox<joda::settings::ImageMathSettings::OperationOrder>> mOperatorOrder;
  std::unique_ptr<SettingComboBox<int32_t>> cStackIndex;
  std::unique_ptr<SettingComboBox<enums::ZProjection>> zProjection;
  std::unique_ptr<SettingSpinBox<int32_t>> zStackIndex;
  std::shared_ptr<SettingComboBox<enums::MemoryIdx::Enum>> mMemoryIdx;
  std::shared_ptr<SettingComboBox<enums::MemoryScope>> mMemoryScope;

  /////////////////////////////////////////////////////

private slots:
};

}    // namespace joda::ui::gui
