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
  inline static std::string TITLE             = "Hough transform";
  inline static std::string ICON              = "trigonometry";
  inline static std::string DESCRIPTION       = "...";
  inline static std::vector<std::string> TAGS = {};

  HoughTransform(joda::settings::PipelineStep &pipelineStep, settings::HoughTransformSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), ICON.data(), parent, {{InOuts::BINARY}, {InOuts::OBJECT}}), mParent(parent)
  {
    auto *tab = addTab(
        "", [] {}, false);

    //
    // Options
    //
    mShape = SettingBase::create<SettingComboBox<joda::settings::HoughTransformSettings::Shape>>(parent, {}, "Shape");
    mShape->addOptions({
        //{.key = joda::settings::HoughTransformSettings::HughMode::LINE_TRANSFORM, .label = "Line", .icon = generateIcon("line")},
        {.key = joda::settings::HoughTransformSettings::Shape::CIRCLE_TRANSFORM, .label = "Circle", .icon = generateIcon("circle")},
    });

    mShape->setValue(settings.shape);
    mShape->connectWithSetting(&settings.shape);

    //
    //
    //
    mMinCircleDistance = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("resize-horizontal"), "Min. circle distance");
    mMinCircleDistance->setPlaceholderText("[0 - " + QString(std::to_string(INT32_MAX).data()) + "]");
    mMinCircleDistance->setUnit("");
    mMinCircleDistance->setMinMax(0, INT32_MAX);
    mMinCircleDistance->setValue(settings.circleProperties.minCircleDistance);
    mMinCircleDistance->connectWithSetting(&settings.circleProperties.minCircleDistance);
    mMinCircleDistance->setShortDescription("Min. ");

    //
    //
    //
    mMinCircleRadius = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("radius"), "Min. circle radius");
    mMinCircleRadius->setPlaceholderText("[0 - " + QString(std::to_string(INT32_MAX).data()) + "]");
    mMinCircleRadius->setUnit("");
    mMinCircleRadius->setMinMax(0, INT32_MAX);
    mMinCircleRadius->setValue(settings.circleProperties.minCircleRadius);
    mMinCircleRadius->connectWithSetting(&settings.circleProperties.minCircleRadius);
    mMinCircleRadius->setShortDescription("Min. ");

    //
    //
    //
    mMaxCircleRadius = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("radius"), "Max. circle radius");
    mMaxCircleRadius->setPlaceholderText("[0 - " + QString(std::to_string(INT32_MAX).data()) + "]");
    mMaxCircleRadius->setUnit("");
    mMaxCircleRadius->setMinMax(0, INT32_MAX);
    mMaxCircleRadius->setValue(settings.circleProperties.maxCircleRadius);
    mMaxCircleRadius->connectWithSetting(&settings.circleProperties.maxCircleRadius);
    mMaxCircleRadius->setShortDescription("Min. ");

    //
    //
    //
    mParam1 = SettingBase::create<SettingLineEdit<float>>(parent, generateIcon("light"), "Max. threshold of canny edge detector");
    mParam1->setPlaceholderText("[0 - " + QString(std::to_string(INT32_MAX).data()) + "]");
    mParam1->setUnit("");
    mParam1->setMinMax(0, INT32_MAX);
    mParam1->setValue(settings.circleProperties.param01);
    mParam1->connectWithSetting(&settings.circleProperties.param01);
    mParam1->setShortDescription("Min. ");

    // @param param2 Second method-specific parameter. In case of #HOUGH_GRADIENT, it is the
    // accumulator threshold for the circle centers at the detection stage. The smaller it is, the more
    // false circles may be detected. Circles, corresponding to the larger accumulator values, will be
    // returned first. In the case of #HOUGH_GRADIENT_ALT algorithm, this is the circle "perfectness" measure.
    // The closer it to 1, the better shaped circles algorithm selects. In most cases 0.9 should be fine.
    // If you want get better detection of small circles, you may decrease it to 0.85, 0.8 or even less.
    // But then also try to limit the search range [minRadius, maxRadius] to avoid many false circles.
    //     mParam2 =
    mParam2 = SettingBase::create<SettingLineEdit<float>>(parent, generateIcon("circle-thin"), "Circle perfectness measure");
    mParam2->setPlaceholderText("[0 - " + QString(std::to_string(INT32_MAX).data()) + "]");
    mParam2->setUnit("");
    mParam2->setMinMax(0, INT32_MAX);
    mParam2->setValue(settings.circleProperties.param02);
    mParam2->connectWithSetting(&settings.circleProperties.param02);
    mParam2->setShortDescription("Min. ");

    //
    //
    //
    mClassOut = SettingBase::create<SettingComboBoxClassesOut>(parent, generateIcon("circle"), "Output class");
    mClassOut->setValue(settings.outputClass);
    mClassOut->connectWithSetting(&settings.outputClass);
    mClassOut->setDisplayIconVisible(false);

    addSetting(tab, "Input / Output", {{mShape.get(), true, 0}, {mClassOut.get(), true, 0}});

    //
    //
    addSetting(tab, "Shape properties",
               {
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
  std::unique_ptr<SettingComboBox<joda::settings::HoughTransformSettings::Shape>> mShape;
  std::shared_ptr<SettingLineEdit<int32_t>> mMinCircleDistance;
  std::shared_ptr<SettingLineEdit<int32_t>> mMinCircleRadius;
  std::shared_ptr<SettingLineEdit<int32_t>> mMaxCircleRadius;
  std::shared_ptr<SettingLineEdit<float>> mParam1;
  std::shared_ptr<SettingLineEdit<float>> mParam2;

  /////////////////////////////////////////////////////

private slots:
};

}    // namespace joda::ui::gui
