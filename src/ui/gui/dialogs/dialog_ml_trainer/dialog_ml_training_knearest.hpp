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
#include "backend/commands/classification/pixel_classifier/pixel_classifier_training_settings.hpp"
#include "ui/gui/helper/iconless_dialog_button_box.hpp"

namespace joda::ui::gui {

class SettingsKNearest : public QDialog
{
public:
  SettingsKNearest(joda::settings::KNearestTrainingSettings *settings, QWidget *parent) : QDialog(parent), mSettings(settings)
  {
    setWindowTitle("K-Nearest");
    auto *layout = new QFormLayout();

    mDefaultK = new QSpinBox();
    mDefaultK->setMinimum(1);
    mDefaultK->setMaximum(100);
    mDefaultK->setSingleStep(1);
    layout->addRow("Default K", mDefaultK);

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
    mDefaultK->setValue(mSettings->defaultK);
  }

  void toSettings()
  {
    mSettings->defaultK = mDefaultK->value();
  };

  void accept() override
  {
    toSettings();
    QDialog::accept();
  }

private:
  /////////////////////////////////////////////////////
  QSpinBox *mDefaultK;

  /////////////////////////////////////////////////////
  joda::settings::KNearestTrainingSettings *mSettings;
};

}    // namespace joda::ui::gui
