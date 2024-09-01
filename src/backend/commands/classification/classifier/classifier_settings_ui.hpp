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
#include "backend/enums/enums_clusters.hpp"
#include "ui/container/command/command.hpp"
#include "ui/container/setting/setting_base.hpp"
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/helper/setting_generator.hpp"
#include "classifier_settings.hpp"

namespace joda::ui {

class Classifier : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Classifier";
  inline static std::string ICON  = "icons8-genealogy-50.png";

  Classifier(settings::ClassifierSettings &settingsIn, QWidget *parent) :
      Command(TITLE.data(), ICON.data(), parent), mSettings(settingsIn), mParent(parent)
  {
    if(settingsIn.classifiers.empty()) {
      addFilter();
    }

    for(auto &classifierSetting : settingsIn.classifiers) {
      auto *tab = addTab("Filter", [this, &classifierSetting] { removeObjectClass(&classifierSetting); });
      mClassifyFilter.emplace_back(classifierSetting, *this, tab, parent);
    }

    auto *addFilter = addActionButton("Add filter", "icons8-add-new-50.png");
    connect(addFilter, &QAction::triggered, this, &Classifier::addFilter);
  }

private:
  /////////////////////////////////////////////////////
  struct ClassifierFilter
  {
    ClassifierFilter(settings::ObjectClass &settings, Classifier &outer, helper::TabWidget *tab, QWidget *parent) :
        outer(outer), tab(tab), settings(settings)
    {
      if(settings.filters.empty()) {
        settings.filters.emplace_back(settings::ClassifierFilter{});
      }
      auto &classifyFilter = *settings.filters.begin();

      //
      //
      mClassOutNoMatch = generateClassDropDown<SettingComboBox<enums::ClassId>>("No match class", parent);
      mClassOutNoMatch->setValue(settings.classOutNoMatch);
      mClassOutNoMatch->connectWithSetting(&settings.classOutNoMatch);

      //
      //
      mGrayScaleValue = SettingBase::create<SettingLineEdit<int32_t>>(parent, "", "Grayscale");
      mGrayScaleValue->setDefaultValue(65535);
      mGrayScaleValue->setPlaceholderText("[0 - 65535]");
      mGrayScaleValue->setUnit("%");
      mGrayScaleValue->setMinMax(0, 65535);
      mGrayScaleValue->setValue(settings.modelClassId);
      mGrayScaleValue->connectWithSetting(&settings.modelClassId);
      mGrayScaleValue->setShortDescription("Cls. ");

      outer.addSetting(tab, "Match",
                       {/*{mClusterOut.get(), false},*/
                        {mClassOutNoMatch.get(), true},
                        {mGrayScaleValue.get(), true}});

      //
      //
      mClassOut = generateClassDropDown<SettingComboBox<enums::ClassId>>("Match class", parent);
      mClassOut->setValue(classifyFilter.classOut);
      mClassOut->connectWithSetting(&classifyFilter.classOut);
      mClassOut->setDisplayIconVisible(false);

      //
      //
      mMinParticleSize = SettingBase::create<SettingLineEdit<int32_t>>(parent, "", "Min particle size");
      mMinParticleSize->setPlaceholderText("[0 - 2,147,483,647]");
      mMinParticleSize->setUnit("px");
      mMinParticleSize->setMinMax(0, INT32_MAX);
      mMinParticleSize->setValue(classifyFilter.minParticleSize);
      mMinParticleSize->connectWithSetting(&classifyFilter.minParticleSize);
      mMinParticleSize->setShortDescription("Min. ");
      //
      //
      mMaxParticleSize = SettingBase::create<SettingLineEdit<int32_t>>(parent, "", "Max particle size");
      mMaxParticleSize->setPlaceholderText("[0 - 2,147,483,647]");
      mMaxParticleSize->setUnit("px");
      mMaxParticleSize->setMinMax(0, INT32_MAX);
      mMaxParticleSize->setValue(classifyFilter.maxParticleSize);
      mMaxParticleSize->connectWithSetting(&classifyFilter.maxParticleSize);
      mMaxParticleSize->setShortDescription("Max. ");

      //
      //
      mMinCircularity = SettingBase::create<SettingLineEdit<float>>(parent, "", "Circularity [0-1]");
      mMinCircularity->setPlaceholderText("[0 - 1]");
      mMinCircularity->setUnit("%");
      mMinCircularity->setMinMax(0, 1);
      mMinCircularity->setValue(classifyFilter.minCircularity);
      mMinCircularity->connectWithSetting(&classifyFilter.minCircularity);
      mMinCircularity->setShortDescription("Circ. ");

      auto *col = outer.addSetting(tab, "Size filter",
                                   {/*{mClusterOut.get(), false},*/
                                    {mClassOut.get(), true},
                                    {mMinCircularity.get(), true},
                                    {mMinParticleSize.get(), true},
                                    {mMaxParticleSize.get(), true}});

      // Intensity filter

      //
      //
      mMinIntensity = SettingBase::create<SettingLineEdit<int32_t>>(parent, "", "Min intensity");
      mMinIntensity->setPlaceholderText("[0 - 65535]");
      mMinIntensity->setUnit("");
      mMinIntensity->setMinMax(0, INT32_MAX);
      mMinIntensity->setValue(classifyFilter.intensity.minIntensity);
      mMinIntensity->connectWithSetting(&classifyFilter.intensity.minIntensity);
      mMinIntensity->setShortDescription("Min. ");
      //
      //
      mMaxIntensity = SettingBase::create<SettingLineEdit<int32_t>>(parent, "", "Max intensity");
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
      zProjectionForIntensityFilter =
          SettingBase::create<SettingComboBox<enums::ZProjection>>(parent, "icons8-layers-50.png", "Z-Projection");
      zProjectionForIntensityFilter->addOptions({{enums::ZProjection::NONE, "Off"},
                                                 {enums::ZProjection::MAX_INTENSITY, "Max. intensity"},
                                                 {enums::ZProjection::MIN_INTENSITY, "Min. intensity"},
                                                 {enums::ZProjection::AVG_INTENSITY, "Avg'. intensity"}});
      zProjectionForIntensityFilter->setValue(classifyFilter.intensity.imageIn.imageIdx);
      zProjectionForIntensityFilter->connectWithSetting(&classifyFilter.intensity.imageIn.imageIdx);

      outer.addSetting(tab, "Intensity filter",
                       {{cStackForIntensityFilter.get(), true},
                        {zProjectionForIntensityFilter.get(), true},
                        {mMinIntensity.get(), true},
                        {mMaxIntensity.get(), true}},
                       col);
    }

    ~ClassifierFilter()
    {
      outer.removeSetting({/*mClusterOut.get(),*/ mClassOut.get(), mMinParticleSize.get(), mMaxParticleSize.get(),
                           mMinCircularity.get(), mSnapAreaSize.get(), cStackForIntensityFilter.get(),
                           zProjectionForIntensityFilter.get(), mMinIntensity.get(), mMaxIntensity.get()});
    }

    // std::unique_ptr<SettingComboBox<enums::ClusterIdIn>> mClusterOut;
    std::unique_ptr<SettingComboBox<enums::ClassId>> mClassOutNoMatch;
    std::unique_ptr<SettingLineEdit<int32_t>> mGrayScaleValue;
    QWidget *mParent;

    std::unique_ptr<SettingComboBox<enums::ClassId>> mClassOut;
    std::unique_ptr<SettingLineEdit<int>> mMinParticleSize;
    std::unique_ptr<SettingLineEdit<int>> mMaxParticleSize;
    std::unique_ptr<SettingLineEdit<float>> mMinCircularity;
    std::unique_ptr<SettingLineEdit<float>> mSnapAreaSize;

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
      auto it = mSettings.classifiers.begin();
      for(; it != mSettings.classifiers.end(); it++) {
        if(&(*it) == obj) {
          mSettings.classifiers.erase(it);
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
    auto &ret = mSettings.classifiers.emplace_back(objClass);
    auto *tab = addTab("Filter", [this, &ret] { removeObjectClass(&ret); });
    mClassifyFilter.emplace_back(ret, *this, tab, mParent);
    updateDisplayText();
  }
};

}    // namespace joda::ui
