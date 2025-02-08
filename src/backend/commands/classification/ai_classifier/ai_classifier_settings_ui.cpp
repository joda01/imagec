///
/// \file      ai_classifier_settings_ui.cpp
/// \author    Joachim Danmayr
/// \date      2025-02-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "ai_classifier_settings_ui.hpp"

namespace joda::ui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
AiClassifier::AiClassifier(joda::settings::PipelineStep &pipelineStep, settings::AiClassifierSettings &settings, QWidget *parent) :
    Command(pipelineStep, TITLE.data(), ICON.data(), parent, {{InOuts::IMAGE}, {InOuts::OBJECT}}), mSettings(settings), mParent(parent)
{
  this->mutableEditDialog()->setMinimumWidth(700);
  this->mutableEditDialog()->setMinimumHeight(500);

  auto *modelTab = addTab(
      "Model settings", [] {}, false);

  auto onnxModels = joda::onnx::OnnxParser::findAiModelFiles();

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
      auto info = joda::onnx::OnnxParser::getModelInfo(std::filesystem::path(mModelPath->getValue()));
      mNetHeight->setValue(info.netInputHeight);
      mNetWidth->setValue(info.netInputWidth);
      /* removeAll();
       mNumberOdModelClasses->setValue(info.classes.size());
       int n = 0;
       for(const auto &classs : info.classes) {
         addFilter(classs, n, 1);
         n++;
       }*/
    }
  });

  //
  //
  mNumberOdModelClasses = SettingBase::create<SettingSpinBox<int32_t>>(parent, generateIcon("deviation"), "Nr. of model classes");
  mNumberOdModelClasses->setMinMax(1, 99);
  mNumberOdModelClasses->setUnit("");
  mNumberOdModelClasses->setMinMax(1, INT32_MAX);
  mNumberOdModelClasses->setValue(settings.modelClasses.size());
  mNumberOdModelClasses->connectWithSetting(&nrOfClassesTmp);
  mNumberOdModelClasses->setShortDescription("Classes:");
  mNumberOdModelClasses->setEnabled(false);
  connect(mNumberOdModelClasses.get(), &SettingBase::valueChanged, [this]() {
    // We have to add
    while(mClassifyFilter.size() < mNumberOdModelClasses->getValue()) {
      addFilter("Class " + std::to_string(mClassifyFilter.size()), mClassifyFilter.size(), 1);
    }
    // We have to remove
    while(mClassifyFilter.size() > mNumberOdModelClasses->getValue()) {
      removeTab(mClassifyFilter.size());
    }
  });

  mNetWidth = SettingBase::create<SettingLineEdit<int32_t>>(parent, {}, "Input with of the model");
  mNetWidth->setPlaceholderText("[0 - 2,147,483,647]");
  mNetWidth->setUnit("");
  mNetWidth->setMinMax(1, INT32_MAX);
  mNetWidth->setValue(settings.modelInputParameters.netInputWidth);
  mNetWidth->connectWithSetting(&settings.modelInputParameters.netInputWidth);
  mNetWidth->setShortDescription("Width:");

  mNetHeight = SettingBase::create<SettingLineEdit<int32_t>>(parent, {}, "Input height of the model");
  mNetHeight->setPlaceholderText("[0 - 2,147,483,647]");
  mNetHeight->setUnit("");
  mNetHeight->setMinMax(1, INT32_MAX);
  mNetHeight->setValue(settings.modelInputParameters.netInputHeight);
  mNetHeight->connectWithSetting(&settings.modelInputParameters.netInputHeight);
  mNetHeight->setShortDescription("Height:");

  mChannels = SettingBase::create<SettingComboBox<joda::settings::AiClassifierSettings::NetChannels>>(parent, {}, "Input channels of the model");
  mChannels->setDefaultValue(joda::settings::AiClassifierSettings::NetChannels::GRAYSCALE);
  mChannels->addOptions({{joda::settings::AiClassifierSettings::NetChannels::GRAYSCALE, "Grayscale", generateIcon("grayscale")},
                         {joda::settings::AiClassifierSettings::NetChannels::RGB, "Color", generateIcon("color")}});
  mChannels->setValue(settings.modelInputParameters.netNrOfChannels);
  mChannels->connectWithSetting(&settings.modelInputParameters.netNrOfChannels);
  mChannels->setShortDescription("Channels:");

  mClassThreshold = SettingBase::create<SettingLineEdit<float>>(parent, generateIcon("percent"), "Class threshold (0.5)");
  mClassThreshold->setPlaceholderText("[0 - 1]");
  mClassThreshold->setUnit("");
  mClassThreshold->setMinMax(0, 1);
  mClassThreshold->setValue(settings.classThreshold);
  mClassThreshold->connectWithSetting(&settings.classThreshold);

  mMaskThreshold = SettingBase::create<SettingLineEdit<float>>(parent, generateIcon("layer-mask"), "Mask threshold (0.8)");
  mMaskThreshold->setPlaceholderText("[0 - 1");
  mMaskThreshold->setUnit("");
  mMaskThreshold->setMinMax(0, 1);
  mMaskThreshold->setValue(settings.maskThreshold);
  mMaskThreshold->connectWithSetting(&settings.maskThreshold);

  auto *col  = addSetting(modelTab, "AI model settings",
                          {{mModelPath.get(), true, 0},
                           {mNumberOdModelClasses.get(), false, 0},
                           {mNetWidth.get(), false, 0},
                           {mNetHeight.get(), false, 0},
                           {mChannels.get(), false, 0}});
  auto *col2 = addSetting(modelTab, "Probabilities", {{mClassThreshold.get(), false, 0}, {mMaskThreshold.get(), false, 0}});

  int32_t cnt = 1;
  for(auto &classifierSetting : settings.modelClasses) {
    auto *tab = addTab(
        std::string("Class " + std::to_string(cnt - 1)).data(), [this, &classifierSetting] { removeObjectClass(&classifierSetting); }, false);
    mClassifyFilter.emplace_back(classifierSetting, *this, tab, cnt, parent);
    cnt++;
  }
}

}    // namespace joda::ui
