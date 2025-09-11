///
/// \file      dialog_roi_manager.cpp
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "dialog_ml_trainer.hpp"
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qformlayout.h>
#include <qpushbutton.h>
#include <filesystem>
#include "backend/commands/classification/pixel_classifier/pixel_classifier.hpp"
#include "backend/commands/classification/pixel_classifier/pixel_classifier_settings.hpp"
#include "backend/commands/classification/pixel_classifier/pixel_classifier_training_settings.hpp"
#include "backend/commands/classification/pixel_classifier/random_forest/random_forest.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/ml_model_parser/ml_model_parser.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "ui/gui/dialogs/dialog_image_view/panel_image_view.hpp"
#include "ui/gui/dialogs/dialog_roi_manager/table_item_delegate_polygon.hpp"
#include "ui/gui/dialogs/dialog_roi_manager/table_model_painted_polygon.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_base.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "ui/gui/helper/html_delegate.hpp"
#include "ui/gui/helper/iconless_dialog_button_box.hpp"
#include "ui/gui/helper/multicombobox.hpp"
#include "ui/gui/helper/setting_generator.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
DialogMlTrainer::DialogMlTrainer(PanelImageView *imagePanel, QWidget *parent) : QDialog(parent), mImagePanel(imagePanel)
{
  setWindowTitle("Machine learning");
  setMinimumSize(300, 400);
  setMaximumWidth(300);
  auto *layout = new QFormLayout();
  // layout->setContentsMargins(0, 0, 4, 0);

  // Settings
  {
    mComboClassifierMethod = new QComboBox();
    mComboClassifierMethod->addItem("Random forest (RTree)", static_cast<int>(joda::settings::PixelClassifierMethod::RTrees));
    mComboClassifierMethod->addItem("Artificial neural network (ANN_MLP)", static_cast<int>(joda::settings::PixelClassifierMethod::ANN_MLP));
    mComboClassifierMethod->addItem("K nearest neighbor", static_cast<int>(joda::settings::PixelClassifierMethod::KNearest));

    auto *trainingSettingsMeta = new QHBoxLayout;
    trainingSettingsMeta->addWidget(mComboClassifierMethod);
    auto *openMetaEditor = new QPushButton(generateSvgIcon<Style::REGULAR, Color::BLACK>("dots-three-outline-vertical"), "");
    openMetaEditor->setStatusTip("ML model settings");
    connect(openMetaEditor, &QPushButton::clicked, [this] {});
    trainingSettingsMeta->addWidget(openMetaEditor);
    trainingSettingsMeta->setStretch(0, 1);    // Make label take all available space
    layout->addRow("Classifier", trainingSettingsMeta);
  }

  // Features
  {
    mComboTrainingFeatures = new QComboBoxMulti();
    mComboTrainingFeatures->addItem("Intensity", static_cast<int>(joda::settings::PixelClassifierFeatures::Intensity));
    mComboTrainingFeatures->addItem("Gaussian blur", static_cast<int>(joda::settings::PixelClassifierFeatures::Gaussian));
    mComboTrainingFeatures->addItem("Laplacian of Gaussian", static_cast<int>(joda::settings::PixelClassifierFeatures::LaplacianOfGaussian));
    mComboTrainingFeatures->addItem("Weighted deviation", static_cast<int>(joda::settings::PixelClassifierFeatures::WeightedDeviation));
    mComboTrainingFeatures->addItem("Gradient magnitude", static_cast<int>(joda::settings::PixelClassifierFeatures::GradientMagnitude));
    mComboTrainingFeatures->addItem("Structure tensor eigenvalues",
                                    static_cast<int>(joda::settings::PixelClassifierFeatures::StructureTensorEigenvalues));
    mComboTrainingFeatures->addItem("Structure tensor coherence",
                                    static_cast<int>(joda::settings::PixelClassifierFeatures::StructureTensorCoherence));
    mComboTrainingFeatures->addItem("Hessian determinant", static_cast<int>(joda::settings::PixelClassifierFeatures::HessianDeterminant));
    mComboTrainingFeatures->addItem("Hessian eigenvalues", static_cast<int>(joda::settings::PixelClassifierFeatures::HessianEigenvalues));

    auto *trainingSettingsMeta = new QHBoxLayout;
    trainingSettingsMeta->addWidget(mComboTrainingFeatures);
    auto *openMetaEditor = new QPushButton(generateSvgIcon<Style::REGULAR, Color::BLACK>("dots-three-outline-vertical"), "");
    openMetaEditor->setStatusTip("Training feature settings");
    connect(openMetaEditor, &QPushButton::clicked, [this] {});
    trainingSettingsMeta->addWidget(openMetaEditor);
    trainingSettingsMeta->setStretch(0, 1);    // Make label take all available space
    layout->addRow("Features", trainingSettingsMeta);

    mComboTrainingFeatures->setCheckedItems(
        {static_cast<int>(joda::settings::PixelClassifierFeatures::Intensity), static_cast<int>(joda::settings::PixelClassifierFeatures::Gaussian)});
  }

  // Start training
  {
    auto *btnStartTraining = new QPushButton("Train");
    connect(btnStartTraining, &QPushButton::pressed, [this]() { startTraining(); });
    layout->addRow(btnStartTraining);
  }

  // Description
  {
    auto *tip = new QLabel(
        "Use the ROI manager and draw shapes around the ares in the image you want to detect. Also use the ROI manager to draw shapes "
        "around some background ares. Once done select one of the machine learning models and press >Train< to train a model which can be "
        "used in the pipeline Pixel Classifier command afterwards.");
    tip->setWordWrap(true);
    layout->addRow(tip);
  }

  setLayout(layout);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogMlTrainer::hideEvent(QHideEvent *event)
{
  // std::cout << "Hide" << std::endl;
  // mMoveAction->setChecked(true);
  // mImagePanel->setState(PanelImageView::State::MOVE);
  // emit dialogDisappeared();
  QDialog::hideEvent(event);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogMlTrainer::closeEvent(QCloseEvent *event)
{
  emit dialogDisappeared();
  QDialog::closeEvent(event);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogMlTrainer::startTraining()
{
  atom::ObjectList objectList;
  mImagePanel->getObjectMapFromAnnotatedRegions(objectList);
  std::set<int32_t> classesToTrain;

  for(const auto &[classId, _] : objectList) {
    classesToTrain.emplace(static_cast<int32_t>(classId));
  }

  if(classesToTrain.size() > 1) {
    std::filesystem::path modelPath = joda::ml::MlModelParser::getUsersMlModelDirectory() / ("tmp" + joda::fs::MASCHINE_LEARNING_OPCEN_CV_XML_MODEL);

    std::set<joda::settings::PixelClassifierFeatures> features;

    const auto &items = mComboTrainingFeatures->getCheckedItems();
    for(const auto &item : items) {
      features.emplace(static_cast<joda::settings::PixelClassifierFeatures>(item.first.toInt()));
    }

    if(features.empty()) {
      QMessageBox::warning(this, "Feature error", "At least one feature must be selected!", QMessageBox::Yes | QMessageBox::No);
      return;
    }

    joda::settings::PixelClassifierTrainingSettings settings{
        .trainingClasses = classesToTrain,
        .method          = static_cast<joda::settings::PixelClassifierMethod>(mComboClassifierMethod->currentData().toInt()),
        .features        = features,
        .outPath         = modelPath,
        .randomForest    = std::nullopt};
    joda::cmd::PixelClassifier::train(*mImagePanel->mutableImage()->getOriginalImage(), objectList, settings);
  }
}

}    // namespace joda::ui::gui
