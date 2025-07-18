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

#include <qaction.h>
#include <qwidget.h>
#include <cstdint>
#include "backend/commands/command.hpp"
#include "backend/enums/enums_classes.hpp"

#include "ui/gui/editor/widget_pipeline/widget_command/command.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_base.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox_classes_out.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_line_edit.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "reclassify_settings.hpp"

namespace joda::ui::gui {

class Reclassify : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Reclassify";
  inline static std::string ICON              = "swap";
  inline static std::string DESCRIPTION       = "Change the classification of objects based on different parameters.";
  inline static std::vector<std::string> TAGS = {"classification", "reclassify", "copy", "move", "incell", "is in"};

  Reclassify(joda::settings::PipelineStep &pipelineStep, settings::ReclassifySettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::OBJECT}, {InOuts::OBJECT}}), mSettings(settings),
      mParent(parent)
  {
    auto *modelTab = addTab(
        "Base", [] {}, false);

    //
    // Base settings
    //
    mClassesIn = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, {}, "Input (e.g. Spot)");
    mClassesIn->setValue(settings.inputClasses);
    mClassesIn->connectWithSetting(&settings.inputClasses);

    mMode = SettingBase::create<SettingComboBox<joda::settings::ReclassifySettings::Mode>>(parent, {}, "Mode");
    mMode->addOptions(
        {{.key = joda::settings::ReclassifySettings::Mode::RECLASSIFY_MOVE, .label = "Reclassify Move", .icon = generateSvgIcon("edit-move")},
         {.key = joda::settings::ReclassifySettings::Mode::RECLASSIFY_COPY, .label = "Reclassify Copy", .icon = generateSvgIcon("edit-copy")}});
    mMode->setValue(settings.mode);
    mMode->connectWithSetting(&settings.mode);

    mClassOutput = SettingBase::create<SettingComboBoxClassesOut>(parent, {}, "Reclassify to");
    mClassOutput->setValue(settings.newClassId);
    mClassOutput->connectWithSetting(&settings.newClassId);

    addSetting(modelTab, "Input", {{mClassesIn.get(), true, 0}, {mMode.get(), true, 0}, {mClassOutput.get(), true, 0}});

    //
    // Intersection filter
    //
    mClassesIntersectWith = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, {}, "Intersect with (e.g. Tetraspeck)");
    mClassesIntersectWith->setValue(settings.intersection.inputClassesIntersectWith);
    mClassesIntersectWith->connectWithSetting(&settings.intersection.inputClassesIntersectWith);

    mMinIntersection = SettingBase::create<SettingLineEdit<float>>(parent, generateSvgIcon("format-number-percent"), "Min. intersection");
    mMinIntersection->setDefaultValue(0.1);
    mMinIntersection->setPlaceholderText("[0 - 1]");
    mMinIntersection->setUnit("%");
    mMinIntersection->setMinMax(0, 1);
    mMinIntersection->setValue(settings.intersection.minIntersection);
    mMinIntersection->connectWithSetting(&settings.intersection.minIntersection);
    mMinIntersection->setShortDescription("Cls. ");

    mFilterLogic = SettingBase::create<SettingComboBox<joda::settings::ReclassifySettings::FilterLogic>>(parent, {}, "Filter logic");
    mFilterLogic->addOptions(
        {{.key = joda::settings::ReclassifySettings::FilterLogic::APPLY_IF_MATCH, .label = "Move/Copy if intersect", .icon = {}},
         {.key = joda::settings::ReclassifySettings::FilterLogic::APPLY_IF_NOT_MATCH, .label = "Move/Copy if not intersect", .icon = {}}});
    mFilterLogic->setValue(settings.intersection.filterLogic);
    mFilterLogic->connectWithSetting(&settings.intersection.filterLogic);

    mHierarchyMode = SettingBase::create<SettingComboBox<joda::settings::ReclassifySettings::HierarchyHandling>>(parent, {}, "Hierarchy mode");
    mHierarchyMode->addOptions({{.key   = joda::settings::ReclassifySettings::HierarchyHandling::CREATE_TREE,
                                 .label = "Create hierarchy tree",
                                 .icon  = generateSvgIcon("view-list-tree")},
                                {.key   = joda::settings::ReclassifySettings::HierarchyHandling::KEEP_EXISTING,
                                 .label = "Keep existing tree",
                                 .icon  = generateSvgIcon("view-list-tree")},
                                {.key   = joda::settings::ReclassifySettings::HierarchyHandling::REMOVE,
                                 .label = "Remove tree information",
                                 .icon  = generateSvgIcon("view-list-tree")}});
    mHierarchyMode->setValue(settings.hierarchyMode);
    mHierarchyMode->connectWithSetting(&settings.hierarchyMode);

    auto *col2 = addSetting(modelTab, "Intersect with",
                            {{mClassesIntersectWith.get(), false, 0},
                             {mMinIntersection.get(), false, 0},
                             {mFilterLogic.get(), false, 0},
                             {mHierarchyMode.get(), false, 0}});

    //
    // Intensity filter
    //
    mMinIntensity = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateSvgIcon("brightness-low"), "Min intensity");
    mMinIntensity->setPlaceholderText("[0 - 65535]");
    mMinIntensity->setUnit("");
    mMinIntensity->setMinMax(0, INT32_MAX);
    mMinIntensity->setValue(settings.intensity.minIntensity);
    mMinIntensity->connectWithSetting(&settings.intensity.minIntensity);
    mMinIntensity->setShortDescription("Min. ");
    //
    //
    mMaxIntensity = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateSvgIcon("brightness-high"), "Max intensity");
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

    //
    //
    //
    zStackIndex = generateStackIndexCombo(true, "Z-Channel", parent);
    zStackIndex->setValue(settings.intensity.imageIn.imagePlane.zStack);
    zStackIndex->connectWithSetting(&settings.intensity.imageIn.imagePlane.zStack);

    addSetting(modelTab, "Intensity filter",
               {{cStackForIntensityFilter.get(), false, 0},
                {zProjectionForIntensityFilter.get(), false, 0},
                {zStackIndex.get(), false, 0},
                {mMinIntensity.get(), false, 0},
                {mMaxIntensity.get(), false, 0}},
               col2);
  }

private:
  /////////////////////////////////////////////////////
  settings::ReclassifySettings &mSettings;
  QWidget *mParent;

  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBoxMultiClassificationIn> mClassesIn;
  std::unique_ptr<SettingComboBox<joda::settings::ReclassifySettings::Mode>> mMode;
  std::unique_ptr<SettingComboBox<joda::settings::ReclassifySettings::HierarchyHandling>> mHierarchyMode;
  std::unique_ptr<SettingComboBoxClassesOut> mClassOutput;

  std::unique_ptr<SettingComboBoxMultiClassificationIn> mClassesIntersectWith;
  std::unique_ptr<SettingLineEdit<float>> mMinIntersection;
  std::unique_ptr<SettingComboBox<joda::settings::ReclassifySettings::FilterLogic>> mFilterLogic;

  std::unique_ptr<SettingComboBox<int32_t>> cStackForIntensityFilter;
  std::unique_ptr<SettingComboBox<enums::ZProjection>> zProjectionForIntensityFilter;
  std::unique_ptr<SettingSpinBox<int32_t>> zStackIndex;
  std::unique_ptr<SettingLineEdit<int>> mMinIntensity;
  std::unique_ptr<SettingLineEdit<int>> mMaxIntensity;

  /////////////////////////////////////////////////////

private slots:
};

}    // namespace joda::ui::gui
