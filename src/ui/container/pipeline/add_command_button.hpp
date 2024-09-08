///
/// \file      add_command_button.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qpushbutton.h>
#include <memory>
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "panel_pipeline_settings.hpp"

namespace joda::ui {

class WindowMain;
class PanelPipelineSettings;

///
/// \class
/// \author
/// \brief
///
class AddCommandButtonBase : public QWidget
{
public:
  AddCommandButtonBase(joda::settings::Pipeline &settings, PanelPipelineSettings *pipelineStepSettingsUi,
                       const settings::PipelineStep *pipelineStepBefore, WindowMain *parent);

  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void enterEvent(QEnterEvent *event) override;
  void leaveEvent(QEvent *event) override;

public slots:
  void onAddCommandClicked();

private:
  /////////////////////////////////////////////////////
  WindowMain *mParent;
  bool mMouseEntered                                = false;
  const settings::PipelineStep *mPipelineStepBefore = nullptr;
  joda::settings::Pipeline &mSettings;
  PanelPipelineSettings *pipelineStepSettingsUi;
};

}    // namespace joda::ui
