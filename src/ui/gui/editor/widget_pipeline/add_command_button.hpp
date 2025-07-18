///
/// \file      add_command_button.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <qpushbutton.h>
#include <memory>
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "ui/gui/editor/widget_pipeline/widget_command/command.hpp"
#include "panel_pipeline_settings.hpp"

namespace joda::ui::gui {

class WindowMain;
class PanelPipelineSettings;
class DialogCommandSelection;

///
/// \class
/// \author
/// \brief
///
class AddCommandButtonBase : public QWidget
{
public:
  AddCommandButtonBase(std::shared_ptr<DialogCommandSelection> &dialogCommandSelection, joda::settings::Pipeline &settings,
                       PanelPipelineSettings *pipelineStepSettingsUi, const settings::PipelineStep *pipelineStepBefore, InOuts outOfStepBefore,
                       WindowMain *parent);

  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void enterEvent(QEnterEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void setInOutBefore(InOuts inout);

public slots:
  void onAddCommandClicked();

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<DialogCommandSelection> mDialogCommandSelection;
  WindowMain *mParent;
  bool mMouseEntered                                = false;
  const settings::PipelineStep *mPipelineStepBefore = nullptr;
  joda::settings::Pipeline &mSettings;
  PanelPipelineSettings *pipelineStepSettingsUi;
  InOuts mOutOfStepBefore;
};

}    // namespace joda::ui::gui
