///
/// \file      random_forest_settings_ui.cpp
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "pixel_classifier_settings_ui.hpp"
#include "backend/helper/ml_model_parser/ml_model_parser.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
PixelClassifier::PixelClassifier(joda::settings::AnalyzeSettings *analyzeSettings, joda::settings::PipelineStep &pipelineStep,
                                 settings::PixelClassifierSettings &settings, QWidget *parent) :
    Command(analyzeSettings, pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::IMAGE}, {InOuts::BINARY}}),
    mSettings(settings), mParent(parent)
{
  auto *openModelsPath = addActionButton("Open models path", generateSvgIcon<Style::REGULAR, Color::BLACK>("arrow-square-out"));
  connect(openModelsPath, &QAction::triggered, [this]() {
    QString appDirPath = joda::ml::MlModelParser::getUsersMlModelDirectory(getWorkingDirectory()).string().c_str();
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
  mGpuMode = SettingBase::create<SettingComboBox<settings::PixelClassifierSettings::GpuUsage>>(parent, {}, "GPU usage");
  mGpuMode->addOptions(
      {{settings::PixelClassifierSettings::GpuUsage::Auto, "Auto"}, {settings::PixelClassifierSettings::GpuUsage::CpuOnly, "CPU only"}});
  mGpuMode->setValue(settings.gpuUsage);
  mGpuMode->connectWithSetting(&settings.gpuUsage);

  //
  //
  //
  mModelDetails = new QLabel();

  auto *col = addSetting(modelTab, "Model settings",
                         {
                             {mModelPath.get(), true, 0},
                             {mGpuMode.get(), false, 0},
                         });

  addWidgets(modelTab, "Model details", {mModelDetails}, col);

  updateModel();

  registerProjectPathChangedCallback([this](const std::filesystem::path & /*path*/) {
    refreshModels();
    mModelPath->setValue(mSettings.modelPath);
    updateModel();
  });
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PixelClassifier::refreshModels()
{
  auto modelsPath = joda::ml::MlModelParser::findMlModelFiles(getWorkingDirectory());
  std::vector<SettingComboBoxString::ComboEntry> entries;
  entries.reserve(modelsPath.size() + 1);
  entries.emplace_back(SettingComboBoxString::ComboEntry{.key = "", .label = "Select model ..."});
  for(const auto &[key, model] : modelsPath) {
    entries.emplace_back(SettingComboBoxString::ComboEntry{.key = model.modelPath.string(), .label = model.modelName.data()});
  }
  mModelPath->addOptions(entries);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PixelClassifier::updateModel()
{
  if(!mModelPath->getValue().empty()) {
    try {
      auto info = joda::ml::MlModelParser::parseOpenCVModelXMLDescriptionFile(std::filesystem::path(mModelPath->getValue()), getWorkingDirectory());
      mModelDetails->setText(info.toString().data());
    } catch(...) {
    }
  }
}
}    // namespace joda::ui::gui
