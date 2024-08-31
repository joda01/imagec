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

  Intersection(settings::IntersectionSettings &settings, QWidget *parent) :
      Command(TITLE.data(), ICON.data(), parent), mSettings(settings), mParent(parent)
  {
    auto *modelTab = addTab("Base", [] {});

    mMode = SettingBase::create<SettingComboBox<joda::settings::IntersectionSettings::Function>>(parent, "", "Mode");
    mMode->addOptions(
        {{.key = joda::settings::IntersectionSettings::Function::COUNT, .label = "Count", .icon = ""},
         {.key = joda::settings::IntersectionSettings::Function::RECLASSIFY, .label = "Reclassify", .icon = ""}});
    mMode->setValue(settings.mode);
    mMode->connectWithSetting(&settings.mode);

    mClassOutput = generateClassDropDown<SettingComboBox<enums::ClassId>>("Reclassified class", parent);
    mClassOutput->setValue(settings.newClassId);
    mClassOutput->connectWithSetting(&settings.newClassId);

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

    auto *col = addSetting(modelTab, "Base settings",
                           {{mMode.get(), true}, {mClassOutput.get(), true}, {mMinIntersection.get(), false}});

    mClustersIn = generateClusterDropDown<SettingComboBox<enums::ClusterIdIn>>("Input cluster", parent);
    mClassesIn  = generateClassDropDown<SettingComboBoxMulti<enums::ClassId>>("Input classes", parent);

    mClustersIntersectWith =
        generateClusterDropDown<SettingComboBox<enums::ClusterIdIn>>("Intersecting cluster", parent);
    mClassesIntersectWith = generateClassDropDown<SettingComboBoxMulti<enums::ClassId>>("Intersecting classes", parent);

    auto *col2 = addSetting(modelTab, "Input classes", {{mClustersIn.get(), true}, {mClassesIn.get(), false}});
    addSetting(modelTab, "Intersect with", {{mClustersIntersectWith.get(), true}, {mClassesIntersectWith.get(), false}},
               col2);
  }

private:
  /////////////////////////////////////////////////////
  settings::IntersectionSettings &mSettings;
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<joda::settings::IntersectionSettings::Function>> mMode;
  std::unique_ptr<SettingLineEdit<float>> mMinIntersection;
  std::unique_ptr<SettingComboBox<enums::ClassId>> mClassOutput;

  std::unique_ptr<SettingComboBox<enums::ClusterIdIn>> mClustersIn;
  std::unique_ptr<SettingComboBoxMulti<enums::ClassId>> mClassesIn;

  std::unique_ptr<SettingComboBox<enums::ClusterIdIn>> mClustersIntersectWith;
  std::unique_ptr<SettingComboBoxMulti<enums::ClassId>> mClassesIntersectWith;

  /////////////////////////////////////////////////////

private slots:
};

}    // namespace joda::ui
