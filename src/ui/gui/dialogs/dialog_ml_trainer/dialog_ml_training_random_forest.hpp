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
#include "backend/commands/classification/pixel_classifier/machine_learning/random_forest/random_forest_settings.hpp"
#include "ui/gui/helper/iconless_dialog_button_box.hpp"

namespace joda::ui::gui {

class SettingsRandomForest : public QDialog
{
public:
  SettingsRandomForest(joda::ml::RandomForestTrainingSettings *settings, QWidget *parent) : QDialog(parent), mSettings(settings)
  {
    setWindowTitle("Random forest");
    auto *layout = new QFormLayout();

    mTreeDepth = new QSpinBox();
    mTreeDepth->setMinimum(1);
    mTreeDepth->setMaximum(100);
    mTreeDepth->setSingleStep(1);
    layout->addRow("Maximum tree depth", mTreeDepth);

    mMinSampleCount = new QSpinBox();
    mMinSampleCount->setMinimum(1);
    mMinSampleCount->setMaximum(100);
    mMinSampleCount->setSingleStep(1);
    layout->addRow("Minimum sample count", mMinSampleCount);

    mMaxNumberOfTrees = new QSpinBox();
    mMaxNumberOfTrees->setMinimum(1);
    mMaxNumberOfTrees->setMaximum(100);
    mMaxNumberOfTrees->setSingleStep(1);
    layout->addRow("Maximum number of trees", mMaxNumberOfTrees);

    mTerminationEpsilon = new QDoubleSpinBox();
    mTerminationEpsilon->setMinimum(0);
    mTerminationEpsilon->setMaximum(1);
    mTerminationEpsilon->setSingleStep(0.001);
    mTerminationEpsilon->setDecimals(8);
    layout->addRow("Termination epsilon", mTerminationEpsilon);

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
    mTreeDepth->setValue(mSettings->maxTreeDepth);
    mMinSampleCount->setValue(mSettings->minSampleCount);
    mMaxNumberOfTrees->setValue(mSettings->maxNumberOfTrees);
    mTerminationEpsilon->setValue(mSettings->terminationEpsilon);
  }

  void toSettings()
  {
    mSettings->maxTreeDepth       = mTreeDepth->value();
    mSettings->minSampleCount     = mMinSampleCount->value();
    mSettings->maxNumberOfTrees   = mMaxNumberOfTrees->value();
    mSettings->terminationEpsilon = mTerminationEpsilon->value();
  };

  void accept() override
  {
    toSettings();
    QDialog::accept();
  }

private:
  /////////////////////////////////////////////////////
  QSpinBox *mTreeDepth;
  QSpinBox *mMinSampleCount;
  QSpinBox *mMaxNumberOfTrees;
  QDoubleSpinBox *mTerminationEpsilon;

  /////////////////////////////////////////////////////
  joda::ml::RandomForestTrainingSettings *mSettings;
};

}    // namespace joda::ui::gui
