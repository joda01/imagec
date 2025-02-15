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
#include <string>
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
#include "hough_transform_settings.hpp"

namespace joda::ui::gui {

class HoughTransform : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Hough transform";
  inline static std::string ICON  = "trigonometry";

  HoughTransform(joda::settings::PipelineStep &pipelineStep, settings::HoughTransformSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent, {{InOuts::BINARY}, {InOuts::OBJECT}}), mParent(parent)
  {
    auto *modelTab = addTab(
        "Base", [] {}, false);

    //
    // Options
    //
    mFunction = SettingBase::create<SettingComboBox<joda::settings::HoughTransformSettings::HughMode>>(parent, {}, "Mode");
    mFunction->addOptions({
        {.key = joda::settings::HoughTransformSettings::HughMode::LINE_TRANSFORM, .label = "Line", .icon = generateIcon("line")},
        {.key = joda::settings::HoughTransformSettings::HughMode::CIRCLE_TRANSFORM, .label = "Circle", .icon = generateIcon("circle")},
    });

    mFunction->setValue(settings.mode);
    mFunction->connectWithSetting(&settings.mode);

    //
    //
    //
    mMinCircleDistance = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("light-min"), "Min. circle distance");
    mMinCircleDistance->setPlaceholderText("[0 - " + QString(std::to_string(INT32_MAX).data()) + "]");
    mMinCircleDistance->setUnit("");
    mMinCircleDistance->setMinMax(0, INT32_MAX);
    mMinCircleDistance->setValue(settings.minCircleDistance);
    mMinCircleDistance->connectWithSetting(&settings.minCircleDistance);
    mMinCircleDistance->setShortDescription("Min. ");

    //
    //
    //
    mMinCircleRadius = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("light-min"), "Min. circle radius");
    mMinCircleRadius->setPlaceholderText("[0 - " + QString(std::to_string(INT32_MAX).data()) + "]");
    mMinCircleRadius->setUnit("");
    mMinCircleRadius->setMinMax(0, INT32_MAX);
    mMinCircleRadius->setValue(settings.minCircleRadius);
    mMinCircleRadius->connectWithSetting(&settings.minCircleRadius);
    mMinCircleRadius->setShortDescription("Min. ");

    //
    //
    //
    mMaxCircleRadius = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("light-min"), "Max. circle radius");
    mMaxCircleRadius->setPlaceholderText("[0 - " + QString(std::to_string(INT32_MAX).data()) + "]");
    mMaxCircleRadius->setUnit("");
    mMaxCircleRadius->setMinMax(0, INT32_MAX);
    mMaxCircleRadius->setValue(settings.maxCircleRadius);
    mMaxCircleRadius->connectWithSetting(&settings.maxCircleRadius);
    mMaxCircleRadius->setShortDescription("Min. ");

    //
    //
    //
    mParam1 = SettingBase::create<SettingLineEdit<float>>(parent, generateIcon("light-min"), "Param 01");
    mParam1->setPlaceholderText("[0 - " + QString(std::to_string(INT32_MAX).data()) + "]");
    mParam1->setUnit("");
    mParam1->setMinMax(0, INT32_MAX);
    mParam1->setValue(settings.param01);
    mParam1->connectWithSetting(&settings.param01);
    mParam1->setShortDescription("Min. ");

    //
    //
    //
    mParam2 = SettingBase::create<SettingLineEdit<float>>(parent, generateIcon("light-min"), "Param 02");
    mParam2->setPlaceholderText("[0 - " + QString(std::to_string(INT32_MAX).data()) + "]");
    mParam2->setUnit("");
    mParam2->setMinMax(0, INT32_MAX);
    mParam2->setValue(settings.param02);
    mParam2->connectWithSetting(&settings.param02);
    mParam2->setShortDescription("Min. ");

    //
    //
    //
    mClassOut = SettingBase::create<SettingComboBoxClassesOut>(parent, generateIcon("circle"), "Match");
    mClassOut->setValue(settings.outputClass);
    mClassOut->connectWithSetting(&settings.outputClass);
    mClassOut->setDisplayIconVisible(false);

    //
    //
    addSetting(modelTab, {
                             {mFunction.get(), true, 0},
                             {mClassOut.get(), true, 0},
                             {mMinCircleDistance.get(), false, 0},
                             {mMinCircleRadius.get(), false, 0},
                             {mMaxCircleRadius.get(), false, 0},
                             {mParam1.get(), false, 0},
                             {mParam2.get(), false, 0},
                         });
  }

private:
  /////////////////////////////////////////////////////
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBoxClassesOut> mClassOut;
  std::unique_ptr<SettingComboBox<joda::settings::HoughTransformSettings::HughMode>> mFunction;
  std::shared_ptr<SettingLineEdit<int32_t>> mMinCircleDistance;
  std::shared_ptr<SettingLineEdit<int32_t>> mMinCircleRadius;
  std::shared_ptr<SettingLineEdit<int32_t>> mMaxCircleRadius;
  std::shared_ptr<SettingLineEdit<float>> mParam1;
  std::shared_ptr<SettingLineEdit<float>> mParam2;

  /////////////////////////////////////////////////////

private slots:
};

}    // namespace joda::ui::gui
