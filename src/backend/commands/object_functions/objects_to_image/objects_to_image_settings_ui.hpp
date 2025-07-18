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
#include "objects_to_image_settings.hpp"

namespace joda::ui::gui {

class ObjectsToImage : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Objects to binary image";
  inline static std::string ICON              = "object-group";
  inline static std::string DESCRIPTION       = "Generates a binary image from a set of objects";
  inline static std::vector<std::string> TAGS = {"binary", "transform", "objects"};

  ObjectsToImage(joda::settings::PipelineStep &pipelineStep, settings::ObjectsToImageSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::OBJECT}, InOuts::BINARY}), mParent(parent)
  {
    auto *modelTab = addTab(
        "Base", [] {}, false);

    mInputFirst = SettingBase::create<SettingComboBoxClassificationIn>(parent, {}, "First operand");
    mInputFirst->setValue(settings.inputClassesFirst);
    mInputFirst->connectWithSetting(&settings.inputClassesFirst);

    mInoutSecond = SettingBase::create<SettingComboBoxClassificationIn>(parent, {}, "Second operand");
    mInoutSecond->setValue(settings.inputClassesSecond);
    mInoutSecond->connectWithSetting(&settings.inputClassesSecond);

    //
    // Options
    //
    mFunction = SettingBase::create<SettingComboBox<joda::settings::ObjectsToImageSettings::Function>>(parent, {}, "Function");
    mFunction->addOptions({{.key = joda::settings::ObjectsToImageSettings::Function::NONE, .label = "NONE", .icon = {}},
                           {.key = joda::settings::ObjectsToImageSettings::Function::NOT, .label = "NOT", .icon = {}},
                           {.key = joda::settings::ObjectsToImageSettings::Function::AND, .label = "AND", .icon = {}},
                           {.key = joda::settings::ObjectsToImageSettings::Function::AND_NOT, .label = "AND-NOT", .icon = {}},
                           {.key = joda::settings::ObjectsToImageSettings::Function::OR, .label = "OR", .icon = {}},
                           {.key = joda::settings::ObjectsToImageSettings::Function::XOR, .label = "XOR", .icon = {}}});
    mFunction->setValue(settings.function);
    mFunction->connectWithSetting(&settings.function);

    addSetting(modelTab, "Input", {{mInputFirst.get(), true, 0}, {mFunction.get(), true, 0}, {mInoutSecond.get(), true, 0}});

    if(mFunction->getValue() != joda::settings::ObjectsToImageSettings::Function::NOT &&
       mFunction->getValue() != joda::settings::ObjectsToImageSettings::Function::NONE) {
      mInoutSecond->getEditableWidget()->setVisible(true);
    } else {
      mInoutSecond->getEditableWidget()->setVisible(false);
    }
    connect(mFunction.get(), &SettingBase::valueChanged, [this]() {
      if(mFunction->getValue() != joda::settings::ObjectsToImageSettings::Function::NOT &&
         mFunction->getValue() != joda::settings::ObjectsToImageSettings::Function::NONE) {
        mInoutSecond->getEditableWidget()->setVisible(true);
      } else {
        mInoutSecond->getEditableWidget()->setVisible(false);
      }
    });
  }

private:
  /////////////////////////////////////////////////////
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<joda::settings::ObjectsToImageSettings::Function>> mFunction;
  std::unique_ptr<SettingComboBoxClassificationIn> mInputFirst;
  std::unique_ptr<SettingComboBoxClassificationIn> mInoutSecond;

  /////////////////////////////////////////////////////

private slots:
};

}    // namespace joda::ui::gui
