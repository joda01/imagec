///
/// \file      dialog_command_selection.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qdialog.h>
#include <qtablewidget.h>
#include <memory>
#include "backend/settings/pipeline/pipeline.hpp"
#include "ui/gui/container/command/command.hpp"

namespace joda::settings {
class PipelineStep;
}

namespace joda::ui::gui {

class WindowMain;
class Command;
class PanelPipelineSettings;

///
/// \class      DialogCommandSelection
/// \author     Joachim Danmayr
/// \brief      Dialog which allows to select a command from a list
///
class DialogCommandSelection : public QDialog
{
public:
  /////////////////////////////////////////////////////
  DialogCommandSelection(joda::settings::Pipeline &settings, PanelPipelineSettings *pipelineStepSettingsUi,
                         const settings::PipelineStep *pipelineStepBefore, InOuts outOfStepBefore, WindowMain *parent);

  void setInOutBefore(InOuts inout);

private:
  /////////////////////////////////////////////////////
  void addNewCommand(int commandListIdx);
  void addTitleToTable(const std::string &title, int position);
  void addCommandsToTable(InOuts outOfStepBefore);
  int addCommandToTable(const settings::PipelineStep &step, InOuts outOfStepBefore);
  std::unique_ptr<joda::ui::gui::Command> generateCommand(const settings::PipelineStep &step);

  /////////////////////////////////////////////////////
  QTableWidget *mCommands;
  WindowMain *mParent;
  std::vector<settings::PipelineStep> mCommandList;
  const settings::PipelineStep *mPipelineStepBefore = nullptr;
  joda::settings::Pipeline &mSettings;
  PanelPipelineSettings *pipelineStepSettingsUi;
};

}    // namespace joda::ui::gui
