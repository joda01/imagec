///
/// \file      ai_classifier_settings_ui.cpp
/// \author    Joachim Danmayr
/// \date      2025-02-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "ai_classifier_settings_ui.hpp"
#include "ui/gui/helper/icon_generator.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
AiClassifier::AiClassifier(joda::settings::PipelineStep &pipelineStep, settings::AiClassifierSettings &settings, QWidget *parent) :
    Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::IMAGE}, {InOuts::OBJECT}}), mSettings(settings),
    mParent(parent)
{
  this->mutableEditDialog()->setMinimumWidth(700);
  this->mutableEditDialog()->setMinimumHeight(600);

  auto *openModelsPath = addActionButton("Open models path", generateSvgIcon<Style::REGULAR, Color::BLACK>("arrow-square-out"));
  connect(openModelsPath, &QAction::triggered, []() {
    QString appDirPath = QCoreApplication::applicationDirPath() + "/models";
    QDesktopServices::openUrl(QUrl("file:///" + appDirPath));
  });

  auto *reloadModels = addActionButton("Reload models", generateSvgIcon<Style::REGULAR, Color::BLACK>("arrows-clockwise"));
  connect(reloadModels, &QAction::triggered, [this]() { refreshModels(); });

  auto *helpButton = addActionButton("Help", generateSvgIcon<Style::REGULAR, Color::BLUE>("question"));
  connect(helpButton, &QAction::triggered, []() {
    QUrl url("https://imagec.org/doc/docs/commands/index.html#object-classification");
    QDesktopServices::openUrl(url);
  });

  addSeparatorToTopToolbar();

  auto *modelTab = addTab(
      "Model settings", [] {}, false);

  mModelPath = SettingBase::create<SettingComboBoxString>(parent, {}, "Model path");
  refreshModels();

  mModelPath->setValue(settings.modelPath);
  mModelPath->connectWithSetting(&settings.modelPath);
  mModelPath->setShortDescription("Path:");

  connect(mModelPath.get(), &SettingBase::valueChanged, [&]() { updateModel(); });
  //
  //
  mNumberOdModelClasses = SettingBase::create<SettingSpinBox<int32_t>>(parent, {}, "Nr. of model classes");
  mNumberOdModelClasses->setUnit("", enums::ObjectType::Undefined);
  mNumberOdModelClasses->setMinMax(1, INT32_MAX);
  mNumberOdModelClasses->setValue(static_cast<int32_t>(settings.modelClasses.size()));
  mNumberOdModelClasses->connectWithSetting(&nrOfClassesTmp);
  mNumberOdModelClasses->setShortDescription("Classes:");
  mNumberOdModelClasses->setEnabled(false);
  connect(mNumberOdModelClasses.get(), &SettingBase::valueChanged, [this]() {
    // We have to add
    while(static_cast<int32_t>(mClassifyFilter.size()) < mNumberOdModelClasses->getValue()) {
      addFilter("Class " + std::to_string(mClassifyFilter.size()), static_cast<int32_t>(mClassifyFilter.size()), 1);
    }
    // We have to remove
    while(static_cast<int32_t>(mClassifyFilter.size()) > mNumberOdModelClasses->getValue()) {
      removeTab(static_cast<int32_t>(mClassifyFilter.size()));
    }
  });

  mNetWidth = SettingBase::create<SettingLineEdit<int32_t>>(parent, {}, "Input width of the model");
  mNetWidth->setPlaceholderText("[0 - 2,147,483,647]");
  mNetWidth->setUnit("");
  mNetWidth->setMinMax(1, INT32_MAX);
  mNetWidth->setValue(settings.modelInputParameter.spaceX);
  mNetWidth->connectWithSetting(&settings.modelInputParameter.spaceX);
  mNetWidth->setShortDescription("Width:");
  mNetWidth->getEditableWidget()->setEnabled(false);

  mNetHeight = SettingBase::create<SettingLineEdit<int32_t>>(parent, {}, "Input height of the model");
  mNetHeight->setPlaceholderText("[0 - 2,147,483,647]");
  mNetHeight->setUnit("");
  mNetHeight->setMinMax(1, INT32_MAX);
  mNetHeight->setValue(settings.modelInputParameter.spaceY);
  mNetHeight->connectWithSetting(&settings.modelInputParameter.spaceY);
  mNetHeight->setShortDescription("Height:");
  mNetHeight->getEditableWidget()->setEnabled(false);

  mChannels = SettingBase::create<SettingComboBox<joda::settings::AiClassifierSettings::NetChannels>>(parent, {}, "Input channels of the model");
  mChannels->setDefaultValue(joda::settings::AiClassifierSettings::NetChannels::GRAYSCALE);
  mChannels->addOptions({{joda::settings::AiClassifierSettings::NetChannels::GRAYSCALE, "Grayscale"},
                         {joda::settings::AiClassifierSettings::NetChannels::RGB, "Color"}});
  mChannels->setValue(settings.modelInputParameter.channels);
  mChannels->connectWithSetting(&settings.modelInputParameter.channels);
  mChannels->setShortDescription("Channels:");
  mChannels->getEditableWidget()->setEnabled(false);

  //
  //
  ////
  mModelFormat = SettingBase::create<SettingComboBox<joda::settings::AiClassifierSettings::ModelFormat>>(parent, {}, "Model format");
  mModelFormat->setDefaultValue(joda::settings::AiClassifierSettings::ModelFormat::ONNX);
  mModelFormat->addOptions({{joda::settings::AiClassifierSettings::ModelFormat::UNKNOWN, "Unknown"},
                            {joda::settings::AiClassifierSettings::ModelFormat::ONNX, "Onnx", generateIcon("onnx")},
                            {joda::settings::AiClassifierSettings::ModelFormat::TORCHSCRIPT, "Torchscript", generateIcon("pytorch")},
                            {joda::settings::AiClassifierSettings::ModelFormat::TENSORFLOW, "Tensorflow", generateIcon("tensorflow")}

  });
  mModelFormat->setValue(settings.modelParameter.modelFormat);
  mModelFormat->connectWithSetting(&settings.modelParameter.modelFormat);
  mModelFormat->setShortDescription("Format:");

  //
  //
  //
  mModelArchitecture =
      SettingBase::create<SettingComboBox<joda::settings::AiClassifierSettings::ModelArchitecture>>(parent, {}, "Model architecture");
  mModelArchitecture->setDefaultValue(joda::settings::AiClassifierSettings::ModelArchitecture::YOLO_V5);
  mModelArchitecture->addOptions({
      {joda::settings::AiClassifierSettings::ModelArchitecture::UNKNOWN, "Unknown"},
      {joda::settings::AiClassifierSettings::ModelArchitecture::YOLO_V5, "Yolo v5", generateIcon("connect")},
      {joda::settings::AiClassifierSettings::ModelArchitecture::U_NET, "U-Net", generateIcon("u")},
      {joda::settings::AiClassifierSettings::ModelArchitecture::CYTO3, "Cyto3", generateIcon("cellpose")},
      {joda::settings::AiClassifierSettings::ModelArchitecture::STAR_DIST, "StarDist", generateIcon("star-color")},
      //{joda::settings::AiClassifierSettings::ModelArchitecture::MASK_R_CNN, "Mask R-CNN", generateIcon("connect")}
  });
  mModelArchitecture->setValue(settings.modelParameter.modelArchitecture);
  mModelArchitecture->connectWithSetting(&settings.modelParameter.modelArchitecture);
  mModelArchitecture->setShortDescription("Architecture:");
  connect(mModelArchitecture.get(), &SettingBase::valueChanged, [this]() {
    mMaskThreshold->setValue(0.8F);
    if(mModelArchitecture->getValue() == joda::settings::AiClassifierSettings::ModelArchitecture::YOLO_V5) {
      mMaskThreshold->setValue(0.8F);
    } else if(mModelArchitecture->getValue() == joda::settings::AiClassifierSettings::ModelArchitecture::U_NET ||
              mModelArchitecture->getValue() == joda::settings::AiClassifierSettings::ModelArchitecture::STAR_DIST) {
      mMaskThreshold->setValue(0.96F);
    }
  });

  //
  //
  //
  mClassThreshold = SettingBase::create<SettingLineEdit<float>>(parent, {}, "Class threshold. Default: Yolo: 0.5");
  mClassThreshold->setPlaceholderText("[0 - 1]");
  mClassThreshold->setUnit("");
  mClassThreshold->setMinMax(0, 1);
  mClassThreshold->setValue(settings.thresholds.classThreshold);
  mClassThreshold->connectWithSetting(&settings.thresholds.classThreshold);

  //
  //
  //
  mMaskThreshold = SettingBase::create<SettingLineEdit<float>>(parent, {}, "Mask threshold. Default: U-Net 0.96, Yolo: 0.8");
  mMaskThreshold->setPlaceholderText("[0 - 1");
  mMaskThreshold->setUnit("");
  mMaskThreshold->setMinMax(0, 1);
  mMaskThreshold->setValue(settings.thresholds.maskThreshold);
  mMaskThreshold->connectWithSetting(&settings.thresholds.maskThreshold);

  //
  //
  //
  mModelDetails = new QLabel();

  auto *col = addSetting(modelTab, "Model settings",
                         {
                             {mModelPath.get(), true, 0},
                             {mModelFormat.get(), false, 0},
                             {mModelArchitecture.get(), false, 0},
                             {mNumberOdModelClasses.get(), false, 0},
                         });

  addWidgets(modelTab, "Model details", {mModelDetails}, col);

  auto *col2 = addSetting(modelTab, "Input parameters", {{mNetWidth.get(), false, 0}, {mNetHeight.get(), false, 0}, {mChannels.get(), false, 0}});
  addSetting(modelTab, "Thresholds", {{mMaskThreshold.get(), false, 0}, {mClassThreshold.get(), false, 0}}, col2);

  int32_t cnt = 1;
  for(auto &classifierSetting : settings.modelClasses) {
    auto *tab = addTab(
        static_cast<std::string>("Class " + std::to_string(cnt - 1)).data(), [this, &classifierSetting] { removeObjectClass(&classifierSetting); },
        false);
    mClassifyFilter.emplace_back(classifierSetting, *this, tab, cnt, parent);
    cnt++;
  }

  updateModel();
}

void AiClassifier::updateInputFields(int32_t nrOfClasses, const settings::AiClassifierSettings::ModelParameters &model,
                                     const settings::AiClassifierSettings::NetInputParameters &settings)
{
  mNetWidth->setValue(settings.spaceX);
  mNetHeight->setValue(settings.spaceY);
  mChannels->setValue(settings.channels);
  mModelFormat->setValue(model.modelFormat);
  mModelArchitecture->setValue(model.modelArchitecture);

  mNumberOdModelClasses->setValue(nrOfClasses);
}

void AiClassifier::refreshModels()
{
  auto onnxModels = joda::ai::AiModelParser::findAiModelFiles();
  std::vector<SettingComboBoxString::ComboEntry> entries;
  entries.reserve(onnxModels.size() + 1);
  entries.emplace_back(SettingComboBoxString::ComboEntry{.key = "", .label = "Select model ..."});
  for(const auto &[key, model] : onnxModels) {
    entries.emplace_back(SettingComboBoxString::ComboEntry{.key = model.modelPath.string(), .label = model.modelName.data()});
  }
  mModelPath->addOptions(entries);
}

void AiClassifier::updateModel()
{
  if(!mModelPath->getValue().empty()) {
    try {
      auto info = joda::ai::AiModelParser::parseResourceDescriptionFile(std::filesystem::path(mModelPath->getValue()));
      mModelDetails->setText(info.toString().data());
      mSettings.modelInputParameter = info.inputs.begin()->second;
      updateInputFields(static_cast<int32_t>(info.classes.size()), info.modelParameter, info.inputs.begin()->second);
      /* removeAll();
       mNumberOdModelClasses->setValue(info.classes.size());
       int n = 0;
       for(const auto &classs : info.classes) {
         addFilter(classs, n, 1);
         n++;
       }*/
    } catch(...) {
    }
  }
}

}    // namespace joda::ui::gui
