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
#include <qmessagebox.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <exception>
#include <filesystem>
#include <memory>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"
#include "backend/commands/classification/pixel_classifier/pixel_classifier.hpp"
#include "backend/commands/classification/pixel_classifier/pixel_classifier_settings.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_file_endians.hpp"
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

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
DialogMlTrainer::DialogMlTrainer(const joda::settings::AnalyzeSettings *analyzeSettings, const std::shared_ptr<atom::ObjectList> &objectMap,
                                 PanelImageView *imagePanel, QWidget *parent) :
    QDialog(parent),
    mImagePanel(imagePanel), mObjectMap(objectMap), mAnalyzeSettings(analyzeSettings)
{
  setWindowTitle("Machine learning (ALPHA)");
  setMinimumSize(300, 400);
  setBaseSize(300, 400);
  auto *layout = new QFormLayout();
  // layout->setContentsMargins(0, 0, 4, 0);

  // Settings
  {
    mComboClassifierMethod = new QComboBox();
    mComboClassifierMethod->addItem("Random forest (RTree using MLpack)", static_cast<int>(joda::ml::ModelType::RTrees));
    mComboClassifierMethod->addItem("Artificial neural network (ANN_MLP using PyTorch)", static_cast<int>(joda::ml::ModelType::ANN_MLP));

    auto *trainingSettingsMeta = new QHBoxLayout;
    trainingSettingsMeta->addWidget(mComboClassifierMethod);
    auto *openMetaEditor = new QPushButton(generateSvgIcon<Style::REGULAR, Color::BLACK>("dots-three-outline-vertical"), "");
    openMetaEditor->setStatusTip("ML model settings");
    connect(openMetaEditor, &QPushButton::clicked, [this] {
      if(static_cast<joda::ml::ModelType>(mComboClassifierMethod->currentData().toInt()) == joda::ml::ModelType::RTrees) {
        SettingsRandomForest dialog(&mModelSettings.randomForest, this);
        dialog.exec();
      } else if(static_cast<joda::ml::ModelType>(mComboClassifierMethod->currentData().toInt()) == joda::ml::ModelType::ANN_MLP) {
        SettingsAnnMlp dialog(&mModelSettings.annMlp, this);
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
    mComboTrainingFeatures->addItem("Intensity", static_cast<int>(joda::ml::TrainingFeatures::Intensity));
    mComboTrainingFeatures->addItem("Gaussian blur", static_cast<int>(joda::ml::TrainingFeatures::Gaussian));
    mComboTrainingFeatures->addItem("Laplacian of Gaussian", static_cast<int>(joda::ml::TrainingFeatures::LaplacianOfGaussian));
    mComboTrainingFeatures->addItem("Weighted deviation", static_cast<int>(joda::ml::TrainingFeatures::WeightedDeviation));
    mComboTrainingFeatures->addItem("Gradient magnitude", static_cast<int>(joda::ml::TrainingFeatures::GradientMagnitude));
    mComboTrainingFeatures->addItem("Structure tensor eigenvalues", static_cast<int>(joda::ml::TrainingFeatures::StructureTensorEigenvalues));
    mComboTrainingFeatures->addItem("Structure tensor coherence", static_cast<int>(joda::ml::TrainingFeatures::StructureTensorCoherence));
    mComboTrainingFeatures->addItem("Hessian determinant", static_cast<int>(joda::ml::TrainingFeatures::HessianDeterminant));
    mComboTrainingFeatures->addItem("Hessian eigenvalues", static_cast<int>(joda::ml::TrainingFeatures::HessianEigenvalues));

    auto *trainingSettingsMeta = new QHBoxLayout;
    trainingSettingsMeta->addWidget(mComboTrainingFeatures);
    auto *openMetaEditor = new QPushButton(generateSvgIcon<Style::REGULAR, Color::BLACK>("dots-three-outline-vertical"), "");
    openMetaEditor->setStatusTip("Training feature settings");
    connect(openMetaEditor, &QPushButton::clicked, [this] {});
    trainingSettingsMeta->addWidget(openMetaEditor);
    trainingSettingsMeta->setStretch(0, 1);    // Make label take all available space
    layout->addRow("Features", trainingSettingsMeta);

    mComboTrainingFeatures->setCheckedItems(
        {static_cast<int>(joda::ml::TrainingFeatures::Intensity), static_cast<int>(joda::ml::TrainingFeatures::Gaussian)});
  }

  {
    mRoiSource = new QComboBox();
    mRoiSource->addItem("Manual annotated objects", static_cast<int>(joda::atom::ROI::Category::MANUAL_SEGMENTATION));
    mRoiSource->addItem("Pipeline annotated objects", static_cast<int>(joda::atom::ROI::Category::AUTO_SEGMENTATION));
    mRoiSource->addItem("Any annotated object", static_cast<int>(joda::atom::ROI::Category::ANY));
    layout->addRow("Training data", mRoiSource);
  }

  {
    mModelName = new QLineEdit();
    layout->addRow("Filename", mModelName);
  }

  // Start training
  {
    mButtonStartTraining = new QPushButton("Train");
    connect(mButtonStartTraining, &QPushButton::pressed, [this]() { startTraining(); });
    layout->addRow(mButtonStartTraining);
  }

  // Stop training
  {
    mButtonStopTraining = new QPushButton("Stop");
    connect(mButtonStopTraining, &QPushButton::pressed, [this]() { stopTraining(); });
    layout->addRow(mButtonStopTraining);
  }

  // Progress bar
  {
    mProgress = new QProgressBar();
    mProgress->setMaximum(0);
    mProgress->setMinimum(0);
    layout->addRow(mProgress);
    setInProgress(false);
  }

  // Description
  {
    auto *tip = new QLabel(R"(<ol>
<li>Use the drawing tools to circle the regions of interest, or create a pipeline by doing so.</li>
<li>Mark background areas by drawing circles with the <b>None</b> class.</li>
<li>Choose the <b>Image Channel</b> that will be used for training the model.</li>
<li>Click <b>Train</b> to start training the model.</li>
<li>Once trained, the model will be available in the Pixel Classifier command.</li>
</ol>)");
    tip->setWordWrap(true);
    layout->addRow(tip);
  }

  setLayout(layout);

  QDialog::connect(this, &DialogMlTrainer::trainingFinished, this, &DialogMlTrainer::onTrainingFinished);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogMlTrainer::onTrainingFinished(bool okay, QString message)
{
  setInProgress(false);
  if(!okay) {
    QMessageBox::warning(parentWidget(), "Training error", message);
  } else {
    // QMessageBox::information(parentWidget(), "Training successful", "Training successful. Model can now be used with >Pixel Classifier< command.");
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogMlTrainer::setInProgress(bool inProgress)
{
  mButtonStartTraining->setEnabled(!inProgress);
  mProgress->setVisible(inProgress);
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
  //
  //
  //
  std::string modelFileName = mModelName->text().toStdString();
  if(modelFileName.empty()) {
    modelFileName = "tmp";
  }
  if(!mAnalyzeSettings->isProjectPathSet()) {
    QMessageBox::information(this, "Create project ...", "Please select an image directory and save the project file first!");
    return;
  }

  auto modelType          = static_cast<ml::ModelType>(mComboClassifierMethod->currentData().toInt());
  ml::Framework framework = ml::Framework::MlPack;
  std::string endian      = joda::fs::MASCHINE_LEARNING_MLPACK_RTREE;
  if(modelType == ml::ModelType::ANN_MLP) {
    endian    = joda::fs::MASCHINE_LEARNING_PYTORCH_ANN_MLP;
    framework = ml::Framework::PyTorch;
  }

  std::filesystem::path modelPath = joda::ml::MlModelParser::getUsersMlModelDirectory(mAnalyzeSettings->getProjectPath()) / (modelFileName + endian);

  std::cout << "Model path " << modelPath.string() << std::endl;
  if(std::filesystem::exists(modelPath)) {
    QMessageBox messageBox(this);
    auto icon = joda::ui::gui::generateSvgIcon<joda::ui::gui::Style::REGULAR, joda::ui::gui::Color::YELLOW>("warning-circle");
    messageBox.setIconPixmap(icon.pixmap(42, 42));
    messageBox.setWindowTitle("Override?");
    messageBox.setText("A model with same name still exists. Do you want to override it?");
    messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
    auto *noButton = messageBox.addButton(tr("No"), QMessageBox::NoRole);
    messageBox.setDefaultButton(noButton);
    messageBox.exec();
    if(messageBox.clickedButton() == noButton) {
      return;
    }
  }

  if(mTrainingsThread && mTrainingsThread->joinable()) {
    mTrainingsThread->join();
  }

  setInProgress(true);
  mTrainingsThread = std::make_unique<std::thread>([this, modelPath, framework, modelType]() {    //
    // At least one background annotation must be present
    //
    if(!mObjectMap->contains(enums::ClassId::NONE) || mObjectMap->at(enums::ClassId::NONE)->empty()) {
      emit trainingFinished(false, "At least one >NONE< annotation must be taken!");
      return;
    }

    //
    // Now we generate a continuous training class id range which maps the class id to pixel class id in the format [0,1,2,3, ...]
    //
    std::map<enums::ClassId, int32_t> classesToTrainMapping;
    classesToTrainMapping.emplace(enums::ClassId::NONE, 0);    // None is always the background/zero class
    int32_t pixelClassId = 1;
    for(const auto &[classId, _] : *mObjectMap) {
      if(classId != enums::ClassId::NONE) {
        classesToTrainMapping.emplace(classId, pixelClassId);
        pixelClassId++;
      }
    }

    if(classesToTrainMapping.size() > 1) {
      std::set<joda::ml::TrainingFeatures> features;

      const auto &items = mComboTrainingFeatures->getCheckedItems();
      for(const auto &item : items) {
        features.emplace(static_cast<joda::ml::TrainingFeatures>(item.first.toInt()));
      }

      if(features.empty()) {
        emit trainingFinished(false, "At least one feature must be selected!");
        return;
      }

      mTrainerSettings.modelTyp = modelType;
      mTrainerSettings.toClassesLabels(classesToTrainMapping);
      mTrainerSettings.trainingFeatures = features;
      mTrainerSettings.outPath          = modelPath;
      mTrainerSettings.categoryToTrain  = static_cast<joda::atom::ROI::Category>(mRoiSource->currentData().toInt());
      mTrainerSettings.framework        = framework;
      try {
        joda::cmd::PixelClassifier::train(*mImagePanel->mutableImage()->getOriginalImage(), mImagePanel->getTileInfo(), *mObjectMap, mTrainerSettings,
                                          mModelSettings);
      } catch(const std::exception &ex) {
        emit trainingFinished(false, ex.what());
        return;
      }
    } else {
      emit trainingFinished(false, "No annotation for training found!");
      return;
    }
    emit trainingFinished(true, "");
    emit triggerPreviewUpdate();
  });
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogMlTrainer::stopTraining()
{
  joda::cmd::PixelClassifier::stopTraining();
}

}    // namespace joda::ui::gui
