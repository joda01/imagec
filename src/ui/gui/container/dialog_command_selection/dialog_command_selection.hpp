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
  DialogCommandSelection(WindowMain *parent);

  void show(const settings::PipelineStep *pipelineStepBefore, InOuts outOfStepBefore, joda::settings::Pipeline *settings,
            PanelPipelineSettings *pipelineStepSettingsUi);

private:
  enum class Group
  {
    IMAGE_PROCESSING        = 0,
    BINARY_IMAGE_PROCESSING = 1,
    CLASSIFICATION          = 2,
    OBJECT_PROCESSING       = 3,
    MEASUREMENT             = 4,
    FILTERING               = 5,
    OUTPUT                  = 6,
    LAST_GROUP              = 7
  };

  /////////////////////////////////////////////////////
  struct CommandTableFilter
  {
    QString searchText;
    InOuts outOfStepBefore = InOuts::ALL;
  };

  struct CommandListEntry
  {
    settings::PipelineStep pipelineStep;
    InOut inOuts;
    QString name;
    QString description;
    Group group;
    std::vector<std::string> tags;
  };

  /////////////////////////////////////////////////////
  void addNewCommand(int commandListIdx);
  void addTitleToTable(const std::string &title, Group group);
  void addCommandsToTable();
  int addCommandToTable(const settings::PipelineStep &step, Group group);
  void filterCommands(const CommandTableFilter &filter);
  std::unique_ptr<joda::ui::gui::Command> generateCommand(const settings::PipelineStep &step);
  bool eventFilter(QObject *obj, QEvent *event) override;

  /////////////////////////////////////////////////////
  QLineEdit *mSearch = nullptr;
  QTableWidget *mCommands;
  std::vector<CommandListEntry> mCommandList;
  std::map<int32_t, int32_t> mCommandIndexMap;    // Key is the array index of the command in the mCommandList vector, value the index in the table
  std::map<Group, int32_t> mTitleINdex;           // Key is the group ID, value the index in the table

  WindowMain *mParent;
  const settings::PipelineStep *mPipelineStepBefore = nullptr;
  joda::settings::Pipeline *mSettings               = nullptr;
  PanelPipelineSettings *mPipelineStepSettingsUi    = nullptr;
  InOuts mOutOfStepBefore;
};

}    // namespace joda::ui::gui
