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
#include "backend/commands/classification/classifier_filter.hpp"
#include "backend/commands/command.hpp"
#include "backend/commands/object_functions/colocalization/colocalization_settings.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "ui/container/command/command.hpp"
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/helper/setting_generator.hpp"
#include "colocalization_settings.hpp"

namespace joda::ui {

class Colocalization : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Colocalization";
  inline static std::string ICON  = "icons8-venn-diagram-50.png";

  Colocalization(settings::ColocalizationSettings &settings, QWidget *parent) :
      Command(TITLE.data(), ICON.data(), parent), mSettings(settings), mParent(parent)
  {
    auto *modelTab = addTab("Base", [] {});

    mClassOutput = generateClassDropDown<SettingComboBox<enums::ClassId>>("Output class", parent);
    mClassOutput->setValue(settings.classOut);
    mClassOutput->connectWithSetting(&settings.classOut);

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

    auto *col = addSetting(modelTab, "Base settings.", {{mClassOutput.get(), true}, {mMinIntersection.get(), false}});

    mClustersIn = generateClusterDropDown<SettingComboBoxMulti<enums::ClusterIdIn>>("Coloc clusters", parent);
    connect(mClustersIn.get(), &SettingBase::valueChanged, this, &Colocalization::onChange);

    mClassesIn = generateClassDropDown<SettingComboBoxMulti<enums::ClassId>>("Coloc classes", parent);
    connect(mClustersIn.get(), &SettingBase::valueChanged, this, &Colocalization::onChange);

    addSetting(modelTab, "Coloc with.", {{mClustersIn.get(), true}, {mClassesIn.get(), false}});

    std::set<enums::ClusterIdIn> clustersToSet;
    std::set<enums::ClassId> classesToSet;
    for(auto &objSet : settings.objectsIn) {
      clustersToSet.emplace(objSet.clusterIn);
      classesToSet.insert(objSet.classesIn.begin(), objSet.classesIn.end());
    }
    mClustersIn->setValue(clustersToSet);
    mClassesIn->setValue(classesToSet);
  }

private:
  /////////////////////////////////////////////////////
  settings::ColocalizationSettings &mSettings;
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<enums::ClassId>> mClassOutput;
  std::unique_ptr<SettingLineEdit<float>> mMinIntersection;

  std::unique_ptr<SettingComboBoxMulti<enums::ClusterIdIn>> mClustersIn;
  std::unique_ptr<SettingComboBoxMulti<enums::ClassId>> mClassesIn;

  /////////////////////////////////////////////////////

private slots:
  void onChange()
  {
    auto clusters = mClustersIn->getValue();
    mSettings.objectsIn.clear();
    for(const auto &cluster : clusters) {
      settings::ColocalizationSettings::IntersectingClasses classs;
      classs.clusterIn = cluster;
      classs.classesIn = mClassesIn->getValue();
      mSettings.objectsIn.emplace_back(classs);
    }
  }
};

}    // namespace joda::ui