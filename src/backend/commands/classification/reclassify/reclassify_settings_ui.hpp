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
#include "ui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/helper/setting_generator.hpp"
#include "reclassify_settings.hpp"

namespace joda::ui {

class Reclassify : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Reclassify";
  inline static std::string ICON  = "move-right";

  Reclassify(joda::settings::PipelineStep &pipelineStep, settings::ReclassifySettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent, {InOuts::OBJECT, InOuts::OBJECT}), mSettings(settings), mParent(parent)
  {
    auto *modelTab = addTab("Base", [] {});

    //
    // Base settings
    //
    mClustersIn = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, generateIcon("circle"), "Input (e.g. Tetraspeck, Cell)");
    mClustersIn->setValue(settings.inputClusters);
    mClustersIn->connectWithSetting(&settings.inputClusters);

    mMode = SettingBase::create<SettingComboBox<joda::settings::ReclassifySettings::Mode>>(parent, {}, "Mode");
    mMode->addOptions(
        {{.key = joda::settings::ReclassifySettings::Mode::RECLASSIFY_MOVE, .label = "Reclassify Move", .icon = generateIcon("move-right")},
         {.key = joda::settings::ReclassifySettings::Mode::RECLASSIFY_COPY, .label = "Reclassify Copy", .icon = generateIcon("query-inner-join")}});
    mMode->setValue(settings.mode);
    mMode->connectWithSetting(&settings.mode);

    mClassOutput = SettingBase::create<SettingComboBoxClassesOut>(parent, generateIcon("circle"), "Reclassify to");
    mClassOutput->setValue(settings.newClassId);
    mClassOutput->connectWithSetting(&settings.newClassId);

    addSetting(modelTab, "Input", {{mClustersIn.get(), true, 0}, {mMode.get(), true, 0}, {mClassOutput.get(), true, 0}});

    //
    // Intersection filter
    //
    mClustersIntersectWith =
        SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, generateIcon("query-outer-join-right"), "Intersect with  (e.g. Spot)");
    mClustersIntersectWith->setValue(settings.intersection.inputClustersIntersectWith);
    mClustersIntersectWith->connectWithSetting(&settings.intersection.inputClustersIntersectWith);

    mMinIntersection = SettingBase::create<SettingLineEdit<float>>(parent, generateIcon("query-inner-join"), "Min. intersection");
    mMinIntersection->setDefaultValue(0.1);
    mMinIntersection->setPlaceholderText("[0 - 1]");
    mMinIntersection->setUnit("%");
    mMinIntersection->setMinMax(0, 1);
    mMinIntersection->setValue(settings.intersection.minIntersection);
    mMinIntersection->connectWithSetting(&settings.intersection.minIntersection);
    mMinIntersection->setShortDescription("Cls. ");

    auto *col2 = addSetting(modelTab, "Intersect with", {{mClustersIntersectWith.get(), false, 0}, {mMinIntersection.get(), false, 0}});

    //
    // Intensity filter
    //
    mMinIntensity = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("light-min"), "Min intensity");
    mMinIntensity->setPlaceholderText("[0 - 65535]");
    mMinIntensity->setUnit("");
    mMinIntensity->setMinMax(0, INT32_MAX);
    mMinIntensity->setValue(settings.intensity.minIntensity);
    mMinIntensity->connectWithSetting(&settings.intensity.minIntensity);
    mMinIntensity->setShortDescription("Min. ");
    //
    //
    mMaxIntensity = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("light"), "Max intensity");
    mMaxIntensity->setPlaceholderText("[0 - 65535]");
    mMaxIntensity->setUnit("");
    mMaxIntensity->setMinMax(0, INT32_MAX);
    mMaxIntensity->setValue(settings.intensity.maxIntensity);
    mMaxIntensity->connectWithSetting(&settings.intensity.maxIntensity);
    mMaxIntensity->setShortDescription("Max. ");

    //
    //
    cStackForIntensityFilter = generateCStackCombo<SettingComboBox<int32_t>>("Image channel", parent);
    cStackForIntensityFilter->setValue(settings.intensity.imageIn.imagePlane.cStack);
    cStackForIntensityFilter->connectWithSetting(&settings.intensity.imageIn.imagePlane.cStack);

    //
    //
    zProjectionForIntensityFilter = generateZProjection(true, parent);
    zProjectionForIntensityFilter->setValue(settings.intensity.imageIn.zProjection);
    zProjectionForIntensityFilter->connectWithSetting(&settings.intensity.imageIn.zProjection);

    addSetting(modelTab, "Intensity filter",
               {{cStackForIntensityFilter.get(), false, 0},
                {zProjectionForIntensityFilter.get(), false, 0},
                {mMinIntensity.get(), false, 0},
                {mMaxIntensity.get(), false, 0}},
               col2);
  }

private:
  /////////////////////////////////////////////////////
  settings::ReclassifySettings &mSettings;
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBoxMultiClassificationIn> mClustersIn;
  std::unique_ptr<SettingComboBox<joda::settings::ReclassifySettings::Mode>> mMode;
  std::unique_ptr<SettingComboBoxClassesOut> mClassOutput;

  std::unique_ptr<SettingComboBoxMultiClassificationIn> mClustersIntersectWith;
  std::unique_ptr<SettingLineEdit<float>> mMinIntersection;

  std::unique_ptr<SettingComboBox<int32_t>> cStackForIntensityFilter;
  std::unique_ptr<SettingComboBox<enums::ZProjection>> zProjectionForIntensityFilter;
  std::unique_ptr<SettingLineEdit<int>> mMinIntensity;
  std::unique_ptr<SettingLineEdit<int>> mMaxIntensity;

  /////////////////////////////////////////////////////

private slots:
};

}    // namespace joda::ui