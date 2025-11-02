///
/// \file      dialog_roi_manager.hpp
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qcombobox.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qwidget.h>
#include <memory>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"
#include "backend/commands/classification/pixel_classifier/pixel_classifier.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "controller/controller.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox_classes_out.hpp"
#include "ui/gui/helper/table_view.hpp"
#include "ui/gui/helper/table_widget.hpp"

class QComboBoxMulti;

namespace joda::ui::gui {

class WindowMain;
class PanelImageView;
class TableModelPaintedPolygon;

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class DialogMlTrainer : public QDialog
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogMlTrainer(const joda::settings::AnalyzeSettings *, const std::shared_ptr<atom::ObjectList> &objectMap, PanelImageView *imagePanel,
                  QWidget *parent);

signals:
  void dialogDisappeared();    // custom signal
  void trainingFinished(bool okay, QString message);
  void triggerPreviewUpdate();

protected:
  void hideEvent(QHideEvent *event) override;
  void closeEvent(QCloseEvent *event) override;
  int32_t getSelectedPixelClass() const;

private:
  enum class TrainingFeatures
  {
    Intensity,
    Gaussian,
    LaplacianOfGaussian,
    WeightedDeviation,
    GradientMagnitude,
    StructureTensorEigenvalues,
    StructureTensorCoherence,
    HessianDeterminant,
    HessianEigenvalues
  };

  /////////////////////////////////////////////////////
  void startTraining();
  void stopTraining();
  void setInProgress(bool);
  void buildFeatureExtractionPipeline();

  /////////////////////////////////////////////////////
  PanelImageView *mImagePanel;
  QComboBox *mComboClassifierMethod;
  QComboBox *mRoiSource;
  QComboBoxMulti *mComboTrainingFeatures;
  QLineEdit *mModelName;
  QPushButton *mButtonStartTraining;
  QPushButton *mButtonStopTraining;
  QProgressBar *mProgress;
  std::shared_ptr<atom::ObjectList> mObjectMap;
  QTextBrowser *mTrainingsLog;

  joda::ml::MachineLearningSettings mTrainerSettings;
  joda::cmd::TrainingsModelSettings mModelSettings;
  std::unique_ptr<std::thread> mTrainingsThread;

  const joda::settings::AnalyzeSettings *mAnalyzeSettings;

private slots:
  void onTrainingFinished(bool okay, QString message);
};

}    // namespace joda::ui::gui
