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
#include <qlineedit.h>
#include <qpushbutton.h>
#include <filesystem>
#include "backend/commands/classification/pixel_classifier/pixel_classifier.hpp"
#include "backend/commands/classification/pixel_classifier/pixel_classifier_settings.hpp"
#include "backend/commands/classification/pixel_classifier/pixel_classifier_training_settings.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/ml_model_parser/ml_model_parser.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "ui/gui/dialogs/dialog_image_view/panel_image_view.hpp"
#include "ui/gui/dialogs/dialog_ml_trainer/dialog_ml_training_ann_mlp.hpp"
#include "ui/gui/dialogs/dialog_ml_trainer/dialog_ml_training_random_forest.hpp"
#include "ui/gui/dialogs/dialog_roi_manager/table_item_delegate_polygon.hpp"
#include "ui/gui/dialogs/dialog_roi_manager/table_model_painted_polygon.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_base.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "ui/gui/helper/html_delegate.hpp"
#include "ui/gui/helper/iconless_dialog_button_box.hpp"
#include "ui/gui/helper/multicombobox.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "dialog_ml_training_knearest.hpp"

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
  setBaseSize(300, 400);
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
    connect(openMetaEditor, &QPushButton::clicked, [this] {
      if(static_cast<joda::settings::PixelClassifierMethod>(mComboClassifierMethod->currentData().toInt()) ==
         joda::settings::PixelClassifierMethod::RTrees) {
        SettingsRandomForest dialog(&mTrainerSettings.randomForest, this);
        dialog.exec();
      } else if(static_cast<joda::settings::PixelClassifierMethod>(mComboClassifierMethod->currentData().toInt()) ==
                joda::settings::PixelClassifierMethod::ANN_MLP) {
        SettingsAnnMlp dialog(&mTrainerSettings.annMlp, this);
        dialog.exec();
      } else if(static_cast<joda::settings::PixelClassifierMethod>(mComboClassifierMethod->currentData().toInt()) ==
                joda::settings::PixelClassifierMethod::KNearest) {
        SettingsKNearest dialog(&mTrainerSettings.kNearest, this);
        dialog.exec();
      }
    });
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

  {
    mRoiSource = new QComboBox();
    mRoiSource->addItem("Manual annotated objects", static_cast<int>(PaintedRoiProperties::SourceType::Manual));
    mRoiSource->addItem("Pipeline annotated objects", static_cast<int>(PaintedRoiProperties::SourceType::FromPipeline));
    mRoiSource->addItem("Any annotated object", -1);
    layout->addRow("Training data", mRoiSource);
  }

  {
    mModelName = new QLineEdit();
    layout->addRow("Filename", mModelName);
  }

  // Start training
  {
    auto *btnStartTraining = new QPushButton("Train");
    connect(btnStartTraining, &QPushButton::pressed, [this]() { startTraining(); });
    layout->addRow(btnStartTraining);
  }

  // Description
  {
    auto *tip = new QLabel(R"(<ol>
<li>Use the drawing tools to circle the regions of interest.</li>
<li>Mark background areas by drawing circles with the background class.</li>
<li>Click <b>Train</b> to start training the model.</li>
<li>Once trained, the model will be available in the Pixel Classifier command.</li>
</ol>)");
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
  std::set<PaintedRoiProperties::SourceType> filter;
  if(mRoiSource->currentData().toInt() < 0) {
    filter.emplace(PaintedRoiProperties::SourceType::Manual);
    filter.emplace(PaintedRoiProperties::SourceType::FromPipeline);
  } else {
    filter.emplace(static_cast<PaintedRoiProperties::SourceType>(mRoiSource->currentData().toInt()));
  }

  //
  // First of all we get the objects to train
  //
  atom::ObjectListWithNone annotatedObjectsToTrain;
  mImagePanel->getObjectMapFromAnnotatedRegions(filter, annotatedObjectsToTrain);

  //
  // At least one background annotation must be present
  //
  if(!annotatedObjectsToTrain.contains(enums::ClassId::NONE) || annotatedObjectsToTrain.at(enums::ClassId::NONE)->empty()) {
    QMessageBox::warning(this, "Annotation missing ...", "At least one >NONE< annotation must be taken!");
    return;
  }

  //
  // Now we generate a continuous training class id range which maps the class id to pixel class id in the format [0,1,2,3, ...]
  //
  std::map<enums::ClassId, int32_t> classesToTrainMapping;
  classesToTrainMapping.emplace(enums::ClassId::NONE, 0);    // None is always the background/zero class
  int32_t pixelClassId = 1;
  for(const auto &[classId, _] : annotatedObjectsToTrain) {
    if(classId != enums::ClassId::NONE) {
      classesToTrainMapping.emplace(classId, pixelClassId);
      pixelClassId++;
    }
  }

  //
  //
  //
  std::string modelFileName = mModelName->text().toStdString();
  if(modelFileName.empty()) {
    modelFileName = "tmp";
  }

  if(classesToTrainMapping.size() > 1) {
    std::filesystem::path modelPath =
        joda::ml::MlModelParser::getUsersMlModelDirectory() / (modelFileName + joda::fs::MASCHINE_LEARNING_OPCEN_CV_XML_MODEL);

    std::set<joda::settings::PixelClassifierFeatures> features;

    const auto &items = mComboTrainingFeatures->getCheckedItems();
    for(const auto &item : items) {
      features.emplace(static_cast<joda::settings::PixelClassifierFeatures>(item.first.toInt()));
    }

    if(features.empty()) {
      QMessageBox::warning(this, "Feature error", "At least one feature must be selected!");
      return;
    }

    mTrainerSettings.method          = static_cast<joda::settings::PixelClassifierMethod>(mComboClassifierMethod->currentData().toInt());
    mTrainerSettings.trainingClasses = classesToTrainMapping;
    mTrainerSettings.features        = features;
    mTrainerSettings.outPath         = modelPath;
    joda::cmd::PixelClassifier::train(*mImagePanel->mutableImage()->getOriginalImage(), annotatedObjectsToTrain, mTrainerSettings);
  } else {
    QMessageBox::warning(this, "Annotation missing ...", "No annotation for training found!");
  }
}

}    // namespace joda::ui::gui
