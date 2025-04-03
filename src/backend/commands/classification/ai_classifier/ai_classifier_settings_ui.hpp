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
#include <string>
#include "backend/commands/classification/classifier_filter.hpp"
#include "backend/commands/command.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/ai_model_parser/ai_model_parser.hpp"
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/gui/container/setting/setting_combobox_string.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/container/setting/setting_spinbox.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "ai_classifier_settings.hpp"

namespace joda::ui::gui {

class AiClassifier : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "AI Classifier";
  inline static std::string ICON              = "magic";
  inline static std::string DESCRIPTION       = "Extract objects from an image using AI.";
  inline static std::vector<std::string> TAGS = {"classifier", "classify", "objects", "ai", "feature extraction", "pattern recognition"};

  AiClassifier(joda::settings::PipelineStep &pipelineStep, settings::AiClassifierSettings &settings, QWidget *parent);

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
                           mMinIntensity.get(), mMaxIntensity.get(), mProbabilityHandicap.get()});
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
    std::unique_ptr<SettingSpinBox<int32_t>> zStackIndex;
    std::unique_ptr<SettingLineEdit<int>> mMinIntensity;
    std::unique_ptr<SettingLineEdit<int>> mMaxIntensity;

    std::unique_ptr<SettingLineEdit<float>> mProbabilityHandicap;

    settings::ObjectClass &settings;
    AiClassifier &outer;
    helper::TabWidget *tab;
  };

  std::unique_ptr<SettingComboBoxString> mModelPath;
  std::unique_ptr<SettingSpinBox<int32_t>> mNumberOdModelClasses;
  std::unique_ptr<SettingLineEdit<int32_t>> mNetWidth;
  std::unique_ptr<SettingLineEdit<int32_t>> mNetHeight;
  std::unique_ptr<SettingComboBox<joda::settings::AiClassifierSettings::NetChannels>> mChannels;

  std::unique_ptr<SettingComboBox<joda::settings::AiClassifierSettings::ModelFormat>> mModelFormat;
  std::unique_ptr<SettingComboBox<joda::settings::AiClassifierSettings::ModelArchitecture>> mModelArchitecture;

  std::unique_ptr<SettingLineEdit<float>> mClassThreshold;
  std::unique_ptr<SettingLineEdit<float>> mMaskThreshold;

  std::list<ClassifierFilter> mClassifyFilter;
  settings::AiClassifierSettings &mSettings;
  QWidget *mParent;
  int32_t nrOfClassesTmp;

  QLabel *mModelDetails;

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
  void updateInputFields(int32_t nrOfClasses, const settings::AiClassifierSettings::ModelParameters &model,
                         const settings::AiClassifierSettings::NetInputParameters &settings);

  void refreshModels();
  void updateModel();
private slots:
  void addFilter(const std::string &title, int32_t classId, float handicap)
  {
    settings::ObjectClass objClass;
    objClass.modelClassId        = classId;
    objClass.probabilityHandicap = handicap;
    auto &ret                    = mSettings.modelClasses.emplace_back(objClass);
    auto *tab                    = addTab(
        title.data(), [this, &ret] { removeObjectClass(&ret); }, false);
    mClassifyFilter.emplace_back(ret, *this, tab, mSettings.modelClasses.size(), mParent);
    updateDisplayText();
  }
};

}    // namespace joda::ui::gui
