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
#include "backend/enums/enums_classes.hpp"
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_base.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "classifier_settings.hpp"

namespace joda::ui::gui {

class Classifier : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Classifier";
  inline static std::string ICON  = "classify";

  Classifier(joda::settings::PipelineStep &pipelineStep, settings::ClassifierSettings &settingsIn, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent, {{InOuts::BINARY}, {InOuts::OBJECT}}), mSettings(settingsIn), mParent(parent)
  {
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

    auto *addFilter = addActionButton("Add filter", generateIcon("add"));
    connect(addFilter, &QAction::triggered, this, &Classifier::addFilter);
  }

private:
  /////////////////////////////////////////////////////
  struct ClassifierFilter
  {
    ClassifierFilter(settings::ObjectClass &settings, Classifier &outer, helper::TabWidget *tab, int32_t tabIndex, QWidget *parent) :
        outer(outer), tab(tab), settings(settings)
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
      mMinParticleSize = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("diameter"), "Min particle size");
      mMinParticleSize->setPlaceholderText("[0 - 2,147,483,647]");
      mMinParticleSize->setUnit("px");
      mMinParticleSize->setMinMax(0, INT32_MAX);
      mMinParticleSize->setValue(classifyFilter.metrics.minParticleSize);
      mMinParticleSize->connectWithSetting(&classifyFilter.metrics.minParticleSize);
      mMinParticleSize->setShortDescription("Min. ");
      //
      //
      mMaxParticleSize = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("diameter"), "Max particle size");
      mMaxParticleSize->setPlaceholderText("[0 - 2,147,483,647]");
      mMaxParticleSize->setUnit("px");
      mMaxParticleSize->setMinMax(0, INT32_MAX);
      mMaxParticleSize->setValue(classifyFilter.metrics.maxParticleSize);
      mMaxParticleSize->connectWithSetting(&classifyFilter.metrics.maxParticleSize);
      mMaxParticleSize->setShortDescription("Max. ");

      //
      //
      mMinCircularity = SettingBase::create<SettingLineEdit<float>>(parent, generateIcon("oval"), "Circularity [0-1]");
      mMinCircularity->setPlaceholderText("[0 - 1]");
      mMinCircularity->setUnit("%");
      mMinCircularity->setMinMax(0, 1);
      mMinCircularity->setValue(classifyFilter.metrics.minCircularity);
      mMinCircularity->connectWithSetting(&classifyFilter.metrics.minCircularity);
      mMinCircularity->setShortDescription("Circ. ");

      auto *col = outer.addSetting(tab, "Match filter",
                                   {{mGrayScaleValue.get(), true, tabIndex},
                                    {mMinCircularity.get(), true, tabIndex},
                                    {mMinParticleSize.get(), true, tabIndex},
                                    {mMaxParticleSize.get(), true, tabIndex}});

      //
      //
      mClassOut = SettingBase::create<SettingComboBoxClassesOut>(parent, generateIcon("circle"), "Match");
      mClassOut->setValue(classifyFilter.outputClass);
      mClassOut->connectWithSetting(&classifyFilter.outputClass);
      mClassOut->setDisplayIconVisible(false);

      //
      //
      mClassOutNoMatch = SettingBase::create<SettingComboBoxClassesOut>(parent, generateIcon("railroad-crossing"), "No match");
      mClassOutNoMatch->setValue(settings.outputClassNoMatch);
      mClassOutNoMatch->connectWithSetting(&settings.outputClassNoMatch);

      outer.addSetting(tab, "Result output", {{mClassOut.get(), true, tabIndex}, {mClassOutNoMatch.get(), true, tabIndex}});

      // Intensity filter

      //
      //
      mMinIntensity = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("light-min"), "Min intensity");
      mMinIntensity->setPlaceholderText("[0 - 65535]");
      mMinIntensity->setUnit("");
      mMinIntensity->setMinMax(0, INT32_MAX);
      mMinIntensity->setValue(classifyFilter.intensity.minIntensity);
      mMinIntensity->connectWithSetting(&classifyFilter.intensity.minIntensity);
      mMinIntensity->setShortDescription("Min. ");
      //
      //
      mMaxIntensity = SettingBase::create<SettingLineEdit<int32_t>>(parent, generateIcon("light"), "Max intensity");
      mMaxIntensity->setPlaceholderText("[0 - 65535]");
      mMaxIntensity->setUnit("");
      mMaxIntensity->setMinMax(0, INT32_MAX);
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
                           mMinCircularity.get(), cStackForIntensityFilter.get(), zProjectionForIntensityFilter.get(), mMinIntensity.get(),
                           mMaxIntensity.get()});
    }

    // std::unique_ptr<SettingComboBox<enums::ClasssIdIn>> mClasssOut;
    std::unique_ptr<SettingComboBoxClassesOut> mClassOutNoMatch;
    std::unique_ptr<SettingComboBox<int32_t>> mGrayScaleValue;
    QWidget *mParent;

    std::unique_ptr<SettingComboBoxClassesOut> mClassOut;
    std::unique_ptr<SettingLineEdit<int>> mMinParticleSize;
    std::unique_ptr<SettingLineEdit<int>> mMaxParticleSize;
    std::unique_ptr<SettingLineEdit<float>> mMinCircularity;

    std::unique_ptr<SettingComboBox<int32_t>> cStackForIntensityFilter;
    std::unique_ptr<SettingComboBox<enums::ZProjection>> zProjectionForIntensityFilter;
    std::unique_ptr<SettingLineEdit<int>> mMinIntensity;
    std::unique_ptr<SettingLineEdit<int>> mMaxIntensity;

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
};

}    // namespace joda::ui::gui
