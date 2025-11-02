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
#include "backend/commands/image_functions/edge_detection_sobel/edge_detection_sobel_settings.hpp"
#include "backend/commands/image_functions/laplacian/laplacian_settings.hpp"
#include "backend/commands/image_functions/nop/nop_settings.hpp"
#include "backend/commands/image_functions/weighted_deviation/weighted_deviation_settings.hpp"
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
    mComboTrainingFeatures->addItem("Intensity", static_cast<int>(TrainingFeatures::Intensity));
    mComboTrainingFeatures->addItem("Gaussian blur", static_cast<int>(TrainingFeatures::Gaussian));
    mComboTrainingFeatures->addItem("Laplacian of Gaussian", static_cast<int>(TrainingFeatures::LaplacianOfGaussian));
    mComboTrainingFeatures->addItem("Weighted deviation", static_cast<int>(TrainingFeatures::WeightedDeviation));
    mComboTrainingFeatures->addItem("Gradient magnitude", static_cast<int>(TrainingFeatures::GradientMagnitude));
    // mComboTrainingFeatures->addItem("Structure tensor eigenvalues", static_cast<int>(TrainingFeatures::StructureTensorEigenvalues));
    // mComboTrainingFeatures->addItem("Structure tensor coherence", static_cast<int>(TrainingFeatures::StructureTensorCoherence));
    // mComboTrainingFeatures->addItem("Hessian determinant", static_cast<int>(TrainingFeatures::HessianDeterminant));
    // mComboTrainingFeatures->addItem("Hessian eigenvalues", static_cast<int>(TrainingFeatures::HessianEigenvalues));

    auto *trainingSettingsMeta = new QHBoxLayout;
    trainingSettingsMeta->addWidget(mComboTrainingFeatures);
    auto *openMetaEditor = new QPushButton(generateSvgIcon<Style::REGULAR, Color::BLACK>("dots-three-outline-vertical"), "");
    openMetaEditor->setStatusTip("Training feature settings");
    connect(openMetaEditor, &QPushButton::clicked, [this] {});
    trainingSettingsMeta->addWidget(openMetaEditor);
    trainingSettingsMeta->setStretch(0, 1);    // Make label take all available space
    layout->addRow("Features", trainingSettingsMeta);

    mComboTrainingFeatures->setCheckedItems({static_cast<int>(TrainingFeatures::Intensity), static_cast<int>(TrainingFeatures::Gaussian)});
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
    mButtonStopTraining->setVisible(false);
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

  {
    mTrainingsLog = new QTextBrowser();
    layout->addRow(mTrainingsLog);

    joda::cmd::PixelClassifier::registerProgressCallback([this](const std::string &progress) {
      QString text = QString::fromStdString(progress);
      QMetaObject::invokeMethod(
          mTrainingsLog,
          [this, text]() {
            mTrainingsLog->append(text);
            mTrainingsLog->moveCursor(QTextCursor::End);
            mTrainingsLog->ensureCursorVisible();
          },
          Qt::QueuedConnection    // 👈 ensures it runs in the GUI thread
      );
    });
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
  mButtonStartTraining->setVisible(!inProgress);
  mButtonStopTraining->setVisible(inProgress);
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
  mTrainingsLog->clear();
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
      buildFeatureExtractionPipeline();
      if(mTrainerSettings.featureExtractionPipelines.empty()) {
        emit trainingFinished(false, "At least one feature must be selected!");
        return;
      }

      mTrainerSettings.modelTyp = modelType;
      mTrainerSettings.toClassesLabels(classesToTrainMapping);
      mTrainerSettings.outPath         = modelPath;
      mTrainerSettings.categoryToTrain = static_cast<joda::atom::ROI::Category>(mRoiSource->currentData().toInt());
      mTrainerSettings.framework       = framework;
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
void DialogMlTrainer::buildFeatureExtractionPipeline()
{
  std::set<TrainingFeatures> features;

  const auto &items = mComboTrainingFeatures->getCheckedItems();
  for(const auto &item : items) {
    features.emplace(static_cast<TrainingFeatures>(item.first.toInt()));
  }

  mTrainerSettings.featureExtractionPipelines.clear();

  // ====================================
  // Build extraction pipeline
  // ====================================

  // --- Intensity ---
  if(features.contains(TrainingFeatures::Intensity)) {
    ml::ImageCommandPipeline cmds;
    {
      joda::settings::NopSettings nop;
      nop.repeat = 0;
      cmds.pipelineSteps.emplace_back(settings::PipelineStep{.$nop = nop});
    }
    mTrainerSettings.featureExtractionPipelines.push_back(cmds);
  }

  // --- Gaussian smoothed ---
  if(features.contains(TrainingFeatures::Gaussian)) {
    ml::ImageCommandPipeline cmds;
    {
      joda::settings::BlurSettings blur;
      blur.mode       = joda::settings::BlurSettings::Mode::GAUSSIAN;
      blur.kernelSize = 5;
      blur.repeat     = 1;
      cmds.pipelineSteps.emplace_back(settings::PipelineStep{.$blur = blur});
    }
    mTrainerSettings.featureExtractionPipelines.push_back(cmds);
  }

  // --- Laplacian of Gaussian ---
  if(features.contains(TrainingFeatures::LaplacianOfGaussian)) {
    ml::ImageCommandPipeline cmds;
    {
      joda::settings::BlurSettings blur;
      blur.mode       = joda::settings::BlurSettings::Mode::GAUSSIAN;
      blur.kernelSize = 5;
      blur.repeat     = 1;
      cmds.pipelineSteps.emplace_back(settings::PipelineStep{.$blur = blur});
    }
    {
      joda::settings::LaplacianSettings laplace;
      laplace.kernelSize = 3;
      laplace.repeat     = 1;
      cmds.pipelineSteps.emplace_back(settings::PipelineStep{.$laplacian = laplace});
    }
    mTrainerSettings.featureExtractionPipelines.push_back(cmds);
  }

  // --- Weighted deviation (Gaussian-weighted std) ---
  if(features.contains(TrainingFeatures::WeightedDeviation)) {
    ml::ImageCommandPipeline cmds;
    {
      joda::settings::WeightedDeviationSettings weightedDev;
      weightedDev.kernelSize = 5;
      weightedDev.sigma      = 1;
      cmds.pipelineSteps.emplace_back(settings::PipelineStep{.$gaussianWeightedDev = weightedDev});
    }
    mTrainerSettings.featureExtractionPipelines.push_back(cmds);
  }

  // --- Gradient magnitude (Sobel) ---
  if(features.contains(TrainingFeatures::GradientMagnitude)) {
    ml::ImageCommandPipeline cmds;
    {
      joda::settings::EdgeDetectionSobelSettings gradientMag;
      gradientMag.kernelSize       = 5;
      gradientMag.derivativeOrderX = 1;
      gradientMag.derivativeOrderY = 1;
      gradientMag.weighFunction    = joda::settings::EdgeDetectionSobelSettings::WeightFunction::MAGNITUDE;

      cmds.pipelineSteps.emplace_back(settings::PipelineStep{.$sobel = gradientMag});
    }
    mTrainerSettings.featureExtractionPipelines.push_back(cmds);
  }

  /*
  // --- Structure tensor eigenvalues & coherence ---
  if(features.contains(TrainingFeatures::StructureTensorEigenvalues) || features.contains(TrainingFeatures::StructureTensorCoherence)) {
    cv::Mat gx;
    cv::Mat gy;
    cv::Sobel(gray, gx, CV_32F, 1, 0, 3);
    cv::Sobel(gray, gy, CV_32F, 0, 1, 3);

    cv::Mat Jxx = gx.mul(gx);
    cv::Mat Jyy = gy.mul(gy);
    cv::Mat Jxy = gx.mul(gy);

    // Smooth tensor components
    cv::GaussianBlur(Jxx, Jxx, cv::Size(5, 5), 1.0);
    cv::GaussianBlur(Jyy, Jyy, cv::Size(5, 5), 1.0);
    cv::GaussianBlur(Jxy, Jxy, cv::Size(5, 5), 1.0);

    // Eigenvalues: λ1, λ2
    cv::Mat tmp = (Jxx - Jyy).mul(Jxx - Jyy) + 4 * Jxy.mul(Jxy);
    cv::sqrt(tmp, tmp);
    cv::Mat l1 = 0.5 * (Jxx + Jyy + tmp);
    cv::Mat l2 = 0.5 * (Jxx + Jyy - tmp);

    if(features.contains(TrainingFeatures::StructureTensorEigenvalues)) {
      featureMaps.push_back(l1);
      featureMaps.push_back(l2);
    }

    if(features.contains(TrainingFeatures::StructureTensorCoherence)) {
      cv::Mat coherence = (l1 - l2) / (l1 + l2 + 1e-6);
      featureMaps.push_back(coherence);
    }
  }

  // --- Hessian determinant & eigenvalues ---
  if(features.contains(TrainingFeatures::HessianDeterminant) || features.contains(TrainingFeatures::HessianEigenvalues)) {
    cv::Mat dxx;
    cv::Mat dyy;
    cv::Mat dxy;
    cv::Sobel(gray, dxx, CV_32F, 2, 0, 3);
    cv::Sobel(gray, dyy, CV_32F, 0, 2, 3);
    cv::Sobel(gray, dxy, CV_32F, 1, 1, 3);

    if(features.contains(TrainingFeatures::HessianDeterminant)) {
      cv::Mat detH = dxx.mul(dyy) - dxy.mul(dxy);
      featureMaps.push_back(detH);
    }

    if(features.contains(TrainingFeatures::HessianEigenvalues)) {
      // Eigenvalues of Hessian
      cv::Mat tmp = (dxx - dyy).mul(dxx - dyy) + 4 * dxy.mul(dxy);
      cv::sqrt(tmp, tmp);
      cv::Mat l1 = 0.5 * (dxx + dyy + tmp);
      cv::Mat l2 = 0.5 * (dxx + dyy - tmp);
      featureMaps.push_back(l1);
      featureMaps.push_back(l2);
    }
  }*/
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
