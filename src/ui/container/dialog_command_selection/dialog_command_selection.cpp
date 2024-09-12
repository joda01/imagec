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
#include <memory>
#include <string>
#include "backend/commands/image_functions/blur/blur_settings.hpp"
#include "backend/commands/image_functions/blur/blur_settings_ui.hpp"
#include "backend/commands/image_functions/image_saver/image_saver_settings.hpp"
#include "backend/commands/image_functions/threshold/threshold_settings.hpp"
#include "backend/commands/image_functions/watershed/watershed_settings.hpp"
#include "backend/commands/object_functions/validator_threshold/validator_threshold_settings.hpp"
#include "backend/commands/object_functions/voronoi_grid/voronoi_grid_settings.hpp"
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
DialogCommandSelection::DialogCommandSelection(joda::settings::Pipeline &settings,
                                               PanelPipelineSettings *pipelineStepSettingsUi,
                                               const settings::PipelineStep *pipelineStepBefore, WindowMain *parent) :
    QDialog(parent),
    mParent(parent), mPipelineStepBefore(pipelineStepBefore), mSettings(settings),
    pipelineStepSettingsUi(pipelineStepSettingsUi)
{
  auto *layout = new QVBoxLayout();

  mCommands = new QTableWidget();
  mCommands->setColumnCount(2);
  mCommands->setColumnHidden(0, true);
  mCommands->setHorizontalHeaderLabels({"", "Command"});
  mCommands->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  mCommands->verticalHeader()->setVisible(false);

  connect(mCommands, &QTableWidget::cellDoubleClicked, [&](int row, int column) {
    // Open results
    // mWindowMain->showPanelResults();
    // mWindowMain->getPanelResults()->openFromFile(mLastLoadedResults->item(row, 0)->text());
    auto idx = mCommands->item(row, 0)->text().toInt();
    addNewCommand(idx);
    close();
  });

  layout->addWidget(mCommands);
  addCommandsToTable();
  setLayout(layout);
  setMinimumHeight(350);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::unique_ptr<joda::ui::Command> DialogCommandSelection::generateCommand(const settings::PipelineStep &step)
{
  return std::move(joda::settings::PipelineFactory<joda::ui::Command>::generate(step, mParent));
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
  addTitleToTable("Image Processing");
  addCommandToTable(settings::PipelineStep{.$crop = settings::MarginCropSettings{}});
  addCommandToTable(settings::PipelineStep{.$blur = settings::BlurSettings{}});
  addCommandToTable(settings::PipelineStep{.$rollingBall = settings::RollingBallSettings{}});
  addCommandToTable(settings::PipelineStep{.$medianSubtract = settings::MedianSubtractSettings{}});
  addCommandToTable(settings::PipelineStep{.$edgeDetection = settings::EdgeDetectionSettings{}});

  addTitleToTable("Binary image Processing");
  addCommandToTable(settings::PipelineStep{.$threshold = settings::ThresholdSettings{}});
  addCommandToTable(settings::PipelineStep{.$watershed = settings::WatershedSettings{}});

  addTitleToTable("Classification");
  settings::ClassifierSettings defaultClassify;
  defaultClassify.modelClasses = {{.modelClassId = 65535}};
  addCommandToTable(settings::PipelineStep{.$classify = defaultClassify});
  addCommandToTable(settings::PipelineStep{.$aiClassify = settings::AiClassifierSettings{}});

  addTitleToTable("Object Processing");
  addCommandToTable(settings::PipelineStep{.$voronoi = settings::VoronoiGridSettings{}});
  addCommandToTable(settings::PipelineStep{.$intersection = settings::IntersectionSettings{}});

  addTitleToTable("Measurement");
  addCommandToTable(settings::PipelineStep{.$colocalization = settings::ColocalizationSettings{}});
  addCommandToTable(settings::PipelineStep{.$measure = settings::MeasureSettings{}});

  addTitleToTable("Filtering");
  addCommandToTable(settings::PipelineStep{.$thresholdValidator = settings::ThresholdValidatorSettings{}});
  addCommandToTable(settings::PipelineStep{.$noiseValidator = settings::NoiseValidatorSettings{}});

  addTitleToTable("Output");
  addCommandToTable(settings::PipelineStep{.$saveImage = settings::ImageSaverSettings{}});
  addCommandToTable(settings::PipelineStep{.$imageFromClass = settings::ImageFromClassSettings{}});
}

void DialogCommandSelection::addTitleToTable(const std::string &title)
{
  int newRow = mCommands->rowCount();
  mCommands->insertRow(newRow);
  auto *iconItem = new QTableWidgetItem();
  iconItem->setFlags(iconItem->flags() & ~Qt::ItemIsSelectable);
  QFont font;
  font.setBold(true);
  iconItem->setFont(font);    // Set the font to Arial, 12 points, and bold

  // iconItem->setIcon(cmd->getIcon());
  iconItem->setText(title.data());
  iconItem->setFlags(iconItem->flags() & ~Qt::ItemIsEditable);
  mCommands->setItem(newRow, 1, iconItem);

  auto *vectorIndex = new QTableWidgetItem();
  vectorIndex->setText("");
  mCommands->setItem(newRow, 0, vectorIndex);
}

void DialogCommandSelection::addCommandToTable(const settings::PipelineStep &step)
{
  std::unique_ptr<joda::ui::Command> cmd = joda::settings::PipelineFactory<joda::ui::Command>::generate(step, nullptr);
  if(cmd != nullptr) {
    mCommandList.emplace_back(step);
    int newRow = mCommands->rowCount();
    mCommands->insertRow(newRow);
    // Set the icon in the first column
    auto *iconItem = new QTableWidgetItem();
    iconItem->setIcon(cmd->getIcon());
    iconItem->setText(cmd->getTitle());
    iconItem->setFlags(iconItem->flags() & ~Qt::ItemIsEditable);
    mCommands->setItem(newRow, 1, iconItem);

    auto *vectorIndex = new QTableWidgetItem();
    vectorIndex->setText(std::to_string(mCommandList.size() - 1).data());
    mCommands->setItem(newRow, 0, vectorIndex);
  }
  cmd.reset();
}

void DialogCommandSelection::addNewCommand(int commandListIdx)
{
  if(mPipelineStepBefore == nullptr) {
    auto inserted = mSettings.pipelineSteps.insert(mSettings.pipelineSteps.begin(), mCommandList[commandListIdx]);
    std::unique_ptr<joda::ui::Command> cmd =
        joda::settings::PipelineFactory<joda::ui::Command>::generate(*inserted, mParent);
    pipelineStepSettingsUi->insertNewPipelineStep(0, std::move(cmd), &*inserted);
    return;
  }

  int posInserted = 0;
  for(auto it = mSettings.pipelineSteps.begin(); it != mSettings.pipelineSteps.end(); ++it) {
    posInserted++;
    if(&*it == mPipelineStepBefore) {
      const auto &inserted = mSettings.pipelineSteps.insert(std::next(it), mCommandList[commandListIdx]);
      std::unique_ptr<joda::ui::Command> cmd =
          joda::settings::PipelineFactory<joda::ui::Command>::generate(*inserted, mParent);
      pipelineStepSettingsUi->insertNewPipelineStep(posInserted, std::move(cmd), &*inserted);
      return;
    }
  }
}

}    // namespace joda::ui
