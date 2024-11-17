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
#include "objects_to_image_settings.hpp"

namespace joda::ui {

class ObjectsToImage : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Objects to binary image";
  inline static std::string ICON  = "metamorphose";

  ObjectsToImage(joda::settings::PipelineStep &pipelineStep, settings::ObjectsToImageSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent, {{InOuts::OBJECT}, InOuts::BINARY}), mParent(parent)
  {
    auto *modelTab = addTab("Base", [] {});

    mInputFirst = SettingBase::create<SettingComboBoxClassificationIn>(parent, generateIcon("circle"), "First operand");
    mInputFirst->setValue(settings.inputObjectFirst);
    mInputFirst->connectWithSetting(&settings.inputObjectFirst);

    mInoutSecond = SettingBase::create<SettingComboBoxClassificationIn>(parent, generateIcon("circle"), "Second operand");
    mInoutSecond->setValue(settings.inputObjectSecond);
    mInoutSecond->connectWithSetting(&settings.inputObjectSecond);

    //
    // Options
    //
    mFunction = SettingBase::create<SettingComboBox<joda::settings::ObjectsToImageSettings::Function>>(parent, {}, "Function");
    mFunction->addOptions({{.key = joda::settings::ObjectsToImageSettings::Function::NONE, .label = "NONE", .icon = generateIcon("ampersand")},
                           {.key = joda::settings::ObjectsToImageSettings::Function::NOT, .label = "NOT", .icon = generateIcon("ampersand")},
                           {.key = joda::settings::ObjectsToImageSettings::Function::AND, .label = "AND", .icon = generateIcon("ampersand")},
                           {.key = joda::settings::ObjectsToImageSettings::Function::AND_NOT, .label = "AND-NOT", .icon = generateIcon("ampersand")},
                           {.key = joda::settings::ObjectsToImageSettings::Function::OR, .label = "OR", .icon = generateIcon("ampersand")},
                           {.key = joda::settings::ObjectsToImageSettings::Function::XOR, .label = "XOR", .icon = generateIcon("ampersand")}});
    mFunction->setValue(settings.function);
    mFunction->connectWithSetting(&settings.function);
    connect(mFunction.get(), &SettingBase::valueChanged, [this]() {
      if(mFunction->getValue() != joda::settings::ObjectsToImageSettings::Function::NOT &&
         mFunction->getValue() != joda::settings::ObjectsToImageSettings::Function::NONE) {
        mInoutSecond->getEditableWidget()->setVisible(true);
      } else {
        mInoutSecond->getEditableWidget()->setVisible(false);
      }
    });

    addSetting(modelTab, "Input", {{mInputFirst.get(), true, 0}, {mFunction.get(), true, 0}, {mInoutSecond.get(), true, 0}});
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

}    // namespace joda::ui
