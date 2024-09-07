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
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/helper/setting_generator.hpp"
#include "intersection_settings.hpp"

namespace joda::ui {

class Intersection : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Intersection";
  inline static std::string ICON  = "icons8-query-inner-join-50.png";

  Intersection(joda::settings::PipelineStep &pipelineStep, settings::IntersectionSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent), mSettings(settings), mParent(parent)
  {
    auto *modelTab = addTab("Base", [] {});

    mMode = SettingBase::create<SettingComboBox<joda::settings::IntersectionSettings::Function>>(parent, "", "Mode");
    mMode->addOptions(
        {{.key = joda::settings::IntersectionSettings::Function::COUNT, .label = "Count", .icon = ""},
         {.key = joda::settings::IntersectionSettings::Function::RECLASSIFY, .label = "Reclassify", .icon = ""}});
    mMode->setValue(settings.mode);
    mMode->connectWithSetting(&settings.mode);

    //
    //
    mMinIntersection = SettingBase::create<SettingLineEdit<float>>(parent, "", "Min. intersection");
    mMinIntersection->setDefaultValue(0.1);
    mMinIntersection->setPlaceholderText("[0 - 1]");
    mMinIntersection->setUnit("%");
    mMinIntersection->setMinMax(0, 1);
    mMinIntersection->setValue(settings.minIntersection);
    mMinIntersection->connectWithSetting(&settings.minIntersection);
    mMinIntersection->setShortDescription("Cls. ");

    auto *col = addSetting(modelTab, "Base settings", {{mMode.get(), true}, {mMinIntersection.get(), false}});

    mClustersIn = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, "", "Input (e.g. Tetraspeck)");
    mClustersIn->setValue(settings.inputObjects.inputClusters);
    mClustersIn->connectWithSetting(&settings.inputObjects.inputClusters);

    mClustersIntersectWith = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, "", "Intersect with");
    mClustersIntersectWith->setValue(settings.inputObjectsIntersectWith.inputClusters);
    mClustersIntersectWith->connectWithSetting(&settings.inputObjectsIntersectWith.inputClusters);

    mClassOutput = SettingBase::create<SettingComboBoxClassesOut>(parent, "", "Reclassify to");
    mClassOutput->setValue(settings.newClassId);
    mClassOutput->connectWithSetting(&settings.newClassId);

    auto *col2 = addSetting(modelTab, "Input classes", {{mClustersIn.get(), true}});
    addSetting(modelTab, "Intersect with", {{mClustersIntersectWith.get(), true}, {mClassOutput.get(), true}}, col2);
  }

private:
  /////////////////////////////////////////////////////
  settings::IntersectionSettings &mSettings;
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<joda::settings::IntersectionSettings::Function>> mMode;
  std::unique_ptr<SettingLineEdit<float>> mMinIntersection;
  std::unique_ptr<SettingComboBoxClassesOut> mClassOutput;

  std::unique_ptr<SettingComboBoxMultiClassificationIn> mClustersIn;
  std::unique_ptr<SettingComboBoxMultiClassificationIn> mClustersIntersectWith;

  /////////////////////////////////////////////////////

private slots:
};

}    // namespace joda::ui
