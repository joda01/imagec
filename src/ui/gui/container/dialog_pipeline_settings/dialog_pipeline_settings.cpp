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
DialogPipelineSettings::DialogPipelineSettings(joda::settings::PipelineSettings &settings, WindowMain *windowMain) :
    QDialog(windowMain), mSettings(settings)
{
  auto *formLayout = new QFormLayout;

  auto addSeparator = [&formLayout]() {
    auto *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    formLayout->addRow(separator);
  };

  mPipelineName = new QLineEdit();
  formLayout->addRow("Pipeline name", mPipelineName);

  mCStackIndex = new QLineEdit();
  formLayout->addRow("Image channel", mCStackIndex);

  zProjection = new QComboBox();
  formLayout->addRow("z-projection", zProjection);

  zStackIndex = new QLineEdit();
  formLayout->addRow("z-index", zStackIndex);

  tStackIndex = new QLineEdit();
  formLayout->addRow("t-index", tStackIndex);

  defaultClassId = new QComboBox();

  mPipelineNotes = new QTextEdit();
  formLayout->addRow("Notes", mPipelineNotes);

  setLayout(formLayout);
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
  onZProjectionChanged();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogPipelineSettings::toSettings()
{
  // mSettings.disabled                     = mActionDisabled->isChecked();
  // mSettings.meta.name                    = pipelineName->getValue();
  // mSettings.meta.notes                   = pipelineNotes->getValue();
  // mSettings.pipelineSetup.cStackIndex    = cStackIndex->getValue();
  // mSettings.pipelineSetup.zProjection    = zProjection->getValue();
  // mSettings.pipelineSetup.defaultClassId = defaultClassId->getValue();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogPipelineSettings::onZProjectionChanged()
{
  // if(zProjection->getValue() == enums::ZProjection::NONE) {
  //   zStackIndex->getEditableWidget()->setVisible(true);
  // } else {
  //   zStackIndex->getEditableWidget()->setVisible(false);
  // }
}

}    // namespace joda::ui::gui
