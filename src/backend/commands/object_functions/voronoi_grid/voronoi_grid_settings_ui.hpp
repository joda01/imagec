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

#include <qwidget.h>
#include <cstdint>
#include <string>
#include "backend/commands/command.hpp"
#include "backend/enums/enums_classes.hpp"
#include "ui/container/command/command.hpp"
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/helper/setting_generator.hpp"
#include "voronoi_grid_settings.hpp"

namespace joda::ui {

class VoronoiGrid : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Voronoi";
  inline static std::string ICON  = "dom-voronoi-50.png";

  VoronoiGrid(settings::VoronoiGridSettings &settings, QWidget *parent) : Command(TITLE.data(), ICON.data(), parent)
  {
    auto *tab = addTab("", [] {});
    //
    //
    pointsCluster = generateClusterDropDown<SettingComboBox<enums::ClusterIdIn>>("Cluster containing centers", parent);
    pointsCluster->setValue(settings.pointsClusterIn);
    pointsCluster->connectWithSetting(&settings.pointsClusterIn);

    //
    //
    pointsClass = generateClassDropDown<SettingComboBoxMulti<enums::ClassId>>("Classes containing centers", parent);
    pointsClass->setValue(settings.pointsClassIn);
    pointsClass->connectWithSetting(&settings.pointsClassIn);

    //
    //
    maskingCluster = generateClusterDropDown<SettingComboBox<enums::ClusterIdIn>>("Masking cluster (optional)", parent);
    maskingCluster->setValue(settings.maskCluster);
    maskingCluster->connectWithSetting(&settings.maskCluster);

    //
    //
    maskingClass = generateClassDropDown<SettingComboBoxMulti<enums::ClassId>>("Masking classes (optional)", parent);
    maskingClass->setValue(settings.maskClasses);
    maskingClass->connectWithSetting(&settings.maskClasses);

    auto *col1 = addSetting(tab, "Voronoi input", {{pointsCluster.get(), true}, {pointsClass.get(), false}});
    addSetting(tab, "Voronoi masking", {{maskingCluster.get(), false}, {maskingClass.get(), false}}, col1);

    excludeAreasWithoutPoints = SettingBase::create<SettingComboBox<bool>>(parent, "", "Max. radius");
    excludeAreasWithoutPoints->setDefaultValue(true);
    excludeAreasWithoutPoints->setValue(settings.excludeAreasWithoutPoint);
    excludeAreasWithoutPoints->connectWithSetting(&settings.excludeAreasWithoutPoint);

    excludeAreasAtTheEdge = SettingBase::create<SettingComboBox<bool>>(parent, "", "Max. radius");
    excludeAreasAtTheEdge->setDefaultValue(true);
    excludeAreasAtTheEdge->setValue(settings.excludeAreasAtTheEdge);
    excludeAreasAtTheEdge->connectWithSetting(&settings.excludeAreasAtTheEdge);

    auto *col2 =
        addSetting(tab, "Filter", {{excludeAreasWithoutPoints.get(), true}, {excludeAreasAtTheEdge.get(), false}});

    //
    //
    mMaxRadius = SettingBase::create<SettingLineEdit<int32_t>>(parent, "", "Max. radius");
    mMaxRadius->setPlaceholderText("[0 - ]");
    mMaxRadius->setUnit("px");
    mMaxRadius->setMinMax(0, UINT32_MAX);
    mMaxRadius->setValue(settings.maxRadius);
    mMaxRadius->connectWithSetting(&settings.maxRadius);
    mMaxRadius->setShortDescription("Rad. ");

    //
    //
    mMinAreaSize = SettingBase::create<SettingLineEdit<int32_t>>(parent, "", "Min. area size");
    mMinAreaSize->setPlaceholderText("[0 - ]");
    mMinAreaSize->setUnit("px");
    mMinAreaSize->setMinMax(0, UINT32_MAX);
    mMinAreaSize->setValue(settings.minAreaSize);
    mMinAreaSize->connectWithSetting(&settings.minAreaSize);
    mMinAreaSize->setShortDescription("Min. ");

    //
    //
    mMaxAreaSize = SettingBase::create<SettingLineEdit<int32_t>>(parent, "", "Max. area size");
    mMaxAreaSize->setPlaceholderText("[0 - ]");
    mMaxAreaSize->setUnit("px");
    mMaxAreaSize->setMinMax(0, UINT32_MAX);
    mMaxAreaSize->setValue(settings.maxAreaSize);
    mMaxAreaSize->connectWithSetting(&settings.maxAreaSize);
    mMaxAreaSize->setShortDescription("Max. ");

    addSetting(tab, "Filter", {{mMaxRadius.get(), true}, {mMinAreaSize.get(), false}, {mMaxAreaSize.get(), false}},
               col2);
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBox<enums::ClassId>> voronoiClassOut;

  std::unique_ptr<SettingComboBox<enums::ClusterIdIn>> pointsCluster;
  std::unique_ptr<SettingComboBoxMulti<enums::ClassId>> pointsClass;

  std::unique_ptr<SettingComboBox<enums::ClusterIdIn>> maskingCluster;
  std::unique_ptr<SettingComboBoxMulti<enums::ClassId>> maskingClass;

  std::unique_ptr<SettingComboBox<bool>> excludeAreasWithoutPoints;
  std::unique_ptr<SettingComboBox<bool>> excludeAreasAtTheEdge;

  std::unique_ptr<SettingLineEdit<int32_t>> mMaxRadius;
  std::unique_ptr<SettingLineEdit<int32_t>> mMinAreaSize;
  std::unique_ptr<SettingLineEdit<int32_t>> mMaxAreaSize;
};

}    // namespace joda::ui
