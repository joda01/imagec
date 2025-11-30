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

#include <qwidget.h>
#include <cstdint>
#include <string>
#include "backend/commands/command.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_units.hpp"
#include "ui/gui/editor/widget_pipeline/widget_command/command.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox_classes_out.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_line_edit.hpp"
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
  inline static std::string ICON              = "circle";
  inline static std::string DESCRIPTION       = "Partition a plane into regions close to each of a given set of objects";
  inline static std::vector<std::string> TAGS = {"voronoi", "grid", "delaunay", "triangulation", "tessellation"};

  VoronoiGrid(joda::settings::AnalyzeSettings *analyzeSettings, joda::settings::PipelineStep &pipelineStep, settings::VoronoiGridSettings &settings,
              QWidget *parent) :
      Command(analyzeSettings, pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::OBJECT}, {InOuts::OBJECT}})
  {
    auto *tab = addTab(
        "", [] {}, false);
    //
    //
    pointsClasss = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, {}, "Centers");
    pointsClasss->setValue(settings.inputClassesPoints);
    pointsClasss->connectWithSetting(&settings.inputClassesPoints);

    voronoiClassOut = SettingBase::create<SettingComboBoxClassesOut>(parent, {}, "Output class");
    voronoiClassOut->setValue(settings.outputClassVoronoi);
    voronoiClassOut->connectWithSetting(&settings.outputClassVoronoi);

    //
    //
    mMaxRadius = SettingBase::create<SettingSpinBox<float>>(parent, {}, "Max. radius");
    mMaxRadius->setMinMax(-1, std::numeric_limits<float>::max(), 3, 0.01);
    mMaxRadius->setUnit("px", enums::ObjectType::LINE2D);
    mMaxRadius->setValue(settings.maxRadius);
    mMaxRadius->connectWithSetting(&settings.maxRadius);
    mMaxRadius->setShortDescription("Rad. ");

    //
    //
    maskingClasss = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, {}, "Masking classs (optional)");
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
    mMinAreaSize = SettingBase::create<SettingSpinBox<float>>(parent, {}, "Min. area size");
    mMinAreaSize->setMinMax(-1, std::numeric_limits<float>::max(), 3, 0.01);
    mMinAreaSize->setUnit("px", enums::ObjectType::AREA2D);
    mMinAreaSize->setValue(settings.minAreaSize);
    mMinAreaSize->connectWithSetting(&settings.minAreaSize);
    mMinAreaSize->setShortDescription("Min. ");

    //
    //
    mMaxAreaSize = SettingBase::create<SettingSpinBox<float>>(parent, {}, "Max. area size");
    mMaxAreaSize->setMinMax(-1, std::numeric_limits<float>::max(), 3, 0.01);
    mMaxAreaSize->setUnit("px", enums::ObjectType::AREA2D);
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

  std::unique_ptr<SettingSpinBox<float>> mMaxRadius;
  std::unique_ptr<SettingSpinBox<float>> mMinAreaSize;
  std::unique_ptr<SettingSpinBox<float>> mMaxAreaSize;
};

}    // namespace joda::ui::gui
