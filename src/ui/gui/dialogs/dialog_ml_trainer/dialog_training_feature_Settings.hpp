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
#include "ui/gui/helper/iconless_dialog_button_box.hpp"
#include "ui/gui/helper/multicombobox.hpp"
#include "training_features_settings.hpp"

namespace joda::ui::gui {

class DialogTrainingFeatureSettings : public QDialog
{
public:
  DialogTrainingFeatureSettings(TrainingFeaturesSettings *settings, QWidget *parent) : QDialog(parent), mSettings(settings)
  {
    setWindowTitle("Feature settings");
    auto *layout = new QFormLayout();

    auto createKernelMultiSelect = []() -> QComboBoxMulti * {
      auto *combo = new QComboBoxMulti();
      combo->addItem("3x3", static_cast<int32_t>(3));
      combo->addItem("5x5", static_cast<int32_t>(5));
      combo->addItem("7x7", static_cast<int32_t>(7));
      combo->addItem("9x9", static_cast<int32_t>(9));
      return combo;
    };

    mBlurKernelSizeVariation = createKernelMultiSelect();
    layout->addRow("Blur kernel size variation", mBlurKernelSizeVariation);

    mWeightedDeviationKernelSizeVariation = createKernelMultiSelect();
    layout->addRow("Weight deviation size variation", mWeightedDeviationKernelSizeVariation);

    mGradientMagnitudeKernelSizeVariation = createKernelMultiSelect();
    layout->addRow("Gradient magnitude kernel variation", mGradientMagnitudeKernelSizeVariation);

    mGradientMagnitudeKernelSizeVariation = createKernelMultiSelect();
    layout->addRow("Gradient magnitude kernel variation", mGradientMagnitudeKernelSizeVariation);

    mStructureTensorKernelSizeVariation = createKernelMultiSelect();
    layout->addRow("Structure tensor magnitude kernel variation", mStructureTensorKernelSizeVariation);

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
    {
      QVariantList toCheck;
      for(const auto &item : mSettings->blurKernelSizeVariation) {
        toCheck.emplace_back(static_cast<int>(item));
      }
      mBlurKernelSizeVariation->setCheckedItems(toCheck);
    }

    {
      QVariantList toCheck;
      for(const auto &item : mSettings->weightedDeviationKernelSizeVariation) {
        toCheck.emplace_back(static_cast<int>(item));
      }
      mWeightedDeviationKernelSizeVariation->setCheckedItems(toCheck);
    }

    {
      QVariantList toCheck;
      for(const auto &item : mSettings->gradientMagnitudeKernelSizeVariation) {
        toCheck.emplace_back(static_cast<int>(item));
      }
      mGradientMagnitudeKernelSizeVariation->setCheckedItems(toCheck);
    }

    {
      QVariantList toCheck;
      for(const auto &item : mSettings->structureTensorKernelSizeVariation) {
        toCheck.emplace_back(static_cast<int>(item));
      }
      mStructureTensorKernelSizeVariation->setCheckedItems(toCheck);
    }
  }

  void toSettings()
  {
    {
      mSettings->blurKernelSizeVariation.clear();
      const auto &items = mBlurKernelSizeVariation->getCheckedItems();
      for(const auto &item : items) {
        mSettings->blurKernelSizeVariation.emplace(item.first.toInt());
      }
    }

    {
      mSettings->weightedDeviationKernelSizeVariation.clear();
      const auto &items = mWeightedDeviationKernelSizeVariation->getCheckedItems();
      for(const auto &item : items) {
        mSettings->weightedDeviationKernelSizeVariation.emplace(item.first.toInt());
      }
    }

    {
      mSettings->gradientMagnitudeKernelSizeVariation.clear();
      const auto &items = mGradientMagnitudeKernelSizeVariation->getCheckedItems();
      for(const auto &item : items) {
        mSettings->gradientMagnitudeKernelSizeVariation.emplace(item.first.toInt());
      }
    }

    {
      mSettings->structureTensorKernelSizeVariation.clear();
      const auto &items = mStructureTensorKernelSizeVariation->getCheckedItems();
      for(const auto &item : items) {
        mSettings->structureTensorKernelSizeVariation.emplace(item.first.toInt());
      }
    }
  }

  void accept() override
  {
    toSettings();
    QDialog::accept();
  }

private:
  /////////////////////////////////////////////////////
  QComboBoxMulti *mBlurKernelSizeVariation;
  QComboBoxMulti *mWeightedDeviationKernelSizeVariation;
  QComboBoxMulti *mGradientMagnitudeKernelSizeVariation;
  QComboBoxMulti *mStructureTensorKernelSizeVariation;

  /////////////////////////////////////////////////////
  TrainingFeaturesSettings *mSettings;
};

}    // namespace joda::ui::gui
