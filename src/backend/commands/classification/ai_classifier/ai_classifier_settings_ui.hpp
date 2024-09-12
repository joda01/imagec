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
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/helper/setting_generator.hpp"
#include "ai_classifier_settings.hpp"

namespace joda::ui {

class AiClassifier : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "AI Classifier";
  inline static std::string ICON  = "icons8-magic-50.png";

  AiClassifier(joda::settings::PipelineStep &pipelineStep, settings::AiClassifierSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent), mSettings(settings), mParent(parent)
  {
    auto *modelTab = addTab("Model", [] {});

    mModelPath = SettingBase::create<SettingLineEdit<std::string>>(parent, "", "Model path");
    mModelPath->connectWithSetting(&settings.modelPath);
    mModelPath->setValue(settings.modelPath);
    mModelPath->setShortDescription("Model:");

    //
    //
    mNumberOdModelClasses = SettingBase::create<SettingLineEdit<int32_t>>(parent, "", "Nr. of model classes");
    mNumberOdModelClasses->setPlaceholderText("[0 - 2,147,483,647]");
    mNumberOdModelClasses->setUnit("");
    mNumberOdModelClasses->setMinMax(1, INT32_MAX);
    mNumberOdModelClasses->setValue(settings.numberOfModelClasses);
    mNumberOdModelClasses->connectWithSetting(&settings.numberOfModelClasses);
    mNumberOdModelClasses->setShortDescription("Classes:");

    auto *col =
        addSetting(modelTab, "AI model settings", {{mModelPath.get(), true}, {mNumberOdModelClasses.get(), false}});
    for(auto &classifierSetting : settings.modelClasses) {
      auto *tab = addTab("Class", [this, &classifierSetting] { removeObjectClass(&classifierSetting); });
      modelClasses.emplace_back(classifierSetting, *this, tab, parent);
    }
    auto *addClassifier = addActionButton("Add class", "icons8-add-new-50.png");
    connect(addClassifier, &QAction::triggered, this, &AiClassifier::addClassifier);
  }

private:
  /////////////////////////////////////////////////////
  struct ClassifierFilter
  {
    ClassifierFilter(settings::ClassifierFilter &settings, AiClassifier &outer, helper::TabWidget *tab,
                     QWidget *parent) :
        outer(outer),
        tab(tab)
    {
      //
      //
      // mClusterOut = generateClusterDropDown<SettingComboBox<enums::ClusterIdIn>>("Match cluster", parent);
      // mClusterOut->setValue(settings.clusterOut);
      // mClusterOut->connectWithSetting(&settings.clusterOut);

      //
      //
      mClassOut = SettingBase::create<SettingComboBoxClassesOut>(parent, "", "Match class");
      mClassOut->setValue(settings.outputCluster.classId);
      mClassOut->connectWithSetting(&settings.outputCluster.classId);
      mClassOut->setDisplayIconVisible(false);

      //
      //
      mMinParticleSize = SettingBase::create<SettingLineEdit<int32_t>>(parent, "", "Min particle size");
      mMinParticleSize->setPlaceholderText("[0 - 2,147,483,647]");
      mMinParticleSize->setUnit("px");
      mMinParticleSize->setMinMax(0, INT32_MAX);
      mMinParticleSize->setValue(settings.minParticleSize);
      mMinParticleSize->connectWithSetting(&settings.minParticleSize);
      mMinParticleSize->setShortDescription("Min. ");
      //
      //
      mMaxParticleSize = SettingBase::create<SettingLineEdit<int32_t>>(parent, "", "Max particle size");
      mMaxParticleSize->setPlaceholderText("[0 - 2,147,483,647]");
      mMaxParticleSize->setUnit("px");
      mMaxParticleSize->setMinMax(0, INT32_MAX);
      mMaxParticleSize->setValue(settings.maxParticleSize);
      mMaxParticleSize->connectWithSetting(&settings.maxParticleSize);
      mMaxParticleSize->setShortDescription("Max. ");

      //
      //
      mMinCircularity = SettingBase::create<SettingLineEdit<float>>(parent, "", "Circularity [0-1]");
      mMinCircularity->setPlaceholderText("[0 - 1]");
      mMinCircularity->setUnit("%");
      mMinCircularity->setMinMax(0, 1);
      mMinCircularity->setValue(settings.minCircularity);
      mMinCircularity->connectWithSetting(&settings.minCircularity);
      mMinCircularity->setShortDescription("Circ. ");

      outer.addSetting(tab, "Filter",
                       {/*{mClusterOut.get(), false},*/
                        {mClassOut.get(), true},
                        {mMinCircularity.get(), true},
                        {mMinParticleSize.get(), true},
                        {mMaxParticleSize.get(), true}});
    }

    ~ClassifierFilter()
    {
      outer.removeSetting({/*mClusterOut.get(),*/ mClassOut.get(), mMinParticleSize.get(), mMaxParticleSize.get(),
                           mMinCircularity.get(), mSnapAreaSize.get()});
    }

    // std::unique_ptr<SettingComboBox<enums::ClusterIdIn>> mClusterOut;
    std::unique_ptr<SettingComboBoxClassesOut> mClassOut;
    std::unique_ptr<SettingLineEdit<int>> mMinParticleSize;
    std::unique_ptr<SettingLineEdit<int>> mMaxParticleSize;
    std::unique_ptr<SettingLineEdit<float>> mMinCircularity;
    std::unique_ptr<SettingLineEdit<float>> mSnapAreaSize;

    AiClassifier &outer;
    helper::TabWidget *tab;
  };

  struct ObjectClass
  {
    ObjectClass(settings::ObjectClass &settings, AiClassifier &outer, helper::TabWidget *tab, QWidget *parent) :
        outer(outer), tab(tab), mSettings(settings)
    {
      //
      //
      // mClusterOutNoMatch = generateClusterDropDown<SettingComboBox<enums::ClusterIdIn>>("No match cluster", parent);
      // mClusterOutNoMatch->setValue(settings.clusterOutNoMatch);
      // mClusterOutNoMatch->connectWithSetting(&settings.clusterOutNoMatch);

      //
      //
      mClassOutNoMatch = SettingBase::create<SettingComboBoxClassesOut>(parent, "", "No match class");
      mClassOutNoMatch->setValue(settings.outputClusterNoMatch.classId);
      mClassOutNoMatch->connectWithSetting(&settings.outputClusterNoMatch.classId);

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

      auto *col = outer.addSetting(tab, "Classification", {{mGrayScaleValue.get(), false}});
      outer.addSetting(tab, "No match handling",
                       {/*{mClusterOutNoMatch.get(), false},*/ {mClassOutNoMatch.get(), false}}, col);

      for(auto &filter : settings.filters) {
        mClassifyFilter.emplace_back(filter, outer, tab, parent);
      }
    }

    ~ObjectClass()
    {
      outer.removeSetting({/*mClusterOutNoMatch.get(),*/ mClassOutNoMatch.get()});
    }

    // std::unique_ptr<SettingComboBox<enums::ClusterIdIn>> mClusterOutNoMatch;
    std::unique_ptr<SettingComboBoxClassesOut> mClassOutNoMatch;
    std::unique_ptr<SettingLineEdit<int32_t>> mGrayScaleValue;

    std::list<ClassifierFilter> mClassifyFilter;
    AiClassifier &outer;
    helper::TabWidget *tab;
    settings::ObjectClass &mSettings;
  };

  std::list<ObjectClass> modelClasses;
  std::unique_ptr<SettingLineEdit<std::string>> mModelPath;
  std::unique_ptr<SettingLineEdit<int32_t>> mNumberOdModelClasses;
  settings::AiClassifierSettings &mSettings;
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
      auto it = modelClasses.begin();
      for(; it != modelClasses.end(); it++) {
        if(&(it->mSettings) == obj) {
          modelClasses.erase(it);
          break;
        }
      }
    }
    updateDisplayText();
  }

  /////////////////////////////////////////////////////

private slots:
  void addClassifier()
  {
    settings::ObjectClass objClass;
    objClass.modelClassId = 0;
    auto &ret             = mSettings.modelClasses.emplace_back(objClass);
    auto *tab             = addTab("Class", [this, &ret] { removeObjectClass(&ret); });
    modelClasses.emplace_back(ret, *this, tab, mParent);
    updateDisplayText();
  }
};

}    // namespace joda::ui
