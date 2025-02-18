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
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/gui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "voronoi_grid_settings.hpp"

namespace joda::ui::gui {

class VoronoiGrid : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Voronoi";
  inline static std::string ICON              = "voronoi";
  inline static std::string DESCRIPTION       = "Partition a plane into regions close to each of a given set of objects";
  inline static std::vector<std::string> TAGS = {"voronoi", "grid", "delaunay", "triangulation", "tessellation"};

  VoronoiGrid(joda::settings::PipelineStep &pipelineStep, settings::VoronoiGridSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::OBJECT}, {InOuts::OBJECT}})
  {
    auto *tab = addTab(
        "", [] {}, false);
    //
    //
    pointsClasss = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, generateIcon("initial-state"), "Centers");
    pointsClasss->setValue(settings.inputClassesPoints);
    pointsClasss->connectWithSetting(&settings.inputClassesPoints);

    voronoiClassOut = SettingBase::create<SettingComboBoxClassesOut>(parent, generateIcon("voronoi"), "Output class");
    voronoiClassOut->setValue(settings.outputClassVoronoi);
    voronoiClassOut->connectWithSetting(&settings.outputClassVoronoi);

    //
    //
    mMaxRadius = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("radius"), "Max. radius");
    mMaxRadius->setPlaceholderText("[0 - ]");
    mMaxRadius->setUnit("px");
    mMaxRadius->setMinMax(0, INT32_MAX);
    mMaxRadius->setValue(settings.maxRadius);
    mMaxRadius->connectWithSetting(&settings.maxRadius);
    mMaxRadius->setShortDescription("Rad. ");

    //
    //
    maskingClasss = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, generateIcon("layer-mask"), "Masking classs (optional)");
    maskingClasss->setValue(settings.inputClassesMask);
    maskingClasss->connectWithSetting(&settings.inputClassesMask);

    auto *col1 = addSetting(tab, "Voronoi input/output", {{pointsClasss.get(), true, 0}, {voronoiClassOut.get(), false, 0}});
    addSetting(tab, "Voronoi masking", {{mMaxRadius.get(), true, 0}, {maskingClasss.get(), false, 0}}, col1);

    excludeAreasWithoutPoints = SettingBase::create<SettingComboBox<bool>>(parent, {}, "Exclude areas without points");
    excludeAreasWithoutPoints->addOptions({{false, "Off", {}}, {true, "On", {}}});
    excludeAreasWithoutPoints->setDefaultValue(true);
    excludeAreasWithoutPoints->setValue(settings.excludeAreasWithoutPoint);
    excludeAreasWithoutPoints->connectWithSetting(&settings.excludeAreasWithoutPoint);

    excludeAreasAtTheEdge = SettingBase::create<SettingComboBox<bool>>(parent, {}, "Exclude areas at the edges");
    excludeAreasAtTheEdge->addOptions({{false, "Off", {}}, {true, "On", {}}});
    excludeAreasAtTheEdge->setDefaultValue(true);
    excludeAreasAtTheEdge->setValue(settings.excludeAreasAtTheEdge);
    excludeAreasAtTheEdge->connectWithSetting(&settings.excludeAreasAtTheEdge);

    auto *col2 = addSetting(tab, "Area filter", {{excludeAreasWithoutPoints.get(), true, 0}, {excludeAreasAtTheEdge.get(), false, 0}});

    //
    //
    mMinAreaSize = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("diameter"), "Min. area size");
    mMinAreaSize->setPlaceholderText("[0 - ]");
    mMinAreaSize->setUnit("px");
    mMinAreaSize->setMinMax(0, INT32_MAX);
    mMinAreaSize->setValue(settings.minAreaSize);
    mMinAreaSize->connectWithSetting(&settings.minAreaSize);
    mMinAreaSize->setShortDescription("Min. ");

    //
    //
    mMaxAreaSize = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("diameter"), "Max. area size");
    mMaxAreaSize->setPlaceholderText("[0 - ]");
    mMaxAreaSize->setUnit("px");
    mMaxAreaSize->setMinMax(0, INT32_MAX);
    mMaxAreaSize->setValue(settings.maxAreaSize);
    mMaxAreaSize->connectWithSetting(&settings.maxAreaSize);
    mMaxAreaSize->setShortDescription("Max. ");

    addSetting(tab, "Size filter", {{mMinAreaSize.get(), false, 0}, {mMaxAreaSize.get(), false, 0}}, col2);
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBoxClassesOut> voronoiClassOut;
  std::unique_ptr<SettingComboBoxMultiClassificationIn> pointsClasss;
  std::unique_ptr<SettingComboBoxMultiClassificationIn> maskingClasss;

  std::unique_ptr<SettingComboBox<bool>> excludeAreasWithoutPoints;
  std::unique_ptr<SettingComboBox<bool>> excludeAreasAtTheEdge;

  std::unique_ptr<SettingLineEdit<int32_t>> mMaxRadius;
  std::unique_ptr<SettingLineEdit<int32_t>> mMinAreaSize;
  std::unique_ptr<SettingLineEdit<int32_t>> mMaxAreaSize;
};

}    // namespace joda::ui::gui
