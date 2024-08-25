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

namespace joda::settings {
class PipelineStep;
}

namespace joda::ui {

class WindowMain;
class Command;

///
/// \class      DialogCommandSelection
/// \author     Joachim Danmayr
/// \brief      Dialog which allows to select a command from a list
///
class DialogCommandSelection : public QDialog
{
public:
  /////////////////////////////////////////////////////
  DialogCommandSelection(WindowMain *parent);

private:
  /////////////////////////////////////////////////////
  void addCommandsToTable();
  void addCommandToTable(const settings::PipelineStep &step);
  std::shared_ptr<joda::ui::Command> generateCommand(const settings::PipelineStep &step);

  /////////////////////////////////////////////////////
  QTableWidget *mCommands;
  WindowMain *mParent;
  std::vector<settings::PipelineStep> mCommandList;
};

}    // namespace joda::ui
