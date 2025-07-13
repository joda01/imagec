///
/// \file      dialog_pipeline_settings.cpp
/// \author    Joachim Danmayr
/// \date      2025-07-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "dialog_pipeline_settings.hpp"
#include <qdialog.h>
#include <qformlayout.h>
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "ui/gui/container/setting/setting_base.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
DialogPipelineSettings::DialogPipelineSettings(const joda::settings::Classification &classes, joda::settings::Pipeline &settings,
                                               WindowMain *windowMain) :
    QDialog(windowMain),
    mSettings(settings)
{
  setWindowTitle("Pipeline settings");
  setMinimumSize(500, 400);
  auto *formLayout = new QFormLayout;

  auto addSeparator = [&formLayout]() {
    auto *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    formLayout->addRow(separator);
  };

  mPipelineName = new QLineEdit();
  formLayout->addRow("Pipeline name", mPipelineName);

  mCStackIndex = new QComboBox();
  mCStackIndex->addItem("Undefined", -2);
  mCStackIndex->addItem("None", -1);
  mCStackIndex->addItem("CH0", 0);
  mCStackIndex->addItem("CH1", 1);
  mCStackIndex->addItem("CH2", 2);
  mCStackIndex->addItem("CH3", 3);
  mCStackIndex->addItem("CH4", 4);
  mCStackIndex->addItem("CH5", 5);
  mCStackIndex->addItem("CH6", 6);
  mCStackIndex->addItem("CH7", 7);
  mCStackIndex->addItem("CH8", 8);
  mCStackIndex->addItem("CH9", 9);
  formLayout->addRow("Image channel", mCStackIndex);

  mClass = new QComboBox();
  mClass->addItem("None", static_cast<int32_t>(enums::ClassId::NONE));
  for(const auto &classs : classes.classes) {
    mClass->addItem(classs.name.data(), static_cast<int32_t>(classs.classId));
  }
  formLayout->addRow("Output class", mClass);

  zProjection = new QComboBox();
  zProjection->addItem("None", (int32_t) enums::ZProjection::NONE);
  zProjection->addItem("Max. intensity", (int32_t) enums::ZProjection::MAX_INTENSITY);
  zProjection->addItem("Min. intensity", (int32_t) enums::ZProjection::MIN_INTENSITY);
  zProjection->addItem("Avg. intensity", (int32_t) enums::ZProjection::AVG_INTENSITY);
  zProjection->addItem("Take middle", (int32_t) enums::ZProjection::TAKE_MIDDLE);
  formLayout->addRow("z-projection", zProjection);

  zStackIndex = new QLineEdit();
  zStackIndex->setPlaceholderText("Z-Stack to analyze [0 - 65535]");
  formLayout->addRow("z-index", zStackIndex);

  tStackIndex = new QLineEdit();
  tStackIndex->setPlaceholderText("T-Stack to analyze [0 - 65535]");
  formLayout->addRow("t-index", tStackIndex);

  defaultClassId = new QComboBox();

  mPipelineNotes = new QTextEdit();
  formLayout->addRow("Notes", mPipelineNotes);

  // Okay and canlce
  auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  formLayout->addWidget(buttonBox);

  setLayout(formLayout);

  fromSettings();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogPipelineSettings::accept()
{
  bool okay      = false;
  int32_t zStack = -1;
  if(!zStackIndex->text().isEmpty()) {
    zStack = zStackIndex->text().toInt(&okay);
    if(!okay) {
      QMessageBox::warning(this, "Not a number...", "Given z-stack value is not a valid number!");
      return;
    }
  }

  int32_t tStack = -1;
  if(!tStackIndex->text().isEmpty()) {
    tStack = tStackIndex->text().toInt(&okay);
    if(!okay) {
      QMessageBox::warning(this, "Not a number...", "Given t-stack value is not a valid number!");
      return;
    }
  }

  if(mCStackIndex->currentData().toInt() < -1) {
    QMessageBox::warning(this, "No image channel selected...", "Select an image channel!");
    return;
  }

  mSettings.meta.name                    = mPipelineName->text().toStdString();
  mSettings.meta.notes                   = mPipelineNotes->toPlainText().toStdString();
  mSettings.pipelineSetup.cStackIndex    = mCStackIndex->currentData().toInt();
  mSettings.pipelineSetup.zStackIndex    = zStack;
  mSettings.pipelineSetup.tStackIndex    = tStack;
  mSettings.pipelineSetup.defaultClassId = (enums::ClassId) mClass->currentData().toInt();
  mSettings.pipelineSetup.zProjection    = (enums::ZProjection) zProjection->currentData().toInt();

  QDialog::accept();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogPipelineSettings::fromSettings()
{
  mPipelineName->setText(mSettings.meta.name.data());
  mCStackIndex->setCurrentIndex(mSettings.pipelineSetup.cStackIndex + 2);

  switch(mSettings.pipelineSetup.zProjection) {
    case enums::ZProjection::$:
    case enums::ZProjection::UNDEFINED:
    case enums::ZProjection::NONE:
      zProjection->setCurrentIndex(0);
      break;
    case enums::ZProjection::MAX_INTENSITY:
      zProjection->setCurrentIndex(1);
      break;
    case enums::ZProjection::MIN_INTENSITY:
      zProjection->setCurrentIndex(2);
      break;
    case enums::ZProjection::AVG_INTENSITY:
      zProjection->setCurrentIndex(3);
      break;
    case enums::ZProjection::TAKE_MIDDLE:
      zProjection->setCurrentIndex(4);
      break;
  }

  for(int idx = 0; idx < mClass->count(); idx++) {
    if(mSettings.pipelineSetup.defaultClassId == (enums::ClassId) mClass->itemData(idx).toInt()) {
      mClass->setCurrentIndex(idx);
      break;
    }
  }

  if(mSettings.pipelineSetup.zStackIndex >= 0) {
    zStackIndex->setText(QString::number((mSettings.pipelineSetup.zStackIndex)));
  } else {
    zStackIndex->setText("");
  }

  if(mSettings.pipelineSetup.tStackIndex >= 0) {
    tStackIndex->setText(QString::number((mSettings.pipelineSetup.tStackIndex)));
  } else {
    tStackIndex->setText("");
  }

  mPipelineNotes->setText(mSettings.meta.notes.data());
}

}    // namespace joda::ui::gui
