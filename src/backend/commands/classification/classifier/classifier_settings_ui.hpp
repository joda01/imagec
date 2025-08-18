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
#include "backend/commands/classification/classifier_filter.hpp"
#include "backend/commands/command.hpp"
#include "backend/enums/enums_classes.hpp"
#include "ui/gui/editor/widget_pipeline/widget_command/command.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_base.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox_classes_out.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_line_edit.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "classifier_settings.hpp"

namespace joda::ui::gui {

class Classifier : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Classifier";
  inline static std::string ICON              = "shapes";
  inline static std::string DESCRIPTION       = "Extract objects from a binary image.";
  inline static std::vector<std::string> TAGS = {"classifier", "classify", "objects"};

  Classifier(joda::settings::PipelineStep &pipelineStep, settings::ClassifierSettings &settingsIn, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::BINARY}, {InOuts::OBJECT}}), mSettings(settingsIn),
      mParent(parent)
  {
    this->mutableEditDialog()->setMinimumWidth(600);
    this->mutableEditDialog()->setMinimumHeight(400);

    //
    // Filter settings
    //
    if(settingsIn.modelClasses.empty()) {
      addFilter();
    }

    int32_t cnt = 1;
    for(auto &classifierSetting : settingsIn.modelClasses) {
      auto *tab = addTab(
          "Filter", [this, &classifierSetting] { removeObjectClass(&classifierSetting); }, false);
      mClassifyFilter.emplace_back(classifierSetting, *this, tab, cnt, parent);
      cnt++;
    }

    //
    // Options
    //
    auto *detectionSettings = addTab(
        "Detection settings", [] {}, false);

    mFunction = SettingBase::create<SettingComboBox<joda::settings::ClassifierSettings::HierarchyMode>>(parent, {}, "Detection hierarchy");
    mFunction->addOptions({
        {.key = joda::settings::ClassifierSettings::HierarchyMode::OUTER, .label = "Outer", .icon = {}},
        {.key = joda::settings::ClassifierSettings::HierarchyMode::INNER, .label = "Inner", .icon = {}},
        {.key = joda::settings::ClassifierSettings::HierarchyMode::INNER_AND_OUTER, .label = "Inner & Outer", .icon = {}},
    });

    mFunction->setValue(settingsIn.detectionHierarchy);
    mFunction->connectWithSetting(&settingsIn.detectionHierarchy);
    addSetting(detectionSettings, "Model settings", {{mFunction.get(), false, 0}});

    auto *addFilter = addActionButton("Add filter", generateSvgIcon<Style::REGULAR, Color::BLACK>("list-plus"));
    connect(addFilter, &QAction::triggered, this, &Classifier::addFilter);
  }

private:
  /////////////////////////////////////////////////////
  struct ClassifierFilter
  {
    ClassifierFilter(settings::ObjectClass &settingsIn, Classifier &outerIn, helper::TabWidget *tabIn, int32_t tabIndex, QWidget *parent) :
        settings(settingsIn), outer(outerIn), tab(tabIn)
    {
      if(settings.filters.empty()) {
        settings.filters.emplace_back(settings::ClassifierFilter{});
      }
      auto &classifyFilter = *settings.filters.begin();

      //
      //
      mGrayScaleValue = generateThresholdClass("Threshold class input", parent);
      mGrayScaleValue->setValue(settings.modelClassId);
      mGrayScaleValue->connectWithSetting(&settings.modelClassId);

      //
      //
      mMinParticleSize = SettingBase::create<SettingSpinBox<float>>(parent, {}, "Min particle size");
      mMinParticleSize->setMinMax(-1, std::numeric_limits<float>::max(), 3, 0.01);
      mMinParticleSize->setUnit("px", true);
      mMinParticleSize->setMinMax(0, std::numeric_limits<float>::max());
      mMinParticleSize->setValue(classifyFilter.metrics.minParticleSize);
      mMinParticleSize->connectWithSetting(&classifyFilter.metrics.minParticleSize);
      mMinParticleSize->setShortDescription("Min. ");
      //
      //
      mMaxParticleSize = SettingBase::create<SettingSpinBox<float>>(parent, {}, "Max particle size");
      mMaxParticleSize->setMinMax(-1, std::numeric_limits<float>::max(), 3, 0.01);
      mMaxParticleSize->setUnit("px", true);
      mMaxParticleSize->setMinMax(0, std::numeric_limits<float>::max());
      mMaxParticleSize->setValue(classifyFilter.metrics.maxParticleSize);
      mMaxParticleSize->connectWithSetting(&classifyFilter.metrics.maxParticleSize);
      mMaxParticleSize->setShortDescription("Max. ");

      //
      //
      mMinCircularity =
          SettingBase::create<SettingSpinBox<float>>(parent, generateSvgIcon<Style::REGULAR, Color::BLACK>("circle"), "Circularity [0-1]");
      mMinCircularity->setMinMax(0, 1, 2, 0.1);
      mMinCircularity->setUnit("%", false);
      mMinCircularity->setMinMax(0, 1);
      mMinCircularity->setValue(classifyFilter.metrics.minCircularity);
      mMinCircularity->connectWithSetting(&classifyFilter.metrics.minCircularity);
      mMinCircularity->setShortDescription("Circ. ");

      //
      //
      mExcludeObjectsAtTheEdge = SettingBase::create<SettingComboBox<bool>>(parent, {}, "Exclude objects at the edges");
      mExcludeObjectsAtTheEdge->addOptions({{false, "Off", {}}, {true, "On", {}}});
      mExcludeObjectsAtTheEdge->setDefaultValue(true);
      mExcludeObjectsAtTheEdge->setValue(classifyFilter.metrics.excludeObjectsAtTheEdge);
      mExcludeObjectsAtTheEdge->connectWithSetting(&classifyFilter.metrics.excludeObjectsAtTheEdge);

      outer.addSetting(tab, "Match filter",
                       {{mGrayScaleValue.get(), true, tabIndex},
                        {mMinCircularity.get(), true, tabIndex},
                        {mMinParticleSize.get(), true, tabIndex},
                        {mMaxParticleSize.get(), true, tabIndex},
                        {mExcludeObjectsAtTheEdge.get(), false, tabIndex}});

      //
      //
      mClassOut = SettingBase::create<SettingComboBoxClassesOut>(parent, {}, "Match");
      mClassOut->setValue(classifyFilter.outputClass);
      mClassOut->connectWithSetting(&classifyFilter.outputClass);
      mClassOut->setDisplayIconVisible(false);

      //
      //
      mClassOutNoMatch = SettingBase::create<SettingComboBoxClassesOut>(parent, {}, "No match");
      mClassOutNoMatch->setValue(settings.outputClassNoMatch);
      mClassOutNoMatch->connectWithSetting(&settings.outputClassNoMatch);

      outer.addSetting(tab, "Result output", {{mClassOut.get(), true, tabIndex}, {mClassOutNoMatch.get(), true, tabIndex}});

      // Intensity filter

      //
      //
      mMinIntensity = SettingBase::create<SettingLineEdit<float>>(parent, {}, "Min intensity");
      mMinIntensity->setPlaceholderText("[0 - 65535]");
      mMinIntensity->setUnit("");
      mMinIntensity->setMinMax(0, std::numeric_limits<float>::max());
      mMinIntensity->setValue(classifyFilter.intensity.minIntensity);
      mMinIntensity->connectWithSetting(&classifyFilter.intensity.minIntensity);
      mMinIntensity->setShortDescription("Min. ");
      //
      //
      mMaxIntensity = SettingBase::create<SettingLineEdit<float>>(parent, {}, "Max intensity");
      mMaxIntensity->setPlaceholderText("[0 - 65535]");
      mMaxIntensity->setUnit("");
      mMaxIntensity->setMinMax(0, std::numeric_limits<float>::max());
      mMaxIntensity->setValue(classifyFilter.intensity.maxIntensity);
      mMaxIntensity->connectWithSetting(&classifyFilter.intensity.maxIntensity);
      mMaxIntensity->setShortDescription("Max. ");

      //
      //
      cStackForIntensityFilter = generateCStackCombo<SettingComboBox<int32_t>>("Image channel", parent);
      cStackForIntensityFilter->setValue(classifyFilter.intensity.imageIn.imagePlane.cStack);
      cStackForIntensityFilter->connectWithSetting(&classifyFilter.intensity.imageIn.imagePlane.cStack);

      //
      //
      zProjectionForIntensityFilter = generateZProjection(true, parent);
      zProjectionForIntensityFilter->setValue(classifyFilter.intensity.imageIn.zProjection);
      zProjectionForIntensityFilter->connectWithSetting(&classifyFilter.intensity.imageIn.zProjection);

      //
      //
      //
      zStackIndex = generateStackIndexCombo(true, "Z-Channel", parent);
      zStackIndex->setValue(classifyFilter.intensity.imageIn.imagePlane.zStack);
      zStackIndex->connectWithSetting(&classifyFilter.intensity.imageIn.imagePlane.zStack);

      /*outer.addSetting(tab, "Intensity filter",
                       {{cStackForIntensityFilter.get(), true},
                        {zProjectionForIntensityFilter.get(), true},
                        {mMinIntensity.get(), true},
                        {mMaxIntensity.get(), true}},
                       col);*/
    }

    ~ClassifierFilter()
    {
      outer.removeSetting({mClassOutNoMatch.get(), mGrayScaleValue.get(), mClassOut.get(), mMinParticleSize.get(), mMaxParticleSize.get(),
                           mMinCircularity.get(), cStackForIntensityFilter.get(), zProjectionForIntensityFilter.get(), zStackIndex.get(),
                           mMinIntensity.get(), mMaxIntensity.get(), mExcludeObjectsAtTheEdge.get()});
    }

    // std::unique_ptr<SettingComboBox<enums::ClasssIdIn>> mClasssOut;

    std::unique_ptr<SettingComboBoxClassesOut> mClassOutNoMatch;
    std::unique_ptr<SettingComboBox<int32_t>> mGrayScaleValue;
    QWidget *mParent;

    std::unique_ptr<SettingComboBoxClassesOut> mClassOut;
    std::unique_ptr<SettingSpinBox<float>> mMinParticleSize;
    std::unique_ptr<SettingSpinBox<float>> mMaxParticleSize;
    std::unique_ptr<SettingSpinBox<float>> mMinCircularity;

    std::unique_ptr<SettingComboBox<int32_t>> cStackForIntensityFilter;
    std::unique_ptr<SettingComboBox<enums::ZProjection>> zProjectionForIntensityFilter;
    std::unique_ptr<SettingSpinBox<int32_t>> zStackIndex;
    std::unique_ptr<SettingLineEdit<float>> mMinIntensity;
    std::unique_ptr<SettingLineEdit<float>> mMaxIntensity;
    std::unique_ptr<SettingComboBox<bool>> mExcludeObjectsAtTheEdge;

    settings::ObjectClass &settings;
    Classifier &outer;
    helper::TabWidget *tab;
  };

  std::list<ClassifierFilter> mClassifyFilter;
  settings::ClassifierSettings &mSettings;
  QWidget *mParent;

  void removeObjectClass(settings::ObjectClass *obj)
  {
    {
      auto it = mSettings.modelClasses.begin();
      for(; it != mSettings.modelClasses.end(); it++) {
        if(&(*it) == obj) {
          mSettings.modelClasses.erase(it);
          break;
        }
      }
    }

    {
      auto it = mClassifyFilter.begin();
      for(; it != mClassifyFilter.end(); it++) {
        if(&(it->settings) == obj) {
          mClassifyFilter.erase(it);
          break;
        }
      }
    }
    updateDisplayText();
  }

  /////////////////////////////////////////////////////
private slots:
  void addFilter()
  {
    settings::ObjectClass objClass;
    auto &ret = mSettings.modelClasses.emplace_back(objClass);
    auto *tab = addTab(
        "Filter", [this, &ret] { removeObjectClass(&ret); }, true);
    mClassifyFilter.emplace_back(ret, *this, tab, mSettings.modelClasses.size(), mParent);
    updateDisplayText();
  }

  std::unique_ptr<SettingComboBox<joda::settings::ClassifierSettings::HierarchyMode>> mFunction;
};

}    // namespace joda::ui::gui
