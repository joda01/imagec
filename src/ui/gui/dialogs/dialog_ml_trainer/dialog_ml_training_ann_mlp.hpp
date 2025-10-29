///
/// \file      widget_ml_training_random_forest.hpp
/// \author    Joachim Danmayr
/// \date      2025-09-14
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include <qdialog.h>
#include <qformlayout.h>
#include <qspinbox.h>
#include <qwidget.h>
#include <cstdint>
#include "backend/commands/classification/pixel_classifier/machine_learning/ann_mlp/ann_mlp_settings.hpp"
#include "ui/gui/helper/iconless_dialog_button_box.hpp"

namespace joda::ui::gui {

class SettingsAnnMlp : public QDialog
{
public:
  SettingsAnnMlp(joda::ml::AnnMlpTrainingSettings *settings, QWidget *parent) : QDialog(parent), mSettings(settings)
  {
    setWindowTitle("Multi-Layer Perceptron");
    auto *layout = new QFormLayout();

    auto addLayer = [&, this](int32_t idx) {
      auto *layer = new QSpinBox();
      layer->setMinimum(0);
      layer->setMaximum(200);
      layer->setSingleStep(1);
      layer->setSuffix(" neurons");
      layer->setSpecialValueText("Off");
      layout->addRow("Layer " + QString::number(idx), layer);
      mNrNeuronsLayer.emplace_back(layer);
    };

    addLayer(1);
    addLayer(2);
    addLayer(3);
    addLayer(4);
    addLayer(5);
    addLayer(6);

    mMaxIterations = new QSpinBox();
    mMaxIterations->setMinimum(1);
    mMaxIterations->setMaximum(INT32_MAX);
    mMaxIterations->setSingleStep(1);
    layout->addRow("Maximum iterations", mMaxIterations);

    mTerminationEpsilon = new QDoubleSpinBox();
    mTerminationEpsilon->setMinimum(0);
    mTerminationEpsilon->setMaximum(1);
    mTerminationEpsilon->setSingleStep(0.001);
    mTerminationEpsilon->setDecimals(10);
    layout->addRow("Termination epsilon", mTerminationEpsilon);

    mBatchSize = new QSpinBox();
    mBatchSize->setSpecialValueText("Full data set");
    mBatchSize->setMinimum(0);
    mBatchSize->setMaximum(INT32_MAX);
    mBatchSize->setSingleStep(1);
    layout->addRow("Batch size", mBatchSize);

    mLearningRate = new QDoubleSpinBox();
    mLearningRate->setMinimum(0);
    mLearningRate->setMaximum(1);
    mLearningRate->setSingleStep(0.001);
    mLearningRate->setDecimals(4);
    layout->addRow("Learning rate", mLearningRate);

    // Okay and canlce
    auto *buttonBox = new IconlessDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);

    setLayout(layout);
    fromSettings();
  };

  void fromSettings()
  {
    for(size_t idx = 0; idx < mSettings->neuronsLayer.size(); idx++) {
      if(mNrNeuronsLayer.size() > idx) {
        mNrNeuronsLayer.at(idx)->setValue(mSettings->neuronsLayer.at(idx));
      }
    }

    mMaxIterations->setValue(mSettings->maxIterations);
    mTerminationEpsilon->setValue(mSettings->terminationEpsilon);
    mBatchSize->setValue(mSettings->batchSize);
    mLearningRate->setValue(mSettings->learningRate);
  }

  void toSettings()
  {
    mSettings->neuronsLayer.clear();
    for(size_t idx = 0; idx < mNrNeuronsLayer.size(); idx++) {
      auto neuronsNr = mNrNeuronsLayer.at(idx)->value();
      mSettings->neuronsLayer.emplace_back(neuronsNr);
    }

    mSettings->maxIterations      = mMaxIterations->value();
    mSettings->terminationEpsilon = mTerminationEpsilon->value();
    mSettings->batchSize          = mBatchSize->value();
    mSettings->learningRate       = mLearningRate->value();
  };

  void accept() override
  {
    toSettings();
    QDialog::accept();
  }

private:
  /////////////////////////////////////////////////////
  std::vector<QSpinBox *> mNrNeuronsLayer;

  QSpinBox *mMaxIterations;
  QDoubleSpinBox *mTerminationEpsilon;
  QSpinBox *mBatchSize;
  QDoubleSpinBox *mLearningRate;

  /////////////////////////////////////////////////////
  joda::ml::AnnMlpTrainingSettings *mSettings;
};

}    // namespace joda::ui::gui
