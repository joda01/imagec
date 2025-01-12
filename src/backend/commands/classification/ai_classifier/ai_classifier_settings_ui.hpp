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
#include "backend/helper/onnx_parser/onnx_parser.hpp"
#include "ui/container/command/command.hpp"
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/container/setting/setting_combobox_string.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/icon_generator.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/helper/setting_generator.hpp"
#include "ai_classifier_settings.hpp"

namespace joda::ui {

class AiClassifier : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "AI Classifier";
  inline static std::string ICON  = "magic";

  AiClassifier(joda::settings::PipelineStep &pipelineStep, settings::AiClassifierSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent, {{InOuts::IMAGE}, {InOuts::OBJECT}}), mSettings(settings), mParent(parent)
  {
    this->mutableEditDialog()->setMinimumWidth(600);
    this->mutableEditDialog()->setMinimumHeight(400);

    auto *modelTab = addTab("Model", [] {});

    auto onnxModels = joda::onnx::OnnxParser::findOnnxFiles();

    std::vector<SettingComboBoxString::ComboEntry> entries;
    entries.reserve(onnxModels.size() + 1);
    entries.emplace_back(SettingComboBoxString::ComboEntry{.key = "", .label = "Select model ..."});
    for(const auto &[key, model] : onnxModels) {
      entries.emplace_back(SettingComboBoxString::ComboEntry{.key = model.modelPath.string(), .label = model.modelName.data()});
    }

    mModelPath = SettingBase::create<SettingComboBoxString>(parent, {}, "Model path");
    mModelPath->addOptions(entries);
    mModelPath->connectWithSetting(&settings.modelPath);
    mModelPath->setValue(settings.modelPath);
    mModelPath->setShortDescription("Path:");
    connect(mModelPath.get(), &SettingBase::valueChanged, [this]() {
      if(!mModelPath->getValue().empty()) {
        auto info = joda::onnx::OnnxParser::getOnnxInfo(std::filesystem::path(mModelPath->getValue()));
        removeAll();
        mNumberOdModelClasses->setValue(info.classes.size());
        int n = 0;
        for(const auto &classs : info.classes) {
          addFilter(classs, n, 1);
          n++;
        }
      }
    });

    //
    //
    mNumberOdModelClasses = SettingBase::create<SettingLineEdit<int32_t>>(parent, {}, "Nr. of model classes");
    mNumberOdModelClasses->setPlaceholderText("[0 - 2,147,483,647]");
    mNumberOdModelClasses->setUnit("");
    mNumberOdModelClasses->setMinMax(1, INT32_MAX);
    mNumberOdModelClasses->setValue(settings.numberOfModelClasses);
    mNumberOdModelClasses->connectWithSetting(&settings.numberOfModelClasses);
    mNumberOdModelClasses->setShortDescription("Classes:");
    mNumberOdModelClasses->setEnabled(false);

    mClassThreshold = SettingBase::create<SettingLineEdit<float>>(parent, {}, "Class threshold (0.5)");
    mClassThreshold->setPlaceholderText("[0 - 1]");
    mClassThreshold->setUnit("");
    mClassThreshold->setMinMax(0, 1);
    mClassThreshold->setValue(settings.classThreshold);
    mClassThreshold->connectWithSetting(&settings.classThreshold);

    mMaskThreshold = SettingBase::create<SettingLineEdit<float>>(parent, {}, "Mask threshold (0.8)");
    mMaskThreshold->setPlaceholderText("[0 - 1");
    mMaskThreshold->setUnit("");
    mMaskThreshold->setMinMax(0, 1);
    mMaskThreshold->setValue(settings.maskThreshold);
    mMaskThreshold->connectWithSetting(&settings.maskThreshold);

    auto *col  = addSetting(modelTab, "AI model settings", {{mModelPath.get(), true, 0}, {mNumberOdModelClasses.get(), false, 0}});
    auto *col2 = addSetting(modelTab, "Probabilities", {{mClassThreshold.get(), false, 0}, {mMaskThreshold.get(), false, 0}});

    int32_t cnt = 0;
    for(auto &classifierSetting : settings.modelClasses) {
      auto *tab = addTab("Filter", [this, &classifierSetting] { removeObjectClass(&classifierSetting); });
      mClassifyFilter.emplace_back(classifierSetting, *this, tab, cnt, parent);
      cnt++;
    }
  }

private:
  /////////////////////////////////////////////////////

  struct ClassifierFilter
  {
    ClassifierFilter(settings::ObjectClass &settings, AiClassifier &outer, helper::TabWidget *tab, int32_t tabIndex, QWidget *parent) :
        outer(outer), tab(tab), settings(settings)
    {
      if(settings.filters.empty()) {
        settings.filters.emplace_back(settings::ClassifierFilter{});
      }
      auto &classifyFilter = *settings.filters.begin();

      //
      //
      mGrayScaleValue = generateAiModelClass("AI class input", parent);
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

      //
      mProbabilityHandicap = SettingBase::create<SettingLineEdit<float>>(parent, {}, "Probability handicap (1)");
      mProbabilityHandicap->setPlaceholderText("[0 - 256]");
      mProbabilityHandicap->setUnit("");
      mProbabilityHandicap->setMinMax(0, 256);
      mProbabilityHandicap->setValue(settings.probabilityHandicap);
      mProbabilityHandicap->connectWithSetting(&settings.probabilityHandicap);

      outer.addSetting(tab, "Result output",
                       {{mClassOut.get(), true, tabIndex}, {mClassOutNoMatch.get(), true, tabIndex}, {mProbabilityHandicap.get(), false, tabIndex}});

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
                           mMaxIntensity.get(), mProbabilityHandicap.get()});
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

    std::unique_ptr<SettingLineEdit<float>> mProbabilityHandicap;

    settings::ObjectClass &settings;
    AiClassifier &outer;
    helper::TabWidget *tab;
  };

  std::unique_ptr<SettingComboBoxString> mModelPath;
  std::unique_ptr<SettingLineEdit<int32_t>> mNumberOdModelClasses;
  std::unique_ptr<SettingLineEdit<float>> mClassThreshold;
  std::unique_ptr<SettingLineEdit<float>> mMaskThreshold;

  std::list<ClassifierFilter> mClassifyFilter;
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

  void removeAll()
  {
    removeAllTabsExceptFirst();
  }

  /////////////////////////////////////////////////////

private slots:
  void addFilter(const std::string &title, int32_t classId, float handicap)
  {
    settings::ObjectClass objClass;
    objClass.modelClassId        = classId;
    objClass.probabilityHandicap = handicap;
    auto &ret                    = mSettings.modelClasses.emplace_back(objClass);
    auto *tab                    = addTab(title.data(), [this, &ret] { removeObjectClass(&ret); });
    mClassifyFilter.emplace_back(ret, *this, tab, mSettings.modelClasses.size(), mParent);
    updateDisplayText();
  }
};

}    // namespace joda::ui
