///
/// \file      dialog_command_selection.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "dialog_command_selection.hpp"
#include <qboxlayout.h>
#include <qdialog.h>
#include <qtablewidget.h>
#include "backend/settings/pipeline/pipeline_factory.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogCommandSelection::DialogCommandSelection(WindowMain *parent) : QDialog(parent), mParent(parent)
{
  auto *layout = new QVBoxLayout();

  mCommands = new QTableWidget();
  mCommands->setColumnCount(2);
  mCommands->setHorizontalHeaderLabels({"", "Command"});

  connect(mCommands, &QTableWidget::cellDoubleClicked, [&](int row, int column) {
    // Open results
    // mWindowMain->showPanelResults();
    // mWindowMain->getPanelResults()->openFromFile(mLastLoadedResults->item(row, 0)->text());
  });

  layout->addWidget(mCommands);
  addCommandsToTable();
  setLayout(layout);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::shared_ptr<joda::ui::Command> DialogCommandSelection::generateCommand(const settings::PipelineStep &step)
{
  return joda::settings::PipelineFactory<joda::ui::Command>::generate(step, mParent);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogCommandSelection::addCommandsToTable()
{
  addCommandToTable(settings::PipelineStep{.$blur = {}});
  addCommandToTable(settings::PipelineStep{.$saveImage = {}});
  addCommandToTable(settings::PipelineStep{.$threshold = {}});
  addCommandToTable(settings::PipelineStep{.$watershed = {}});
  addCommandToTable(settings::PipelineStep{.$imageFromClass = {}});
  addCommandToTable(settings::PipelineStep{.$classify = {}});
  addCommandToTable(settings::PipelineStep{.$aiClassify = {}});
  addCommandToTable(settings::PipelineStep{.$colocalization = {}});
  addCommandToTable(settings::PipelineStep{.$intersection = {}});
  addCommandToTable(settings::PipelineStep{.$measure = {}});
  addCommandToTable(settings::PipelineStep{.$rollingBall = {}});
  addCommandToTable(settings::PipelineStep{.$medianSubtract = {}});
  addCommandToTable(settings::PipelineStep{.$crop = {}});
  addCommandToTable(settings::PipelineStep{.$edgeDetection = {}});
  addCommandToTable(settings::PipelineStep{.$voronoi = {}});
  addCommandToTable(settings::PipelineStep{.$thresholdValidator = {}});
  addCommandToTable(settings::PipelineStep{.$noiseValidator = {}});
}

void DialogCommandSelection::addCommandToTable(const settings::PipelineStep &step)
{
  auto cmd = generateCommand(step);
  mCommandList.emplace_back(step);

  int newRow = mCommands->rowCount();
  mCommands->insertRow(newRow);
  // Set the icon in the first column
  QTableWidgetItem *iconItem = new QTableWidgetItem();
  iconItem->setIcon(cmd->getIcon());
  mCommands->setItem(newRow, 0, iconItem);

  // Set text in the second column (optional)
  QTableWidgetItem *textItem = new QTableWidgetItem(cmd->getTitle());
  mCommands->setItem(newRow, 1, textItem);
}

}    // namespace joda::ui
